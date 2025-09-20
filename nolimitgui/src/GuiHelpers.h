#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include "AppDefs.h"

#include <CoreLib/VxFileTypeMasks.h>
#include <CoreLib/VxGUID.h>
#include <CoreLib/VxXferDefs.h>
#include <PktLib/VxCommon.h>

#include <QMessageBox>
#include <QString>

#define SUPPORTED_IMAGE_FILES "Image files (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tif *.tiff *.svg *.xbm *.xpm)"

enum EErrMsgType
{
    eErrMsgNotSet = 0,
    eErrMsgUserUnavailable,
    eErrMsgUserIsOffline,
    eErrMsgAlreadyInSession,
    eErrMsgSessionNotFound,
    eErrMsgOfferSent,
    eErrMsgOfferSendFailed,
    eErrMsgNotConnectedToHost,
    eErrMsgNoUserSelectedToSendTo,
    eErrMsgPurgeEverythingWarning,

    eErrMsgVoiceMessageTooShort,
    eErrMsgVideoClipTooShort,
    eErrMsgVideoClipFailedToStart,

    eMaxEErrMsgBoxType
};

enum EMediaFileType
{
    eMediaFileAny = 0,
    eMediaFileVideo,
    eMediaFileAudio,
    eMediaFileImage,

    eMaxMediaFileType
};

class ActivityBase;
class AppCommon;
class AppletBase;
class AssetBaseInfo;
class FileInfo;
class GuiUser;
class PluginSetting;
class PluginSettingsWidget;
class BottomBarWidget;
class TitleBarWidget;
class VxFileInfoBase;
class VxFrame;
class VxNetIdent;

class QComboBox;
class QDir;
class QFileInfo;
class QFrame;
class QFontMetrics;
class QImage;
class QPixmap;
class QWidget;

class GuiHelpers
{
public:
	static std::string			browseForDirectory( QString startDir = "", QWidget* parent = nullptr );

    static bool                 browseForFile( QWidget* parent, enum EMediaFileType mediaFileType, FileInfo& retFileInfo, QString startDir = "" );
    static bool                 browseForFile( QWidget* parent, enum EMediaFileType mediaFileType, VxFileInfoBase& retFileInfo, QString startDir = "" );
    static bool                 browseForFile( QWidget* parent, enum EMediaFileType mediaFileType, std::string& retFileName, QString startDir = "" );

    static void                 listFilesInFolder( QDir& folder, std::vector<VxFileInfoBase>& retFileList, uint8_t fileFilterMask = VXFILE_TYPE_AUDIO_VIDEO_PHOTO );
    static bool                 qtFileInfoToVxFileInfo( const QFileInfo& fileInfo, VxFileInfoBase& retFileInfo, uint8_t fileFilterMask = VXFILE_TYPE_AUDIO_VIDEO_PHOTO );

    static uint64_t             testCanReadFile( std::string fileNameAndPath );

    static QString              fileMaskToFileFilter( uint8_t fileMask );
    static QString              fileExtensionToFilter( QString fileExtensions );

    static int					calculateTextHeight( QFontMetrics& fontMetrics, QString textStr );
    static bool					copyResourceToOnDiskFile( QString resourcePath, QString fileNameAndPath );

    static EApplet              getAppletThatPlaysFile( AppCommon& myApp, uint8_t fileType, QString fullFileName, VxGUID& assetId, bool isStream );
    static EApplet              getAppletThatPlaysFile( AppCommon& myApp, AssetBaseInfo& assetInfo );

    static bool                 isAppletAService( EApplet applet );
    static bool                 isAppletAClient( EApplet applet );
    static EApplet              hostTypeToHostClientApplet( EHostType hostType );

    static EPluginType          getAppletAssociatedPlugin( EApplet applet );
    static EApplet              pluginTypeToEditApplet( EPluginType pluginType );
    static EApplet              pluginTypeToSettingsApplet( EPluginType pluginType );
    static EMyIcons             pluginTypeToSettingsIcon( EPluginType pluginType );
    static EApplet              pluginTypeToUserApplet( EPluginType pluginType );
    static EApplet              pluginTypeToViewApplet( EPluginType pluginType );
    // return applet capable of accepting session with another user
    static EApplet              pluginTypeToSessionApplet( EPluginType pluginType );
    // return default frame the session applet should launch in
    static QFrame*              pluginTypeToDefaultContentFrame( EPluginType pluginType );
    // return opposite of default frame the session applet should launch in
    static QFrame*              pluginTypeToOppositeDefaultContentFrame( EPluginType pluginType );

    static bool                 isPluginSingleSession( EPluginType pluginType );
    //! which plugins to show in permission list
    static bool                 isPluginAPrimaryService( EPluginType pluginType );
    static bool                 getSecondaryPlugins( EPluginType pluginType, QVector<EPluginType> secondaryPlugins );

    static bool                 isMessagerFrame( QWidget* curWidget );

    /// @brief get home or messenger topmost widget
    static QFrame*              getParentPageFrame( QWidget* curWidget );
    /// @brief get home or messenger topmost frame widget object name
    static QString              getParentPageFrameName( QWidget* curWidget );
    /// @brief messenger frame topmost widget
    static QFrame*              getMessengerPageFrame( QWidget* curWidget );
    /// @brief get launch frame topmost widget
    static QFrame*              getLaunchPageFrame( QWidget* curWidget );
    /// @brief get opposite page frame of current widget's frame
    static QFrame*              getOppositePageFrame( QWidget* curWidget );
    /// @brief get frame with given object name
    static QFrame*              findContentFrame( QString& contentFrameObjName );

