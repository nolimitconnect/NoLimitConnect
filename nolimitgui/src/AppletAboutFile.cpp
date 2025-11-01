//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletAboutFile.h"

#include "ActivityBrowseFiles.h"
#include "ActivityMsgBoxYesNo.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "AppletDownloads.h"

#include "FileShareItemWidget.h"
#include "MyIcons.h"
#include "AppGlobals.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"

#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiPlayerMgr.h"

#include <AssetBase/AssetPlaySession.h>
#include <AssetMgr/AssetMgr.h>
#include <AssetMgr/AssetInfo.h>
#include <P2PEngine/P2PEngine.h>

#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>

#include "PermissionWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletAboutFile.h"

#include <QPlainTextEdit>
#include <QScrollBar>

namespace
{
    const int MAX_LOG_EDIT_BLOCK_CNT = 1000;
    const int MAX_INFO_MSG_SIZE = 2048;
}

QPlainTextEdit* AppletAboutFile::getInfoEdit( void ) { return ui.m_InfoPlainTextEdit; }

//============================================================================
AppletAboutFile::AppletAboutFile( AppCommon& app, QWidget* parent, QString launchParam )
    : AppletBase( OBJNAME_APPLET_FILE_OFFER_SELECT, app, parent )
    , ui(*(new Ui::AppletAboutFileUi))
{
    setAppletType( eAppletAboutFile );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( getAppletType() ) ); 

    ui.m_BrowseButton->setVisible( false );
    ui.m_IdentWidget->setVisible( false );

    getInfoEdit()->setMaximumBlockCount( MAX_LOG_EDIT_BLOCK_CNT );
    getInfoEdit()->setReadOnly( true );

    connect( ui.m_BrowseButton, SIGNAL(clicked()), this, SLOT(slotBrowseFolderButClick() ) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletAboutFile::~AppletAboutFile()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletAboutFile::setFileInfo( FileInfo& fileInfo )
{
    m_FileInfo = fileInfo;
    std::string fileName = m_FileInfo.getFileNameAndPath();
    if( fileName.empty() )
    {
        GuiHelpers::showFileNameEmptyError( this );
        closeApplet();
        return;
    }

    // see if we have asset for it.. it has more details
    AssetBaseInfo assetInfo;
    if( m_MyApp.getEngine().getAssetMgr().getAsset( fileName, assetInfo ) )
    {
        m_FileInfo = assetInfo.getFileInfo();
        if( updateFromFileInfo( m_FileInfo ) )
        {
            updateFromAssetInfo( assetInfo );
        }   
    }
    else
    {
        updateFromFileInfo( m_FileInfo );
    }
}

//============================================================================
void AppletAboutFile::slotBrowseFolderButClick( void )
{
    QString curDir = ui.m_FilePathLabel->text();
    if( VxFileUtil::directoryExists( curDir.toUtf8().constData() ) )
    {

    }
}

//============================================================================
bool AppletAboutFile::updateFromFileInfo( FileInfo& fileInfo )
{
    ui.m_FileNameLabel->setText( m_FileInfo.getFileName().c_str() );
    ui.m_FileSizeLabel->setText( GuiParams::describeFileLength( m_FileInfo.getFileLength() ) );
    ui.m_FilePathLabel->setText( m_FileInfo.getFilePath().c_str() );

    bool isStream = m_FileInfo.isStream();
    QString yes = QObject::tr( " : Yes" );
    QString no = QObject::tr( " : No" );

    infoMsg( QObject::tr( "Is Stream" ) + ( isStream ? yes : no ) );

    bool showThumb{ false };
    VxGUID thumbId = m_FileInfo.getThumbId();
    QImage thumbImage;
    GetAppInstance().getThumbImage( thumbId, thumbImage );
    if( !thumbImage.isNull() )
    {
        ui.m_ThumbnailViewWidget->setImage( thumbId, thumbImage );
        ui.m_ThumbIdText->setText( thumbId.toHexString().c_str() );
        showThumb = true;
    }

    showThumbnail( showThumb );

    return true;
}

//============================================================================
bool AppletAboutFile::updateFromAssetInfo( AssetBaseInfo& assetInfo )
{

    return true;
}

//============================================================================
void AppletAboutFile::showThumbnail( bool visible )
{
    ui.m_ThumbFrame->setVisible( visible );
}

//============================================================================
void AppletAboutFile::infoMsg( const QString& text )
{
    getInfoEdit()->appendPlainText( text ); // Adds the message to the widget
    getInfoEdit()->verticalScrollBar()->setValue( getInfoEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
}
