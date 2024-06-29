//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiHelpers.h"

#include "GuiParams.h"
#include "GuiUser.h"

#include "ActivityBase.h"
#include "AppletBase.h"
#include "AppCommon.h"
#include "AppDefs.h"
#include "AppTranslate.h"
#include "MyIconsDefs.h"
#include "VxFrame.h"
#include "PluginSettingsWidget.h"

#include <CoreLib/ObjectCommon.h>
#include <CoreLib/ObjectCommonDefs.h>

#include <CoreLib/VFile.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <P2PEngine/P2PEngine.h>
#include <VxVideoLib/VxVideoLib.h>

#include <QDesktopServices>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>

#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QUrl>

#include <QFileSystemModel>

#if defined (Q_OS_ANDROID)
#include <QtQml/QQmlFile>

# if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#  include <QtAndroid>
# else
#  include <QtCore/private/qandroidextras_p.h>
# endif
#endif //defined (Q_OS_ANDROID)

//============================================================================
QString GuiHelpers::getAvailableStorageSpaceText()
{
    return GuiParams::describeFileLength( VxFileUtil::getDiskFreeSpace( VxGetAppDirectory( eAppDirRootDataStorage ).c_str() ) );
}

//============================================================================
std::string GuiHelpers::browseForDirectory( QString startDir, QWidget* parent )
{
    listFilesInFolder( startDir.toUtf8().constData() );

    QFileDialog dialog( parent, QObject::tr("Open Folder"), startDir );
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
	dialog.setFileMode( QFileDialog::Directory );
#else
	dialog.setFileMode(QFileDialog::DirectoryOnly);
#endif // QT_VERSION > QT_VERSION_CHECK(6,0,0)

    dialog.setOptions( QFileDialog::ShowDirsOnly );
    //dialog.setDirectory( startDir );

    QString selectedDir;
	QStringList fileNames;
	if (dialog.exec())
	{
		fileNames = dialog.selectedFiles();
		if( fileNames.size() )
		{
			selectedDir = fileNames[0];
		}
	}

    if( selectedDir.isEmpty() )
    {
        return "";
    }

    std::string folder = selectedDir.toUtf8().constData();

    VxFileUtil::decodePercentEncodingOfSlash( folder );

    VxFileUtil::makeForwardSlashPath( folder );
    VxFileUtil::assureTrailingDirectorySlash( folder );

    listFilesInFolder( folder );

	return folder;
}

void GuiHelpers::listFilesInFolder( std::string folder, uint8_t fileFilterMask )
{
    std::string folderName( folder );
    VxFileUtil::removeTrailingDirectorySlash(folderName);
    VxFileUtil::encodePercentEncodingOfSlash(folderName);


    if( 0 == fileFilterMask )
    {
        fileFilterMask = VXFILE_TYPE_ALLNOTEXE | VXFILE_TYPE_DIRECTORY;
    }

    VxGUID onlineId = GetPtoPEngine().getMyOnlineId();
    QDir browseDir( folderName.c_str() );

    QFileInfoList fileInfoList = browseDir.entryInfoList();
    LogMsg( LOG_VERBOSE, "%d files in dir %s", fileInfoList.size(), folderName.c_str() );
    for( auto fileListInfo : fileInfoList )
    {
        std::string fileName = fileListInfo.filePath().toUtf8().constData();
        VxFileUtil::decodePercentEncodingOfSlash( fileName );
        if( fileName.empty() )
        {
            continue;
        }

        if( fileListInfo.isDir() )
        {
            if( fileFilterMask & VXFILE_TYPE_DIRECTORY )
            {
                VxFileUtil::assureTrailingDirectorySlash( fileName );
                LogMsg( LOG_VERBOSE, "Directory %s", fileName.c_str() );
                //FileInfo dirInfo( onlineId, fileName, 0, VXFILE_TYPE_DIRECTORY );
                //GetPtoPEngine().getToGui().toGuiFileList( appInstId, dirInfo );
            }
        }
        else if( fileListInfo.isExecutable() )
        {
            LogMsg( LOG_VERBOSE, "Executable ignored File %s", fileName.c_str() );
        }
        else if( fileListInfo.isReadable() )
        {
            int64_t fileLen = fileListInfo.size();

            if( fileLen )
            {
                uint8_t fileType = VxFileNameToFileType( fileName );
                if( fileLen && ( fileType & fileFilterMask ) )
                {
                    if( testCanReadFile( fileName ) )
                    {
                        LogMsg( LOG_VERBOSE, "Readable File %s len %" PRId64 " type 0x%x ", fileName.c_str(), fileLen, fileType );
                    }
                    else
                    {
                        LogMsg( LOG_VERBOSE, "NOT Readable File %s len %" PRId64 " type 0x%x ", fileName.c_str(), fileLen, fileType );
                    }


                    // FileInfo fileInfo( onlineId, fileName, fileLen, fileType );
                    // fileInfo.setIsInLibrary( GetPtoPEngine().fromGuiGetIsFileInLibrary( fileName ) );
                    // fileInfo.setIsSharedFile( GetPtoPEngine().fromGuiGetIsFileShared( fileInfo ) );
                }
            }
            else
            {
                LogMsg( LOG_VERBOSE, "Could Not Resolve file length of file %s", fileName.c_str() );
            }
        }
        else
        {
            LogMsg( LOG_VERBOSE, "NOT Readable File %s", fileName.c_str() );
        }
    }
}

//============================================================================
bool GuiHelpers::browseForFile( QWidget* parent, std::string& retFileName, QString startDir, uint8_t fileMask )
{
    contentUrlToFileSystemPath( startDir );

    listFilesInFolder( startDir.toUtf8().constData() );

    QFileDialog dialog( parent, QObject::tr("Select File"), startDir );

    dialog.setFileMode( QFileDialog::FileMode::ExistingFile );

    dialog.setOptions( dialog.options() | QFileDialog::ReadOnly );

    //dialog.setDirectory( startDir );

    QString selectedFile;
    QStringList fileNames;
    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
        if( fileNames.size() )
        {
            selectedFile = fileNames[0];
        }
    }

    if( selectedFile.isEmpty() )
    {
        return false;
    }

    std::string realFileName = getRealFileName( selectedFile );


    //std::string fullFileName = selectedFile.toUtf8().constData();

    std::string fullFileName = realFileName;

    if( !requestAndroidStoragePermissions())
    {
        QMessageBox warnStorage( QMessageBox::Icon::Information, QObject::tr("Cannot Access File "), fullFileName.c_str(), QMessageBox::Ok);
        warnStorage.exec();
        return false;
    }

    std::string decodedFileName = VxFileUtil::decodePercentEncodingAll( fullFileName );

    VxFileUtil::makeForwardSlashPath( fullFileName );

    if( !testCanReadFile( fullFileName ) )
    {
        QMessageBox warnStorage( QMessageBox::Icon::Information, QObject::tr("Cannot Read File 2"), decodedFileName.c_str(), QMessageBox::Ok);
        warnStorage.exec();
    }

    /*
    QFileInfo fileInfo(selectedFile);
    QString absolutePath = fileInfo.absoluteFilePath();
    QString conicalPath = fileInfo.canonicalFilePath();

    std::filesystem::path absPath = fileInfo.filesystemAbsoluteFilePath();
    std::filesystem::path conPath = fileInfo.filesystemCanonicalFilePath();

    contentUrlToFileSystemPath( selectedFile );

    LogMsg( LOG_VERBOSE, "sel %s \n absolute %s conical %s \n abs %s \n con %s",
            selectedFile.toUtf8().constData(),
           absolutePath.toUtf8().constData(),
           conicalPath.toUtf8().constData(),
           absPath.c_str(), conPath.c_str() );

    retFileName = selectedFile.toUtf8().constData();



    VxFileUtil::decodePercentEncodingOfSlash( retFileName );
    VxFileUtil::makeForwardSlashPath( retFileName );
*/

    retFileName = fullFileName;
    return !retFileName.empty();
}

//============================================================================
uint64_t GuiHelpers::testCanReadFile( std::string fullFileName )
{
    uint64_t fileLen = VxFileUtil::fileExists(fullFileName.c_str());
    if( fileLen )
    {
        VFile* vFile = VxFileUtil::fileOpen( fullFileName.c_str(), "rb" );
        if( vFile )
        {
            VFileClose( vFile );
            return fileLen;
        }
    }

    return 0;
}

//============================================================================
bool GuiHelpers::browseForFile( QWidget* parent, FileInfo& retFileInfo, QString startDir, uint8_t fileMask )
{
    std::string fileName;
    bool result = browseForFile( parent, fileName, startDir, fileMask );
    if( !result )
    {
        return false;
    }

    uint64_t fileLen = VxFileUtil::getFileLen( fileName.c_str() );
    if( !fileLen )
    {
        return false;
    }

    uint8_t fileType = VxFileUtil::fileExtensionToFileTypeFlag( fileName.c_str() );

    retFileInfo = FileInfo( GetAppInstance().getMyOnlineId(), fileName, fileLen, fileType );
    return true;
}