    static VxFrame*             getVxFrame( QWidget* curWidget );
    //static QFrame*              getVxFrameContentItemsFrame( QWidget* curWidget );
    //static TitleBarWidget*      getVxFrameTitleBarWidget( QWidget* curWidget );
    //static BottomBarWidget*     getVxFrameBottomBarWidget( QWidget* curWidget );

    static bool                 validateUserName( QWidget* curWidget, QString strUserName );
    static bool                 validateMoodMessage( QWidget* curWidget, QString strMoodMsg );
    static bool                 validateAge( QWidget* curWidget, int age );

    static void                 setValuesFromIdentity( QWidget* curWidget, VxNetIdent* ident, QComboBox * age, QComboBox * genderCombo, QComboBox * languageCombo, QComboBox * contentCombo );
    static void                 setIdentityFromValues( QWidget* curWidget, VxNetIdent* ident, QComboBox * age, QComboBox * genderCombo, QComboBox * languageCombo, QComboBox * contentCombo );

    static void                 fillPermissionComboBox( QComboBox * permissionComboBox );
    static EFriendState         comboIdxToFriendState( int comboIdx );
    static int                  friendStateToComboIdx( EFriendState friendState );

    static void                 fillExpireTimeComboBox( QComboBox* permissionComboBox );
    static int                  expireTimeComboIdxToSeconds( int comboIdx );

    static void                 fillAge( QComboBox * comboBox );
    static EAgeType             getAge( QComboBox * comboBox );
    static bool                 setAge( QComboBox * comboBox, EAgeType ageType );
    static uint8_t              ageToIndex( EAgeType age );

    static void                 fillGender( QComboBox * comboBox );
    static EGenderType          getGender( QComboBox * comboBox );
    static bool                 setGender( QComboBox * comboBox, EGenderType gender );
    static uint8_t              genderToIndex( EGenderType gender );

    static void                 fillLanguage( QComboBox * comboBox );
    static ELanguageType        getLanguage( QComboBox * comboBox );
    static bool                 setLanguage( QComboBox * comboBox, ELanguageType language );
    static uint16_t             languageToIndex( ELanguageType language );

    static void                 fillContentRating( QComboBox * comboBox );
    static EContentRating       getContentRating( QComboBox * comboBox );
    static bool                 setContentRating( QComboBox * comboBox, EContentRating contentRating );
    static uint8_t              contentRatingToIndex( EContentRating content );

    static void                 fillContentCatagory( QComboBox * comboBox );
    static uint8_t              contentCatagoryToIndex( EContentCatagory content );

    static void                 fillJoinRequest( QComboBox* comboBox );
    static EJoinState           comboIdxToJoinState( int comboIdx );
    static uint8_t              joinRequestToIndex( EJoinState joinState );

    static ActivityBase*        findParentActivity( QWidget* widget );
    static QWidget*             findAppletContentFrame( QWidget* widget );
    static ActivityBase*        findLaunchWindow( QWidget* widget );
    static AppletBase *         findParentApplet( QWidget* widget );
    static QWidget*             findParentPage( QWidget* parent ); // this should return home or messenger page
    static QWidget*             findParentContentFrame( QWidget* parent );

    static bool                 widgetToPluginSettings( EPluginType pluginType, PluginSettingsWidget* settingsWidget, PluginSetting& pluginSetting );
    static bool                 pluginSettingsToWidget( EPluginType pluginType, PluginSetting& pluginSetting, PluginSettingsWidget* settingsWidget );

    static bool                 createThumbFileName( VxGUID& assetGuid, QString& retFileName );
    static bool                 makeCircleImage( QImage& image );
    static bool                 makeCircleImage( QPixmap& targetPixmap );
    static uint64_t             saveToPngFile( QImage& image, QString& fileName ); // returns file length
    static uint64_t             saveToPngFile( QPixmap& pixmap, QString& fileName ); // returns file length

    static void                 fillHostType( QComboBox* comboBox, bool excludePeerHost = true );
    static EHostType            comboIdxToHostType( int comboIdx );

    static QMessageBox::StandardButton errorMsgBox( EErrMsgType errMsgType, QWidget* parent = nullptr, GuiUser* guiUser = nullptr );

    static void                 processQtEvents( int ms = 50 );

    static void                 contentUrlToFileSystemPath( QString& contentUrl );

    static std::string          getRealFileName( QString selectedFile );

    static void                 showApplicationNotReadyError( bool appReadyButNetworkNotReady, QWidget* parent = nullptr );
    static void                 showFilePermissionError( QWidget* parent = nullptr );
    static void                 showRequiresOpenPort( QWidget* parent = nullptr );
    static void                 showInviteInvalidError( QWidget* parent = nullptr );
    static void                 showInviteMyselfError( QWidget* parent = nullptr );

    static void                 showUserNotFoundError( QWidget* parent = nullptr );
    static void                 showUserNotOnlineError( QWidget* parent = nullptr );
    static void                 showRequiresFriendshipError( QWidget* parent = nullptr );
    static void                 showInvalidHostIdError( QWidget* parent = nullptr );
    static void                 showInvalidHostTypeError( QWidget* parent = nullptr );
    static void                 showInvalidUrlrror( QWidget* parent = nullptr );
    static void                 showNoMembersOnlineError( QWidget* parent = nullptr );

    static void                 showAddAssetFailedError( void );
    static void                 showHostIsDisabledError( EHostType hostType );

    static void                 showCreateInvite( EHostType hostType, QWidget* parent = nullptr );

    static bool                 confirmDeleteFile( AppCommon& appCommon, QFrame* contentFrame, bool shredFile, QString fileName = "" );

    // android permissions
    static int                  getAndroidSDKVersion( void );
    static bool                 havePermission( QString permissionName );
    static bool                 requestFilePermission( enum EMediaFileType permissionType, bool showUserMsgIfDenied = true );


};