//============================================================================
QString GuiHelpers::fileMaskToFileFilter( uint8_t fileMask )
{
    QString filter = "All files (*.*)";
    std::string fileExtensions = VxGetFileExtensionsFromFileType( fileMask );
	if( fileMask == VXFILE_TYPE_AUDIO_VIDEO || fileMask == VXFILE_TYPE_AUDIO_VIDEO_PHOTO )
	{
        filter += ";;Media (";
        filter += fileExtensionToFilter( fileExtensions.c_str() );
        filter += ")";
	}
	else if( fileMask == VXFILE_TYPE_ALLNOTEXE )
	{
        filter += ";;Known (";
        filter += fileExtensionToFilter( fileExtensions.c_str() );
        filter += ")";
	}
	else if( fileMask == VXFILE_TYPE_ANY || !fileMask )
	{
		return filter;
	}
	else
	{
		if( fileMask == VXFILE_TYPE_PHOTO )
		{
            filter += ";;Photo (";
            filter += fileExtensionToFilter( fileExtensions.c_str() );
            filter += ")";
		}
		else if( fileMask == VXFILE_TYPE_AUDIO )
		{
            filter += ";;Audio (";
            filter += fileExtensionToFilter( fileExtensions.c_str() );
            filter += ")";
		}
		else if( fileMask == VXFILE_TYPE_VIDEO )
		{
	        filter += ";;Video (";
            filter += fileExtensionToFilter( fileExtensions.c_str() );
            filter += ")";
		}
		else if( fileMask == VXFILE_TYPE_DOC )
		{
	        filter += ";;Document (";
            filter += fileExtensionToFilter( fileExtensions.c_str() );
            filter += ")";
		}
		else if( fileMask == VXFILE_TYPE_ARCHIVE_OR_CDIMAGE )
		{
	        filter += ";;Archive (";
            filter += fileExtensionToFilter( fileExtensions.c_str() );
            filter += ")";
		}
		else if( fileMask == VXFILE_TYPE_EXECUTABLE )
		{
	        filter += ";;Executable (";
            filter += fileExtensionToFilter( fileExtensions.c_str() );
            filter += ")";
		}
	}

    return filter;
}

//============================================================================
QString GuiHelpers::fileExtensionToFilter( QString fileExtensions )
{
    QString filter;
    QStringList extList = fileExtensions.split( ',' );
    for( auto ext : extList )
    {
        if( !filter.isEmpty() )
        {
            filter += " ";
        }

        filter += "*.";
        filter += ext;
    }

    return filter;
}

//============================================================================
bool GuiHelpers::copyResourceToOnDiskFile( QString resourcePath, QString fileNameAndPath )
{
	bool resourceCopied = false;

	QFile resfile( resourcePath );
	QFile onDiskFile( fileNameAndPath );
	if( resfile.open( QIODevice::ReadOnly ) )
	{
		if( onDiskFile.open( QIODevice::ReadWrite ) )
		{
			if( 0 == onDiskFile.write( resfile.readAll() ) )
			{
				qWarning() << "Could not write app resource file " << fileNameAndPath;
			}
			else
			{
				resourceCopied = true;
			}

			onDiskFile.close();
		}
		else
		{
			qWarning() << "Could not open app resource file for writing " << fileNameAndPath;
		}

		resfile.close();
	}
	else
	{
		qWarning() << "Could not open resource file " << resourcePath;
	}

	return resourceCopied;
}

//============================================================================
EApplet GuiHelpers::getAppletThatPlaysFile( AppCommon& myApp, AssetBaseInfo& assetInfo )
{
    return getAppletThatPlaysFile( myApp, 
                                   GuiParams::assetTypeToFileType( assetInfo.getAssetType() ), 
                                   assetInfo.getAssetName().c_str(), 
                                   assetInfo.getAssetUniqueId(),
                                   assetInfo.getIsStream() );
}

//============================================================================
EApplet GuiHelpers::getAppletThatPlaysFile( AppCommon& myApp, uint8_t fileType, QString fullFileName, VxGUID& assetId, bool isStream )
{
    EApplet applet = eAppletUnknown;
    if( fileType & VXFILE_TYPE_VIDEO )
    {
        if( isStream )
        {
            applet = eAppletPlayerStream;
        }
        else if( myApp.getFromGuiInterface().fromGuiIsNoLimitVideoFile( fullFileName.toUtf8().constData() ) )
        {
            applet = eAppletPlayerCamClip;
        }
        else
        {
            applet = eAppletPlayerNlc;
        }
    }
    else if( fileType & VXFILE_TYPE_PHOTO )
    {
        applet = eAppletPlayerPhoto;
    }
    else if( fileType & VXFILE_TYPE_AUDIO )
    {
        if( isStream )
        {
            applet = eAppletPlayerStream;
        }
        else if( myApp.getFromGuiInterface().fromGuiIsNoLimitAudioFile( fullFileName.toUtf8().constData() ) )
        {
            applet = eAppletPlayerNlc; // should this be a specialized player ? for less cpu consuption ?
        }
        else
        {
            applet = eAppletPlayerNlc;
        }
    }

    return applet;
}

//============================================================================
int GuiHelpers::calculateTextHeight( QFontMetrics& fontMetrics, QString textStr )
{
	int textHeight = 0;
	if( textStr.isEmpty() )
	{
		textHeight = fontMetrics.height();
	}
	else
	{
		int lineCount = textStr.count( QLatin1Char('\n') ) + 1;
		textHeight = lineCount * fontMetrics.height();
	}

	return textHeight;
}

//============================================================================
bool GuiHelpers::isAppletAService( EApplet applet )
{
    return ( ( eAppletServiceAboutMe == applet )
             || ( eAppletServiceAvatarImage == applet )
             || ( eAppletServiceShareFiles == applet )
             || ( eAppletServiceShareWebCam == applet )
             || ( eAppletServiceAboutMe == applet )
             || ( eAppletServiceStoryboard == applet )
             );
}

//============================================================================
bool GuiHelpers::isAppletAClient( EApplet applet )
{
    return ( ( eAppletAboutMeClient == applet )
             || ( eAppletAvatarImageClient == applet )
             || ( eAppletConnectionTestClient == applet )
             || ( eAppletGroupClient == applet )
             || ( eAppletHostGroupListingClient == applet )
             || ( eAppletRandomConnectClient == applet )
             || ( eAppletClientShareFiles == applet )
             || ( eAppletCamClient == applet )
             || ( eAppletStoryboardClient == applet )
             );
}

//============================================================================
EPluginType GuiHelpers::getAppletAssociatedPlugin( EApplet applet )
{
    EPluginType pluginType = ePluginTypeInvalid;
    switch( applet )
    {
    case eAppletAboutMeClient:              return ePluginTypeAboutMePageClient;
    case eAppletCamClient:                  return ePluginTypeCamServer;
    case eAppletAvatarImageClient:          return ePluginTypeClientPeerUser;
    case eAppletConnectionTestClient:       return ePluginTypeClientConnectTest;
    case eAppletGroupClient:            return ePluginTypeHostGroup;
    case eAppletHostGroupListingClient:     return ePluginTypeNetworkSearchList;
    case eAppletHostNetworkClient:          return ePluginTypeHostNetwork;
    case eAppletRandomConnectClient:        return ePluginTypeClientRandomConnect;
    case eAppletClientShareFiles:           return ePluginTypeFileShareServer;

    case eAppletFileShareClientView:        return ePluginTypeFileShareClient;

    case eAppletServiceAboutMe:              return ePluginTypeAboutMePageServer;
    case eAppletServiceAvatarImage:          return ePluginTypeHostPeerUser;
    case eAppletServiceConnectionTest:       return ePluginTypeHostConnectTest;
    case eAppletChatRoomHostAdmin:           return ePluginTypeHostChatRoom;
    case eAppletGroupHostAdmin:              return ePluginTypeHostGroup;
    case eAppletServiceHostNetwork:          return ePluginTypeHostNetwork;
    case eAppletServiceShareFiles:           return ePluginTypeFileShareServer;
    case eAppletServiceShareWebCam:          return ePluginTypeCamServer;
    case eAppletServiceStoryboard:           return ePluginTypeStoryboardServer;

    case eAppletSettingsAboutMe:            return ePluginTypeAboutMePageServer;
    case eAppletSettingsAvatarImage:        return ePluginTypeHostPeerUser;
    case eAppletSettingsWebCamServer:       return ePluginTypeCamServer;
    case eAppletSettingsConnectTest:        return ePluginTypeHostConnectTest;
    case eAppletSettingsShareFiles:         return ePluginTypeFileShareServer;
    case eAppletSettingsFileXfer:           return ePluginTypePersonFileXfer;

    case eAppletSettingsHostChatRoom:       return ePluginTypeHostChatRoom;
    case eAppletSettingsHostGroup:          return ePluginTypeHostGroup;
    case eAppletSettingsHostNetwork:        return ePluginTypeHostNetwork;
    case eAppletSettingsHostRandomConnect:  return ePluginTypeHostRandomConnect;

    case eAppletSettingsMessenger:          return ePluginTypeMessenger;
    case eAppletSettingsRandomConnect:      return ePluginTypeClientRandomConnect;

    case eAppletSettingsStoryboard:         return ePluginTypeStoryboardServer;
    case eAppletSettingsTruthOrDare:        return ePluginTypeTruthOrDare;
    case eAppletSettingsVideoPhone:         return ePluginTypeVideoPhone;
    case eAppletSettingsVoicePhone:         return ePluginTypeVoicePhone;


    default:
        break;
    }

    return pluginType;
}

//============================================================================
EApplet GuiHelpers::pluginTypeToEditApplet( EPluginType pluginType )
{
    EApplet appletType = eAppletUnknown;

    switch( pluginType )
    {
    case ePluginTypeAboutMePageServer:      return eAppletEditAboutMe;
    case ePluginTypeHostPeerUser:           return eAppletEditAvatarImage;
    case ePluginTypeStoryboardServer:       return eAppletEditStoryboard;
    case ePluginTypeCamServer:              return eAppletUnknown;
    case ePluginTypeHostConnectTest:        return eAppletSettingsConnectTest;
    case ePluginTypeFileShareServer:        return eAppletUnknown;
    case ePluginTypePersonFileXfer:         return eAppletUnknown;
    case ePluginTypeHostGroup:              return eAppletUnknown;
    case ePluginTypeNetworkSearchList:      return eAppletUnknown;
    case ePluginTypeHostNetwork:            return eAppletUnknown;
    case ePluginTypeClientRandomConnect:    return eAppletUnknown;
    case ePluginTypeHostRandomConnect:      return eAppletUnknown;
    default:
        break;
    }

    return appletType;
}

//============================================================================
EApplet GuiHelpers::pluginTypeToSettingsApplet( EPluginType pluginType )
{
    EApplet appletType = eAppletUnknown;

    switch( pluginType )
    {
    case ePluginTypeAboutMePageServer:      return eAppletSettingsAboutMe;
    case ePluginTypeHostPeerUser:           return eAppletSettingsAvatarImage;
    case ePluginTypeCamServer:              return eAppletSettingsWebCamServer;
    case ePluginTypeHostConnectTest:        return eAppletSettingsConnectTest;
    case ePluginTypeFileShareServer:        return eAppletSettingsShareFiles;
    case ePluginTypePersonFileXfer:         return eAppletSettingsFileXfer;

    case ePluginTypeHostChatRoom:           return eAppletSettingsHostChatRoom;
    case ePluginTypeHostGroup:              return eAppletSettingsHostGroup;
    case ePluginTypeHostNetwork:            return eAppletSettingsHostNetwork;
    case ePluginTypeHostRandomConnect:      return eAppletSettingsHostRandomConnect;

    case ePluginTypeMessenger:              return eAppletSettingsMessenger;
    case ePluginTypePushToTalk:             return eAppletSettingsPushToTalk;
    case ePluginTypeClientRandomConnect:    return eAppletSettingsRandomConnect;

    case ePluginTypeStoryboardServer:       return eAppletSettingsStoryboard;
    case ePluginTypeTruthOrDare:            return eAppletSettingsTruthOrDare;
    case ePluginTypeVideoPhone:             return eAppletSettingsVideoPhone;
    case ePluginTypeVoicePhone:             return eAppletSettingsVoicePhone;

    default:
        break;
    }

    return appletType;
}

//============================================================================
EApplet GuiHelpers::pluginTypeToViewApplet( EPluginType pluginType )
{
    EApplet appletType = eAppletUnknown;

    switch( pluginType )
    {
    case ePluginTypeAboutMePageServer:      return eAppletEditAboutMe;
    case ePluginTypeHostPeerUser:           return eAppletEditAvatarImage;
    case ePluginTypeStoryboardServer:       return eAppletEditStoryboard;
    case ePluginTypeCamServer:              return eAppletUnknown;
    case ePluginTypeHostConnectTest:        return eAppletSettingsConnectTest;
    case ePluginTypeFileShareServer:        return eAppletUnknown;
    case ePluginTypePersonFileXfer:         return eAppletUnknown;
    case ePluginTypeHostGroup:              return eAppletUnknown;
    case ePluginTypeNetworkSearchList:      return eAppletUnknown;
    case ePluginTypeHostNetwork:            return eAppletUnknown;
    case ePluginTypeClientRandomConnect:    return eAppletUnknown;
    case ePluginTypeHostRandomConnect:      return eAppletUnknown;

    default:
        break;
    }

    return appletType;
}

//============================================================================
EApplet GuiHelpers::pluginTypeToUserApplet( EPluginType pluginType )
{
    EApplet appletType = eAppletUnknown;

    switch( pluginType )
    {
    case ePluginTypeAboutMePageServer:     return eAppletEditAboutMe;
    case ePluginTypeAboutMePageClient:     return eAppletAboutMeClient;
    case ePluginTypeHostPeerUser:          return eAppletEditAvatarImage;
    //case ePluginTypeCamServer:              return eAppletSettingsWebCamServer;
    // case ePluginTypeHostConnectTest:     return eAppletSettingsConnectTest;
    //case ePluginTypeFileShareServer:             return eAppletShareFiles;
    // case ePluginTypePersonFileXfer:               return eAppletSettingsFileXfer;

    case ePluginTypeHostChatRoom:           return eAppletSettingsHostChatRoom;
    case ePluginTypeHostGroup:              return eAppletSettingsHostGroup;
    case ePluginTypeHostNetwork:            return eAppletSettingsHostNetwork;
    case ePluginTypeHostRandomConnect:      return eAppletSettingsHostRandomConnect;

    case ePluginTypeStoryboardServer:       return eAppletEditStoryboard;
    case ePluginTypeStoryboardClient:       return eAppletStoryboardClient;

    //case ePluginTypeMessenger:              return eAppletMultiMessenger;
    //case ePluginTypeTruthOrDare:            return eAppletPeerTruthOrDare;
    //case ePluginTypeVideoPhone:             return eAppletPeerVideoPhone;
    //case ePluginTypeVoicePhone:             return eAppletPeerVoicePhone;

    default:
        break;
    }

    return appletType;
}

//============================================================================
EApplet GuiHelpers::pluginTypeToSessionApplet( EPluginType pluginType )
{
    EApplet appletType = eAppletUnknown;

    switch( pluginType )
    {
    case ePluginTypeMessenger:              return eAppletMultiMessenger;
    case ePluginTypeTruthOrDare:            return eAppletPeerTruthOrDare;
    case ePluginTypeVideoPhone:             return eAppletPeerVideoPhone;
    case ePluginTypeVoicePhone:             return eAppletPeerVoicePhone;

    default:
        break;
    }

    return appletType;
}


//============================================================================
QFrame* GuiHelpers::pluginTypeToDefaultContentFrame( EPluginType pluginType )
{
    EApplet appletType = pluginTypeToSessionApplet( pluginType );
    if( eAppletUnknown != appletType )
    {
        QString contentFrameObjName = OBJNAME_FRAME_LAUNCH_PAGE;
        if( ePluginTypeMessenger == pluginType )
        {
            contentFrameObjName = OBJNAME_FRAME_MESSAGER_PAGE;
        }

        return findContentFrame( contentFrameObjName );
    }

    return nullptr;
}

//============================================================================
QFrame* GuiHelpers::pluginTypeToOppositeDefaultContentFrame( EPluginType pluginType )
{
    EApplet appletType = pluginTypeToSessionApplet( pluginType );
    if( eAppletUnknown != appletType )
    {
        QString contentFrameObjName = OBJNAME_FRAME_MESSAGER_PAGE;
        if( ePluginTypeMessenger == pluginType )
        {
            contentFrameObjName = OBJNAME_FRAME_LAUNCH_PAGE;
        }

        return findContentFrame( contentFrameObjName );
    }

    return nullptr;
}

//============================================================================
EMyIcons GuiHelpers::pluginTypeToSettingsIcon( EPluginType pluginType )
{
    EMyIcons iconType = eMyIconUnknown;

    switch( pluginType )
    {
    case ePluginTypeAboutMePageServer:      return eMyIconSettingsAboutMe;
    case ePluginTypeHostPeerUser:           return eMyIconSettingsAvatarImage;
    case ePluginTypeCamServer:              return eMyIconSettingsShareWebCam;
    case ePluginTypeHostConnectTest:        return eMyIconSettingsConnectionTest;
    case ePluginTypeFileShareServer:        return eMyIconSettingsShareFiles;
    case ePluginTypePersonFileXfer:         return eMyIconSettingsFileXfer;
    case ePluginTypeHostGroup:              return eMyIconSettingsHostGroup;
    case ePluginTypeNetworkSearchList:      return eMyIconSettingsHostGroupListing;
    case ePluginTypeHostNetwork:            return eMyIconSettingsHostNetwork;
    case ePluginTypeMessenger:              return eMyIconSettingsMessenger;
    case ePluginTypeClientRandomConnect:    return eMyIconSettingsRandomConnect;
    case ePluginTypeHostRandomConnect:      return eMyIconSettingsRandomConnectRelay;
    case ePluginTypeStoryboardServer:       return eMyIconSettingsShareStoryboard;
    case ePluginTypeTruthOrDare:            return eMyIconSettingsTruthOrDare;
    case ePluginTypeVideoPhone:             return eMyIconSettingsVideoPhone;
    case ePluginTypeVoicePhone:             return eMyIconSettingsVoicePhone;
    case ePluginTypePushToTalk:             return eMyIconSettingsPushToTalk;

    default:
        break;
    }

    return iconType;
}

//============================================================================
bool GuiHelpers::isPluginSingleSession( EPluginType pluginType )
{
    return ::IsPluginSingleSession( pluginType );
}

//============================================================================
//! which plugins to show in permission list
bool GuiHelpers::isPluginAPrimaryService( EPluginType pluginType )
{
    bool isPrimaryPlugin = false;
    switch( pluginType )
    {
    case ePluginTypeAdmin:
    case ePluginTypeAboutMePageServer:
    case ePluginTypeVoicePhone:
    case ePluginTypeVideoPhone:
    case ePluginTypeTruthOrDare:
    case ePluginTypeMessenger:
    case ePluginTypeCamServer:
    case ePluginTypeStoryboardServer:
    case ePluginTypeFileShareServer:
    case ePluginTypePersonFileXfer:
    case ePluginTypeHostGroup:
    case ePluginTypeHostChatRoom:
    case ePluginTypeHostNetwork:
    case ePluginTypeHostRandomConnect:
    case ePluginTypeHostConnectTest:
    case ePluginTypePushToTalk:
        isPrimaryPlugin = true;
        break;

    case ePluginTypeNetworkSearchList:
    case ePluginTypeClientConnectTest:
    default:
        break;
    }

    return isPrimaryPlugin;
}

//============================================================================
bool GuiHelpers::getSecondaryPlugins( EPluginType pluginType, QVector<EPluginType> secondaryPlugins )
{
    secondaryPlugins.clear();
    switch( pluginType )
    {
    case ePluginTypeHostNetwork:
        secondaryPlugins.push_back( ePluginTypeNetworkSearchList );
        secondaryPlugins.push_back( ePluginTypeHostConnectTest );
        break;

    case ePluginTypeHostChatRoom:
        secondaryPlugins.push_back( ePluginTypeHostConnectTest );
        break;

    case ePluginTypeHostGroup:
        secondaryPlugins.push_back( ePluginTypeHostConnectTest );
        break;

    case ePluginTypeHostRandomConnect:
        secondaryPlugins.push_back( ePluginTypeHostConnectTest );
        break;

    default:
        break;
    }

    return !secondaryPlugins.isEmpty();
}

//============================================================================
bool GuiHelpers::isMessagerFrame( QWidget* curWidget )
{
    bool isMessengerFrame{ false };
    
    QObject * curParent = curWidget;
    QString messengerPageObjName = OBJNAME_FRAME_MESSAGER_PAGE;

    while( curParent )
    {
        QString objName = curParent->objectName();
        if( objName == messengerPageObjName )
        {
            isMessengerFrame = true;
            break;
        }


        curParent = dynamic_cast<QObject *>( curParent->parent() );
        if( !curParent )
        {
            break;
        }
    }

    return isMessengerFrame;

}

//============================================================================
QFrame* GuiHelpers::getParentPageFrame( QWidget* curWidget )
{
    QFrame* pageFrame = nullptr;
    QObject * curParent = curWidget;

    QString launchPageObjName = OBJNAME_FRAME_LAUNCH_PAGE;
    QString messengerPageObjName = OBJNAME_FRAME_MESSAGER_PAGE;

    while( curParent )
    {
        QString objName = curParent->objectName();
        if( ( objName == launchPageObjName ) || ( objName == messengerPageObjName ) )
        {
            pageFrame = dynamic_cast<QFrame*>( curParent );
            if( pageFrame )
            {
                break;
            }
        }

        if( !curParent->parent() )
        {
            LogMsg( LOG_WARNING, "Object %s has no parent", objName.toUtf8().constData() );
        }

        curParent = dynamic_cast<QObject *>( curParent->parent() );
    }

    return pageFrame;
}

//============================================================================
QString GuiHelpers::getParentPageFrameName( QWidget* curWidget )
{
    QString frameName;

    QFrame* parentFrame = getParentPageFrame( curWidget );
    if( parentFrame )
    {
        frameName = parentFrame->objectName();
        if( frameName != OBJNAME_FRAME_LAUNCH_PAGE && frameName != OBJNAME_FRAME_MESSAGER_PAGE )
        {
            LogMsg( LOG_ERROR, "GuiHelpers::getParentPageFrameName invalid frame name" );
            frameName.clear();
        }
    }

    return frameName;
}

//============================================================================
QFrame* GuiHelpers::getMessengerPageFrame( QWidget* curWidget )
{
    QFrame* pageFrame = nullptr;
    QObject * curParent = curWidget;

    QString launchPageObjName = OBJNAME_FRAME_LAUNCH_PAGE;
    QString messengerPageObjName = OBJNAME_FRAME_MESSAGER_PAGE;

    while( curParent )
    {
        QString objName = curParent->objectName();
        if( (objName == launchPageObjName) || (objName == messengerPageObjName) )
        {
            if( objName == messengerPageObjName )
            {
                pageFrame = dynamic_cast<QFrame*>(curParent);
                if (pageFrame)
                {
                    break;
                }
            }
            else
            {
                bool foundMessengerFrame = false;
                QWidget* baseFrame = dynamic_cast<QWidget*>(curParent->parent());
                if( baseFrame )
                {
                    QObjectList childList = baseFrame->children();
                    for( auto iter = childList.begin();  iter != childList.end(); ++iter )
                    {
                        QFrame*childFrame = dynamic_cast<QFrame*>(*iter);
                        if( childFrame && childFrame->objectName() == messengerPageObjName )
                        {
                            pageFrame = childFrame;
                            foundMessengerFrame = true;
                            break;
                        }
                    }
                }

                if( foundMessengerFrame )
                {
                    break;
                }
            }
        }

        if( !curParent->parent() )
        {
            LogMsg( LOG_WARNING, "Object %s has no parent", objName.toUtf8().constData() );
        }

        curParent = dynamic_cast<QObject *>(curParent->parent());
    }

    return pageFrame;
}

//============================================================================
QFrame* GuiHelpers::getLaunchPageFrame( QWidget* curWidget )
{
    QFrame* pageFrame = nullptr;
    QObject * curParent = curWidget;

    QString launchPageObjName = OBJNAME_FRAME_LAUNCH_PAGE;
    QString messengerPageObjName = OBJNAME_FRAME_MESSAGER_PAGE;

    while (curParent)
    {
        QString objName = curParent->objectName();
        if ((objName == launchPageObjName) || (objName == messengerPageObjName))
        {
            if( objName == launchPageObjName )
            {
                pageFrame = dynamic_cast<QFrame*>(curParent);
                if (pageFrame)
                {
                    break;
                }
            }
            else
            {
                bool foundLaunchFrame = false;
                QWidget* baseFrame = dynamic_cast<QWidget*>(curParent->parent());
                if( baseFrame )
                {
                    QObjectList childList = baseFrame->children();
                    for( auto iter = childList.begin();  iter != childList.end(); ++iter )
                    {
                        QFrame* childFrame = dynamic_cast<QFrame*>(*iter);
                        if( childFrame && childFrame->objectName() == launchPageObjName )
                        {
                            pageFrame = childFrame;
                            foundLaunchFrame = true;
                            break;
                        }
                    }
                }

                if( foundLaunchFrame )
                {
                    break;
                }
            }
        }

        if (!curParent->parent())
        {
            LogMsg( LOG_WARNING, "Object %s has no parent", objName.toUtf8().constData() );
        }

        curParent = dynamic_cast<QObject *>(curParent->parent());
    }

    return pageFrame;
}

//============================================================================
QFrame* GuiHelpers::getOppositePageFrame( QWidget* curWidget )
{
    QFrame* pageFrame = nullptr;
    QObject * curParent = curWidget;

    QString launchPageObjName = OBJNAME_FRAME_LAUNCH_PAGE;
    QString messengerPageObjName = OBJNAME_FRAME_MESSAGER_PAGE;

    while (curParent)
    {
        QString objName = curParent->objectName();
        if ((objName == launchPageObjName) || (objName == messengerPageObjName))
        {
            QString otherPageObjeName = (objName == launchPageObjName) ? messengerPageObjName : launchPageObjName;

            bool foundOtherFrame = false;
            QWidget* baseFrame = dynamic_cast<QWidget*>(curParent->parent());
            if( baseFrame )
            {
                QObjectList childList = baseFrame->children();
                for( auto iter = childList.begin();  iter != childList.end(); ++iter )
                {
                    QFrame* childFrame = dynamic_cast<QFrame*>(*iter);
                    if( childFrame && childFrame->objectName() == otherPageObjeName && childFrame == GetAppInstance().getHomePage().getMessengerParentFrame() )
                    {
                        pageFrame = childFrame;
                        foundOtherFrame = true;
                        break;
                    }
                }
            }

            if( foundOtherFrame )
            {
                break;
            }
        }

        if (!curParent->parent())
        {
            LogMsg( LOG_WARNING, "Object %s has no parent", objName.toUtf8().constData() );
        }

        curParent = dynamic_cast<QObject *>(curParent->parent());
    }

    return pageFrame;
}

//============================================================================
QFrame* GuiHelpers::findContentFrame( QString& contentFrameObjName )
{
    if( OBJNAME_FRAME_MESSAGER_PAGE == contentFrameObjName )
    {
        return GetAppInstance().getHomePage().getMessengerParentFrame();
    }
    else
    {
        return GetAppInstance().getHomePage().getLaunchPageFrame();
    }
}

//============================================================================
AppletBase * GuiHelpers::findParentApplet( QWidget* parent )
{
    AppletBase * appletBase = nullptr;
    if( parent )
    {
        appletBase = dynamic_cast< AppletBase * >( parent );
        while( parent && !appletBase )
        {
            parent = dynamic_cast< QWidget* >( parent->parent() );
            if( parent )
            {
                appletBase = dynamic_cast< AppletBase * >( parent );
            }
        }
    }

    return appletBase;
}

//============================================================================
bool GuiHelpers::validateUserName( QWidget* curWidget, QString strUserName )
{
    if( strUserName.contains( "NoLimitConnect" )
        || strUserName.contains( "nolimitconnect" )
        || strUserName.contains( "No Limit Connect" )
        || strUserName.contains( "no limit connect" )
        || strUserName.contains( "NoLimitConnectWeb" )
        || strUserName.contains( "NlcPtoP" )
        || strUserName.contains( "Nlc PtoP Web" )
        || strUserName.contains( "gotv ptop web" ) )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Invalid User Name" ), QObject::tr( "User Name cannot have NoLimitConnect in name." ) );
        return false;
    }

    if( strUserName.contains( "'" ) || strUserName.contains( "\"" ) )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Invalid User Name" ), QObject::tr( "User Name cannot have special character quote." ) );
        return false;
    }

    if( strUserName.contains( "," ) )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Invalid User Name" ), QObject::tr( "User Name cannot have comma." ) );
        return false;
    }

    if( strUserName.contains( "(" ) || strUserName.contains( ")" ) )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Invalid User Name" ), QObject::tr( "User Name cannot have special character parentheses." ) );
        return false;
    }

    if( strUserName.contains( "/" ) || strUserName.contains( "\\" ) )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Invalid User Name" ), QObject::tr( "User Name cannot have special character slashes." ) );
        return false;
    }

    if( strUserName.length() > 27 )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Invalid User Name" ), QObject::tr( "User Name is too long (maximum 27 chars)." ) );
        return false;
    }

    if( strUserName.length() < 4 )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Invalid User Name" ), QObject::tr( "User Name is too short (minimum 4 chars)." ) );
        return false;
    }

    return true;
}

//============================================================================
bool GuiHelpers::validateMoodMessage( QWidget* curWidget, QString strMoodMsg )
{
    //if( strMoodMsg.contains( "'" ) )
    //{
    //    QMessageBox::warning( curWidget, QObject::tr( "Application" ), QObject::tr( "Mood Message cannot have special character quote." ) );
    //    return false;
    //}

    int iLen = strMoodMsg.length();
    if( iLen > 27 )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Application" ), QObject::tr( "Mood Message is too long (maximum 27 chars)" ) );
        return false;
    }

    return true;
}

//============================================================================
bool GuiHelpers::validateAge( QWidget* curWidget, int age )
{
    if( age < 0 )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Age Verify" ), QObject::tr( "Invalid Age" ) );
        return false;
    }

    if( age > 120 )
    {
        QMessageBox::warning( curWidget, QObject::tr( "Age Verify" ), QObject::tr( "Age Connot be greater than 120 years old" ) );
        return false;
    }

    return true;
}

//============================================================================
void GuiHelpers::fillGender( QComboBox * comboBox )
{
    if( comboBox )
    {
        comboBox->clear();
        for( int i = 0; i < eMaxGenderType; i++ )
        {
            comboBox->addItem( GuiParams::describeGender( (EGenderType)i ) );
        }
    }
}

//============================================================================
EGenderType GuiHelpers::getGender( QComboBox * comboBox )
{
    return (EGenderType)comboBox->currentIndex();
}

//============================================================================
bool GuiHelpers::setGender( QComboBox * comboBox, EGenderType gender)
{
    if( comboBox )
    {
        comboBox->setCurrentIndex(genderToIndex(gender));
        return true;
    }
    else
    {
        return false;
    }
}

//============================================================================
uint8_t GuiHelpers::genderToIndex( EGenderType gender )
{
    if( ( gender >= 0 ) && ( gender < eMaxGenderType ) )
    {
        return ( uint8_t )gender;
    }

    return 0;
}

//============================================================================
void GuiHelpers::fillAge( QComboBox * comboBox )
{
    if( comboBox )
    {
        comboBox->clear();
        for( int i = 0; i < eMaxAgeType; i++ )
        {
            comboBox->addItem(  GuiParams::describeAge( (EAgeType)i ) );
        }
    }
}

//============================================================================
EAgeType GuiHelpers::getAge( QComboBox * comboBox )
{
    if( comboBox )
    {
        return (EAgeType)comboBox->currentIndex();
    }
    else
    {
        return eAgeTypeUnspecified;
    }
}

//============================================================================
bool GuiHelpers::setAge( QComboBox * comboBox, EAgeType ageType)
{
    if( comboBox )
    {
        comboBox->setCurrentIndex(ageToIndex(ageType));
        return true;
    }
    else
    {
        return false;
    }
}

//============================================================================
uint8_t GuiHelpers::ageToIndex( EAgeType age )
{
    if( ( age >= 0 ) && ( age < eMaxAgeType ) )
    {
        return ( uint8_t )age;
    }

    return 0;
}

//============================================================================
void GuiHelpers::fillContentRating( QComboBox * comboBox )
{
    if( comboBox )
    {
        comboBox->clear();
        for( int i = 0; i < eMaxContentRating; i++ )
        {
            comboBox->addItem(  GuiParams::describeContentRating( (EContentRating)i ) );
        }
    }
}

//============================================================================
EContentRating GuiHelpers::getContentRating( QComboBox * comboBox )
{
    if( comboBox )
    {
        return (EContentRating)comboBox->currentIndex();
    }
    else
    {
        return eContentRatingUnspecified;
    }
}

//============================================================================
bool GuiHelpers::setContentRating( QComboBox * comboBox, EContentRating contentRating)
{
    if( comboBox )
    {
        comboBox->setCurrentIndex(contentRatingToIndex(contentRating));
        return true;
    }
    else
    {
        return false;
    }
}

//============================================================================
uint8_t GuiHelpers::contentRatingToIndex( EContentRating rating )
{
    if( ( rating >= 0 ) && ( rating < eMaxContentRating ) )
    {
        return ( uint8_t )rating;
    }

    return 0;
}

//============================================================================
void GuiHelpers::fillContentCatagory( QComboBox * comboBox )
{
    if( comboBox )
    {
        comboBox->clear();
        for( int i = 0; i < eMaxContentCatagory; i++ )
        {
            comboBox->addItem(  GuiParams::describeContentCatagory( ( EContentCatagory )i ) );
        }
    }
}

//============================================================================
uint8_t GuiHelpers::contentCatagoryToIndex( EContentCatagory rating )
{
    if( ( rating >= 0 ) && ( rating < eMaxContentCatagory ) )
    {
        return ( uint8_t )rating;
    }

    return 0;
}

//============================================================================
void GuiHelpers::fillLanguage( QComboBox * comboBox )
{
    if( comboBox )
    {
        comboBox->clear();
        for( int i = 0; i < eMaxLanguageType; i++ )
        {
            comboBox->addItem(  GuiParams::describeLanguage( (ELanguageType)i ) );
        }
    }
}

//============================================================================
ELanguageType GuiHelpers::getLanguage( QComboBox * comboBox )
{
    if( comboBox )
    {
        return (ELanguageType)comboBox->currentIndex();
    }
    else
    {
        return eLangUnspecified;
    }
}

//============================================================================
bool GuiHelpers::setLanguage( QComboBox * comboBox, ELanguageType language)
{
    if( comboBox )
    {
        comboBox->setCurrentIndex(languageToIndex(language));
        return true;
    }
    else
    {
        return false;
    }
}

//============================================================================
uint16_t GuiHelpers::languageToIndex( ELanguageType language )
{
    if( ( language >= 0 ) && ( language < eMaxLanguageType ) )
    {
        return ( uint16_t )language;
    }

    return 0;
}

//============================================================================
void GuiHelpers::fillPermissionComboBox( QComboBox * permissionComboBox )
{
    if( permissionComboBox )
    {
        permissionComboBox->clear();
        permissionComboBox->addItem( GuiParams::describePermissionLevel( eFriendStateAdmin ) );
        permissionComboBox->addItem( GuiParams::describePermissionLevel( eFriendStateFriend ) );
        permissionComboBox->addItem( GuiParams::describePermissionLevel( eFriendStateGuest ) );
        permissionComboBox->addItem( GuiParams::describePermissionLevel( eFriendStateAnonymous ) );
        permissionComboBox->addItem( GuiParams::describePermissionLevel( eFriendStateIgnore ) );
    }
}

//============================================================================
EFriendState GuiHelpers::comboIdxToFriendState( int comboIdx )
{
    switch( comboIdx )
    {
    case 0:
        return eFriendStateAdmin;
    case 1:
        return eFriendStateFriend;
    case 2:
        return eFriendStateGuest;
    case 3:
        return eFriendStateAnonymous;
    default:
        return eFriendStateIgnore;
    }   
}

//============================================================================
int GuiHelpers::friendStateToComboIdx( EFriendState friendState )
{
    switch( friendState )
    {
    case eFriendStateAdmin:
        return 0;
    case eFriendStateFriend:
        return 1;
    case eFriendStateGuest:
        return 2;
    case eFriendStateAnonymous:
        return 3;
    default:
        return 4;
    }
}

//============================================================================
void GuiHelpers::fillExpireTimeComboBox( QComboBox* comboBox )
{
    if( comboBox )
    {
        comboBox->clear();
        for( int i = eExpireTimeWhenResponseRxed; i < eMaxExpireTime; i++ )
        {
            comboBox->addItem( GuiParams::describeExpireTime( (EExpireTime)i ) );
        }
    }
}

//============================================================================
int GuiHelpers::expireTimeComboIdxToSeconds( int comboIdx )
{
    return GuiParams::getExpireTimeSeconds( (EExpireTime)comboIdx );
}

//============================================================================
void GuiHelpers::fillJoinRequest( QComboBox* comboBox )
{
    if( comboBox )
    {
        comboBox->clear();
        comboBox->addItem( GuiParams::describeJoinState( eJoinStateJoinRequested ) );
        comboBox->addItem( GuiParams::describeJoinState( eJoinStateJoinIsGranted ) );
        comboBox->addItem( GuiParams::describeJoinState( eJoinStateJoinDenied ) );
    }
}

//============================================================================
EJoinState GuiHelpers::comboIdxToJoinState( int comboIdx )
{
    switch( comboIdx )
    {
    case 0: return eJoinStateJoinRequested;
    case 1: return eJoinStateJoinIsGranted;
    default: return  eJoinStateJoinDenied;
    }
}

//============================================================================
uint8_t GuiHelpers::joinRequestToIndex( EJoinState joinState )
{
    switch( joinState )
    {
    case eJoinStateJoinRequested:
        return 0;
    case eJoinStateJoinIsGranted:
        return 1;
    default:
        return 2;
    }
}

//============================================================================
void GuiHelpers::setValuesFromIdentity( QWidget* curWidget, VxNetIdent* ident, QComboBox *  ageCombo, QComboBox * genderCombo, QComboBox * languageCombo, QComboBox * contentCombo )
{
    if( curWidget && ident && ageCombo && genderCombo && languageCombo && contentCombo )
    {
        ageCombo->setCurrentIndex( ident->getAgeType() );
        genderCombo->setCurrentIndex( ident->getGender() );
        languageCombo->setCurrentIndex( ident->getPrimaryLanguage() );
        contentCombo->setCurrentIndex( ident->getPreferredContent() );
    }
}

//============================================================================
void GuiHelpers::setIdentityFromValues( QWidget* curWidget, VxNetIdent* ident, QComboBox * age, QComboBox * genderCombo, QComboBox * languageCombo, QComboBox * contentCombo )
{
    if( curWidget && ident && age && genderCombo && languageCombo && contentCombo )
    {
        int ageValue = age->currentIndex();
        if( ( 0 > ageValue ) || ( eMaxAgeType <= ageValue ) )
        {
            ageValue = 0;
        }

        ident->setAgeType( (EAgeType)ageValue );

        int genderValue = genderCombo->currentIndex();
        if( ( 0 > genderValue ) || ( eMaxGenderType <= genderValue ) )
        {
            genderValue = 0;
        }

        ident->setGender( genderValue );

        int languageValue = languageCombo->currentIndex();
        if( ( 0 > languageValue ) || ( eMaxLanguageType <= genderValue ) )
        {
            languageValue = 0;
        }

        ident->setPrimaryLanguage( languageValue );

        int contentValue = contentCombo->currentIndex();
        if( ( 0 > contentValue ) || ( eMaxContentRating <= genderValue ) )
        {
            contentValue = 0;
        }

        ident->setPreferredContent( contentValue );
    }
}

//============================================================================
ActivityBase* GuiHelpers::findParentActivity( QWidget* widget )
{
    ActivityBase* actBase = nullptr;
    QObject * objWidget = widget;
    while( objWidget )
    {
        ActivityBase* actTemp = dynamic_cast<ActivityBase*>( objWidget );
        if( actTemp )
        {
            actBase = actTemp;
            break;
        }

        objWidget = objWidget->parent();
    }

    return actBase;
}

//============================================================================
QWidget* GuiHelpers::findAppletContentFrame( QWidget* widget )
{
    ActivityBase* actBase = findLaunchWindow( widget );
    if( actBase )
    {
        return actBase->getContentItemsFrame();
    }

    return nullptr;
}

//============================================================================
QWidget* GuiHelpers::findParentPage( QWidget* parent ) // this should return home or messenger page
{
    // from title bar find the Home page or messenger
    QFrame* pageFrame = nullptr;
    QObject* curParent = parent;

    QString launchPageObjName = OBJNAME_FRAME_LAUNCH_PAGE;
    QString messengerPageObjName = OBJNAME_FRAME_MESSAGER_PAGE;

    while( curParent )
    {
        QString objName = curParent->objectName();
        if( (objName == launchPageObjName) || (objName == messengerPageObjName) )
        {
            pageFrame = dynamic_cast<QFrame*>(curParent);
            if( pageFrame )
            {
                break;
            }
        }

        if( !curParent->parent() )
        {
            LogMsg( LOG_WARNING, "Object %s has no parent", objName.toUtf8().constData() );
            break;
        }
        else
        {
            curParent = curParent->parent();
        }
    }

    return pageFrame;
}

//============================================================================
QWidget* GuiHelpers::findParentContentFrame( QWidget* parent )
{
    ActivityBase* actBase = findParentActivity( parent );
    if( actBase )
    {
        return actBase->getContentItemsFrame();
    }
    else
    {
        return findParentPage( parent );
    }

    return nullptr;
}

//============================================================================
ActivityBase* GuiHelpers::findLaunchWindow( QWidget* widget )
{
    QObject * objWidget = findParentActivity( widget );
    QObject * prevWidget = objWidget;
    while( objWidget )
    {
        if( dynamic_cast<VxFrame*>( objWidget ) )
        {
            return dynamic_cast<ActivityBase*>( prevWidget );
        }

        prevWidget = objWidget;
        objWidget = objWidget->parent();
    }

    return nullptr;
}

//============================================================================
bool GuiHelpers::widgetToPluginSettings( EPluginType pluginType, PluginSettingsWidget* settingsWidget, PluginSetting& pluginSetting )
{
    bool result = false;
    if( ePluginTypeInvalid != pluginType && settingsWidget )
    {
        pluginSetting.setPluginType( pluginType );
        pluginSetting.setContentRating( (EContentRating)settingsWidget->getContentRatingComboBox()->currentIndex() );
        pluginSetting.setLanguage( (ELanguageType)settingsWidget->getLanguageComboBox()->currentIndex() );
        pluginSetting.setGender( (EGenderType)settingsWidget->getGenderComboBox()->currentIndex() );
        pluginSetting.setAgeType( (EAgeType)settingsWidget->getAgeComboBox()->currentIndex() );
        pluginSetting.setPluginUrl( false, settingsWidget->getServiceUrlEdit( false )->text().toUtf8().constData() );
        pluginSetting.setPluginUrl( true, settingsWidget->getServiceUrlEdit( true )->text().toUtf8().constData() );
        pluginSetting.setTitle( settingsWidget->getServiceTitleEdit()->text().toUtf8().constData() );
        pluginSetting.setGreetingMsg( settingsWidget->getGreetingEdit()->toPlainText().toUtf8().constData() );
        pluginSetting.setRejectMsg( settingsWidget->getRejectEdit()->toPlainText().toUtf8().constData() );

        pluginSetting.setThumnailId( settingsWidget->getThumbnailChooseWidget()->updateAndGetThumbnailId(), settingsWidget->getThumbnailChooseWidget()->getThumbnailIsCircular() );

        QString description =settingsWidget->getServiceDescriptionEdit()->toPlainText().trimmed();
        if( !description.isEmpty() )
        {
            pluginSetting.setDescription( description.toUtf8().constData() );
        }
        else
        {
            pluginSetting.setDescription( "" );
        }

        result = true;
    }

    return result;
}

//============================================================================
bool GuiHelpers::pluginSettingsToWidget( EPluginType pluginType, PluginSetting& pluginSetting, PluginSettingsWidget* settingsWidget )
{
    bool result = false;
    if( ePluginTypeInvalid != pluginType && settingsWidget )
    {
        settingsWidget->getAgeComboBox()->setCurrentIndex( GuiHelpers::ageToIndex( pluginSetting.getAgeType() ) );
        settingsWidget->getGenderComboBox()->setCurrentIndex( GuiHelpers::genderToIndex( pluginSetting.getGender() ) );
        settingsWidget->getContentRatingComboBox()->setCurrentIndex( GuiHelpers::contentRatingToIndex( pluginSetting.getContentRating() ) );
        settingsWidget->getLanguageComboBox()->setCurrentIndex( GuiHelpers::languageToIndex( pluginSetting.getLanguage() ) );

        settingsWidget->getServiceUrlEdit( false )->setText( pluginSetting.getPluginUrl( false ).c_str() );
        settingsWidget->getServiceUrlEdit( true )->setText( pluginSetting.getPluginUrl( true ).c_str() );

        settingsWidget->getServiceTitleEdit()->setText( pluginSetting.getTitle().c_str() );
        settingsWidget->getServiceDescriptionEdit()->appendPlainText( pluginSetting.getDescription().c_str() );
        settingsWidget->getGreetingEdit()->appendPlainText( pluginSetting.getGreetingMsg().c_str() );
        settingsWidget->getRejectEdit()->appendPlainText( pluginSetting.getRejectMsg().c_str() );

        settingsWidget->getThumbnailChooseWidget()->loadThumbnail( pluginSetting.getThumnailId(), pluginSetting.getThumbnailIsCircular() );
 
        result = true;
    }

    return result;
}

//============================================================================
bool GuiHelpers::createThumbFileName( VxGUID& assetGuid, QString& retFileName )
{
    bool validFileName = false;
    if( assetGuid.isVxGUIDValid() )
    {
        retFileName = VxGetAppDirectory( eAppDirThumbs ).c_str();
        if( !retFileName.isEmpty() )
        {
            retFileName += assetGuid.toHexString().c_str();
            retFileName += ".nlt"; // use extension not known as image so thumbs will not be scanned by android image gallery etc
            validFileName = true;
        }
    }

    return validFileName;
}

//============================================================================
bool GuiHelpers::makeCircleImage( QImage& image )
{
    QPixmap target( image.width(), image.height() );
    target.fill( Qt::transparent );

    QPainter painter( &target );

    // Set clipped region (circle) in the center of the target image
    QRegion clipRegion( QRect( 0, 0, image.width(), image.height() ), QRegion::Ellipse );
    painter.setClipRegion( clipRegion );

    painter.drawImage( 0, 0, image );
    image = target.toImage();
    return !image.isNull();
}

//============================================================================
bool GuiHelpers::makeCircleImage( QPixmap& targetPixmap )
{
    QPixmap target( targetPixmap.width(), targetPixmap.height() );
    target.fill( Qt::transparent );

    QPainter painter( &target );

    // Set clipped region (circle) in the center of the target image
    QRegion clipRegion( QRect( 0, 0, targetPixmap.width(), targetPixmap.height() ), QRegion::Ellipse );
    painter.setClipRegion( clipRegion );

    painter.drawPixmap( 0, 0, targetPixmap );
    targetPixmap = target;
    return !targetPixmap.isNull();
}

//============================================================================
uint64_t GuiHelpers::saveToPngFile( QImage& image, QString& fileName ) // returns file length
{
    bool result = !image.isNull() && !fileName.isEmpty();
    if( result )
    {
        result = image.save( fileName, "PNG" );
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiHelpers::saveToPngFile Invalid Param" );
        return 0;
    }

    if( result )
    {
        return VxFileUtil::fileExists( fileName.toUtf8().constData() );
    }

    return 0;
}

//============================================================================
uint64_t GuiHelpers::saveToPngFile( QPixmap& bitmap, QString& fileName ) // returns file length
{
    bool result = !bitmap.isNull() && !fileName.isEmpty();
    if( result )
    {
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        if( !bitmap.isNull() )
        {
            result = bitmap.save( fileName, "PNG" );
#else
        const QPixmap* bitmap = m_ThumbPixmap;
        if( bitmap )
        {
            result = bitmap->save( fileName, "PNG" );
#endif // QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiHelpers::saveToPngFile Invalid Param" );
        return 0;
    }

    if( result )
    {
        return VxFileUtil::fileExists( fileName.toUtf8().constData() );
    }  

    return 0;
}

//============================================================================
bool GuiHelpers::requestAndroidStoragePermissions( void )
{
#if defined (Q_OS_ANDROID)
    // MANAGE_EXTERNAL_STORAGE always returns false
    // bool result = checkUserPermission({ "android.permission.MANAGE_EXTERNAL_STORAGE" });
    // result &= checkUserPermission({ "android.permission.READ_EXTERNAL_STORAGE" });
    // return result;
    return checkUserPermission({ "android.permission.READ_EXTERNAL_STORAGE" });
#else
    return true;
#endif // defined (Q_OS_ANDROID)
}

//============================================================================
bool GuiHelpers::checkUserPermission( QString permissionName ) // returns false if user denies permission to use android hardware
{
#if defined (Q_OS_ANDROID)
    if( QtAndroidPrivate::Authorized != QtAndroidPrivate::checkPermission(permissionName).result() )
    {
        if( QtAndroidPrivate::Authorized != QtAndroidPrivate::requestPermission(permissionName).result() )
        {
            return false;
        }
    }

    return true;
#else
    return true;
#endif // defined (Q_OS_ANDROID)
}

//============================================================================
bool GuiHelpers::requestPermission( std::string permissionName )
{
#if !defined(Q_OS_ANDROID)
    return true;
#endif

#if defined (Q_OS_ANDROID) && QT_VERSION < QT_VERSION_CHECK(6,0,0)
    //Request required permissions at runtime.. does not seem to work with Qt 6.2.0
    for(const QString &permission : permissions)
    {
        LogMsg( LOG_DEBUG, "requesting permission %s", permission.toUtf8().constData() );
        auto result = QtAndroid::checkPermission(permission);
        if(result == QtAndroid::PermissionResult::Denied)
        {
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if(resultHash[permission] == QtAndroid::PermissionResult::Denied)
            {
                LogMsg( LOG_DEBUG, "DENIED permission %s", permission.toUtf8().constData() );
                return false;
            }

            LogMsg( LOG_DEBUG, "ACCEPTED permission %s", permission.toUtf8().constData() );
        }
    }

    LogMsg( LOG_DEBUG, "permission done" );
#endif
#if defined (Q_OS_ANDROID)

    const QString requiredPermission(QLatin1String( permissionName.c_str() ) );
    auto permissionResult = QtAndroidPrivate::checkPermission(requiredPermission).result();
    if( permissionResult != QtAndroidPrivate::Authorized )
    {
        if( QtAndroidPrivate::Authorized !=  QtAndroidPrivate::requestPermission(requiredPermission).result() )
        {
            LogMsg(LOG_INFO, "Cannot Proceed without %s permission", permissionName.c_str() );
            return false;
        }
    }
#endif // defined (Q_OS_ANDROID)

    return true;
}

//============================================================================
void GuiHelpers::fillHostType( QComboBox* comboBox, bool excludePeerHost )
{
    comboBox->addItem( GuiParams::describeHostType( eHostTypeGroup ) );
    comboBox->addItem( GuiParams::describeHostType( eHostTypeChatRoom ) );
    comboBox->addItem( GuiParams::describeHostType( eHostTypeRandomConnect ) );
    comboBox->addItem( GuiParams::describeHostType( eHostTypeNetwork ) );
    comboBox->addItem( GuiParams::describeHostType( eHostTypeConnectTest ) );

    if( !excludePeerHost )
    { 
        comboBox->addItem( GuiParams::describeHostType( eHostTypePeerUser ) );
    }
}

//============================================================================
EHostType GuiHelpers::comboIdxToHostType( int comboIdx )
{
    switch( comboIdx )
    {
    case 0:
        return eHostTypeGroup;
    case 1:
        return eHostTypeChatRoom;
    case 2:
        return eHostTypeRandomConnect;
    case 3:
        return eHostTypeNetwork;
    case 4:
        return eHostTypeConnectTest;
    case 5:
        return eHostTypePeerUser;

    default:
        return eHostTypeUnknown;
    }
}

//============================================================================
QMessageBox::StandardButton GuiHelpers::errorMsgBox (EErrMsgType errMsgType, QWidget* parent, GuiUser* guiUser )
{
    std::string userName = guiUser ? guiUser->getOnlineName().c_str() : "Unknown";
    QMessageBox::StandardButton buttonResult{ QMessageBox::NoButton };
    switch( errMsgType )
    {
    case eErrMsgUserUnavailable:
        buttonResult = QMessageBox::information( parent, QObject::tr( "User Unavailable" ), 
            QObject::tr( "User " ) + userName.c_str() + QObject::tr( " Is Unavailable" ), QMessageBox::Ok );
        break;

    case eErrMsgAlreadyInSession:
        buttonResult = QMessageBox::information( parent, QObject::tr( "Already In A Session" ),
            QObject::tr( "Already in session. Please close existing session and try again" ), QMessageBox::Ok );
        break;

    case eErrMsgOfferSent:
        buttonResult = QMessageBox::information( parent, QObject::tr( "Sent User An Offer" ),
            QObject::tr( "Offer Was Sent To  " ) + userName.c_str(), QMessageBox::Ok );
        break;

    case eErrMsgOfferSendFailed:
        buttonResult = QMessageBox::information( parent, QObject::tr( "Send Offer Failed" ),
            QObject::tr( "Offer Send Failed  " ), QMessageBox::Ok );
        break;

    case eErrMsgNotConnectedToHost:
        buttonResult = QMessageBox::information( parent, QObject::tr( "Not Connected" ),
            QObject::tr( "You are not connected to host " ), QMessageBox::Ok );
        break;

    case eErrMsgNoUserSelectedToSendTo:
        buttonResult = QMessageBox::information( parent, QObject::tr( "No User Selected" ),
            QObject::tr( "You must select a user to send to " ), QMessageBox::Ok );
        break;

    case eErrMsgUserIsOffline:
        buttonResult = QMessageBox::information( parent, QObject::tr( "User is offline" ),
            QObject::tr( "User is no longer connected" ), QMessageBox::Ok );
        break;

    case eErrMsgPurgeEverythingWarning:
        buttonResult = QMessageBox::question( parent, QObject::tr( "Purge Everthing Warning" ),
            QObject::tr( "This action will delete everything not locked including Identity, Downloaded Files, Storyboard, Message History Etc.\nThe action cannot be undone\nAre you very sure you want to proceed?" ) );
        break;

    case eErrMsgVoiceMessageTooShort:
        buttonResult = QMessageBox::information( parent, QObject::tr( "Voice Message Too Short" ),
            QObject::tr( "The Voice Message was too short and will not be sent." ) );
        break;

    case eErrMsgVideoClipTooShort:
        buttonResult = QMessageBox::information( parent, QObject::tr( "Video clip Too Short" ),
            QObject::tr( "The Video Clip was too short and will not be sent." ) );
        break;

    case eErrMsgVideoClipFailedToStart:
        buttonResult = QMessageBox::information( parent, QObject::tr( "Video record failed to start" ),
            QObject::tr( "Video record failed to start." ) );
        break;

    default:
        buttonResult = QMessageBox::information( parent, QObject::tr( "Unknown Error" ),
            QObject::tr( "Unknown Error " ) + QString::number(errMsgType) + QObject::tr( " for user " ) + userName.c_str(), QMessageBox::Ok );
        break;
    }

    return buttonResult;
}

//============================================================================
void GuiHelpers::processQtEvents( int ms )
{
	QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
}

//============================================================================
static QString getRealPathFromUri(const QUrl &url)
{
    QString path = "";

    QFileInfo info = QFileInfo(url.toString());
    if(info.isFile())
    {
        QString abs = QFileInfo(url.toString()).absoluteFilePath();
        if(!abs.isEmpty() && abs != url.toString() && QFileInfo(abs).isFile())
        {
            return abs;
        }
    }
    else if(info.isDir())
    {
        QString abs = QFileInfo(url.toString()).absolutePath();
        if(!abs.isEmpty() && abs != url.toString() && QFileInfo(abs).isDir())
        {
            return abs;
        }
    }
    QString localfile = url.toLocalFile();
    if((QFileInfo(localfile).isFile() || QFileInfo(localfile).isDir()) && localfile != url.toString())
    {
        return localfile;
    }
#ifdef Q_OS_ANDROID
    QJniObject jUrl = QJniObject::fromString(url.toString());
    QJniObject jContext = QtAndroidPrivate::context();
    QJniObject jContentResolver = jContext.callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
    QJniObject jUri = QJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", jUrl.object<jstring>());
    QJniObject jCursor = jContentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", jUri.object<jobject>(), nullptr, nullptr, nullptr, nullptr);
    QJniObject jScheme = jUri.callObjectMethod("getScheme", "()Ljava/lang/String;");
    QJniObject authority;
    if(jScheme.isValid())
    {
        authority = jUri.callObjectMethod("getAuthority", "()Ljava/lang/String;");
    }
    if(authority.isValid() && authority.toString() == "com.android.externalstorage.documents")
    {
        QJniObject jPath = jUri.callObjectMethod("getPath", "()Ljava/lang/String;");
        path = jPath.toString();
    }
    else if(jCursor.isValid() && jCursor.callMethod<jboolean>("moveToFirst"))
    {
        QJniObject jColumnIndex = QJniObject::fromString("_data");
        jint columnIndex = jCursor.callMethod<jint>("getColumnIndexOrThrow", "(Ljava/lang/String;)I", jColumnIndex.object<jstring>());
        QJniObject jRealPath = jCursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex);
        path = jRealPath.toString();
        if(authority.isValid() && authority.toString().startsWith("com.android.providers") && !url.toString().startsWith("content://media/external/"))
        {
            QStringList list = path.split(":");
            if(list.count() == 2)
            {
                QString type = list.at(0);
                QString id = list.at(1);
                if(type == "image")
                    type = type + "s";
                if(type == "document" || type == "documents")
                    type = "file";
                if(type == "msf")
                    type = "downloads";
                if(QList<QString>({"images","video","audio"}).contains(type))
                    type = type + "/media";
                path = "content://media/external/"+type;
                path = path + "/" + id;
                return getRealPathFromUri(path);
            }
        }
    }
    else
    {
        QJniObject jPath = jUri.callObjectMethod("getPath", "()Ljava/lang/String;");
        path = jPath.toString();
    }

    if(path.startsWith("primary:"))
    {
        path = path.remove(0,QString("primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/document/primary:"))
    {
        path = path.remove(0,QString("/document/primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/tree/primary:"))
    {
        path = path.remove(0,QString("/tree/primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/storage/emulated/0/"))
    {
        path = path.remove(0,QString("/storage/emulated/0/").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/tree//"))
    {
        path = path.remove(0,QString("/tree//").length());
        path = "/" + path;
    }
    if(!QFileInfo(path).isFile() && !QFileInfo(path).isDir() && !path.startsWith("/data"))
        return url.toString();
    return path;
#else
    return url.toString();
#endif

}

//============================================================================
void GuiHelpers::contentUrlToFileSystemPath( QString& contentUrl )
{
    QString realPath = getRealPathFromUri(contentUrl);
    QFileInfo fileInfo(contentUrl);

    LogMsg(LOG_DEBUG, "contentUrlToFileSystemPath %s result %s fileInfo %s ",
            contentUrl.toUtf8().constData(),
           realPath.toUtf8().constData(), fileInfo.fileName().toUtf8().constData() );


               /*
    QUrl url_path(contentUrl);

    qDebug() << "[Original String]:" << contentUrl;
    qDebug() << "--------------------------------------------------------------------";
    qDebug() << "[getRealPathFromUri]:" << getRealPathFromUri(url_path);


    qDebug() << "(QUrl::toEncoded)          :" << url_path.toEncoded(QUrl::FullyEncoded);
    qDebug() << "(QUrl::url)                :" << url_path.url();
    qDebug() << "(QUrl::toString)           :" << url_path.toString();
    qDebug() << "(QUrl::toDisplayString)    :" << url_path.toDisplayString(QUrl::FullyEncoded);
    qDebug() << "(QUrl::fromPercentEncoding):" << url_path.fromPercentEncoding(contentUrl.toUtf8());
    qDebug() << "(QQmlFile::urlToLocalFileOrQrc):" << QQmlFile::urlToLocalFileOrQrc(contentUrl).toStdString();



    qDebug() << "(QFileInfo::canonicalPath)      :" << fileInfo.canonicalPath();
    qDebug() << "(QFileInfo::absolutePath)       :" << fileInfo.absolutePath();
    qDebug() << "(QFileInfo::path)               :" << fileInfo.path();
    qDebug() << "(QFileInfo::baseName)           :" << fileInfo.baseName();
    qDebug() << "(QFileInfo::filePath)           :" << fileInfo.filePath();
    qDebug() << "(QFileInfo::absoluteFilePath)   :" << fileInfo.absoluteFilePath();
    qDebug() << "(QFileInfo::canonicalFilePath)  :" << fileInfo.canonicalFilePath();

    QDir dir(contentUrl);
    qDebug() << "(QDir::canonicalPath)      :" << dir.canonicalPath();
    qDebug() << "(QDir::absolutePath)       :" << dir.absolutePath();
    qDebug() << "(QDir::path)               :" << dir.path();

    qDebug() << "(QDir::clean)           :" << dir.cleanPath(contentUrl);
    qDebug() << "(QDir::absoluteFilePath)   :" << dir.absoluteFilePath(contentUrl);
*/
    static QVector<QStringList> locations;
    if(locations.isEmpty())
    {
        locations.push_back( QStandardPaths::standardLocations(QStandardPaths::AppDataLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::DesktopLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::MusicLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::MoviesLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::PicturesLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::TempLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::HomeLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::AppLocalDataLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::CacheLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::GenericDataLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::RuntimeLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::ConfigLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::DownloadLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::GenericCacheLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::GenericConfigLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::AppConfigLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::PublicShareLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::TemplatesLocation ) );
    }

    int row = 0;
    for( auto stringList : locations )
    {
        row++;
        int col = 0;
        for( auto string : stringList )
        {
            col++;
            QDir dir(string);


            LogMsg(LOG_DEBUG, "%d-%d %s \n con %s \n abs %s \n path %s",
                   row, col, string.toUtf8().constData(),
                   dir.canonicalPath().toUtf8().constData(),
                    dir.absolutePath().toUtf8().constData(),
                        dir.path().toUtf8().constData()
                );
        }
    }


    LogMsg(LOG_DEBUG, "END");
}

std::string GuiHelpers::getRealFileName( QString selectedFileIn )
{
#if defined(TARGET_OS_ANDROID)
    std::string selectedFile = selectedFileIn.toUtf8().constData();
    std::string justFile;
    std::string justPath;
    /*
    VxFileUtil::seperatePathAndFile( selectedFile, justPath, justFile );
    if( justPath.empty() || justFile.empty() )
    {
        return selectedFile;
    }
*/

    QDir browseDir( "content://com.android.externalstorage.documents/document/primary:NoLimitConnectData/MediaTestFiles" );

    QFileInfoList fileInfoList = browseDir.entryInfoList();
    LogMsg( LOG_VERBOSE, "%d files in dir %s", fileInfoList.size(), justPath.c_str() );
    for( auto fileListInfo : fileInfoList )
    {
        std::string fileName = fileListInfo.filePath().toUtf8().constData();

        if( fileName.empty() )
        {
            continue;
        }

        if( fileListInfo.isDir() )
        {
            LogMsg( LOG_VERBOSE, "Directory %s", fileName.c_str() );


        }
        else if( fileListInfo.isExecutable() )
        {
            LogMsg( LOG_VERBOSE, "Executable ignored File %s", fileName.c_str() );
        }
        else if( fileListInfo.isReadable() )
        {
            int64_t fileLen = fileListInfo.size();

            if( fileLen )
            {
                uint8_t fileType = VxFileNameToFileType( fileName );

            }
            else
            {
                LogMsg( LOG_VERBOSE, "Could Not Resolve file length of file %s", fileName.c_str() );
            }
        }
        else
        {
            LogMsg( LOG_VERBOSE, "NOT Readable File %s", fileName.c_str() );
        }
    }

    return selectedFile;
#else
    return selectedFileIn.toUtf8().constData();
#endif // defined(TARGET_OS_ANDROID)
}
