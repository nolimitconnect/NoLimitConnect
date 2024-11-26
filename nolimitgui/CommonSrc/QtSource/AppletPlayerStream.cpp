//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPlayerStream.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "BottomBarWidget.h"
#include "GuiPlayerMgr.h"
#include "SoundMgr.h"
#include "VxMenuButton.h"

#include <P2PEngine/P2PEngine.h>
#include <VirtStream/VirtStreamMgr.h>

#include "MediaPlayerNlc.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletPlayerStream.h"

#define ENABLE_NLC_PLAYER_STREAMS 1

namespace
{
	const int PROCESS_QT_DEFAULT_MS = 50;

	void ProcessQtEvents( int ms = PROCESS_QT_DEFAULT_MS )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
	}
}

RenderGlWidget*		AppletPlayerStream::getRenderConsumer( void )		{ return ui.m_RenderWidget; }
QSlider*			AppletPlayerStream::getPlayPosSlider( void )		{ return ui.m_PlayControlWidget->getPlayPosSlider(); }
QPushButton*		AppletPlayerStream::getReplayButton( void )			{ return ui.m_ReplayButton; }
VxPushButton*		AppletPlayerStream::getPlayPauseButton( void )		{ return ui.m_PlayControlWidget->getPlayPauseButton(); }
PlayControlWidget*	AppletPlayerStream::getPlayControlWidget( void )	{ return ui.m_PlayControlWidget; }

//============================================================================
AppletPlayerStream::AppletPlayerStream( AppCommon& app, QWidget* parent )
: AppletPlayerNlcBase( OBJNAME_APPLET_PLAYER_STREAM, app, parent )
, ui(*(new Ui::AppletPlayerStreamUi))
{
	initAppletPlayerStream();
}

//============================================================================
void AppletPlayerStream::initAppletPlayerStream( void )
{
	setAppletType( eAppletPlayerStream );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( slotAppletClosing() ) );
	connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

	ui.setupUi( getContentItemsFrame() );
	ui.m_PlayControlWidget->setVisible( false );
	ui.m_ReplayButton->setVisible( false );

    setMenuBottomVisibility( true );

    BottomBarWidget * bottomBar = getBottomBarWidget();
    if( bottomBar )
    {
        setupBottomMenu( bottomBar->getMenuButton() );
    }

#if 0
	ui.m_StreamsComboBox->setEnabled( false ); // do not enable until media player is ready
	ui.m_StreamsComboBox->addItem( "Debug Streams" );

	m_MyApp.getEngine().getAssetMgr().getStreamableAssets( m_StreamableAssets );
	
	for( auto& streamAsset : m_StreamableAssets )
	{
		streamAsset.setIsStream( true );
		streamAsset.setPluginType( ePluginTypeFileShareClient );
		streamAsset.setDestUserId( m_MyApp.getMyOnlineId() );
		ui.m_StreamsComboBox->addItem( streamAsset.getAssetName().c_str() );
	}

#else
    ui.m_StreamsComboBox->setVisible( false );
#endif // defined(DEBUG)

	connect( ui.m_StreamsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMediaStreamComboBoxSelectionChange(int)) );

	connect( ui.m_ReplayButton, SIGNAL(clicked()), this, SLOT( slotPlayButtonClick() ) );

	connect( ui.m_RenderWidget, SIGNAL(signalLeftMouseButtonClick()), this, SLOT(slotLeftMouseButtonClick()));

	connect( ui.m_PlayControlWidget, SIGNAL(signalPlayPauseButtonClicked()), this, SLOT(slotPlayPauseButtonClick()));
	connect( ui.m_PlayControlWidget, SIGNAL(signalStopButtonClicked()), this, SLOT(slotStopButtonClick()));

	onAppletInitialized();
}

//============================================================================
AppletPlayerStream::~AppletPlayerStream()
{
	stopMediaIfPlaying();
}

//============================================================================
void AppletPlayerStream::slotAppletClosing( void )
{
	GetVirtStreamMgr().onPlaybackStopped( m_LclSessionId );
}

//============================================================================
void AppletPlayerStream::setupBottomMenu( VxMenuButton* menuButton )
{
	if( menuButton )
	{
		menuButton->setMenuId( 1 );
		menuButton->addMenuItem( eMenuItemShowRecent );
		menuButton->addMenuItem( eMenuItemShowWatched );
		menuButton->addMenuItem( eMenuItemShowLibrary );
		menuButton->addMenuItem( eMenuItemBrowse );
	}

	connect( menuButton, SIGNAL( signalMenuItemSelected( int, EMenuItemType ) ),
		this, SLOT( slotMenuItemSelected( int, EMenuItemType ) ) );
}

//============================================================================
void AppletPlayerStream::slotMenuItemSelected( int menuId, EMenuItemType menuItemType )
{
	//ActivityBrowseStreams* dlgBrowse;
	//switch( menuItemType )
	//{
	//case eMenuItemBrowse:
	//	dlgBrowse = new ActivityBrowseStreams( m_MyApp, eStreamFilterVideoOnly, this, true );
	//	dlgBrowse->setAppletType( getAppletType() );
	//	dlgBrowse->exec();
	//	if( dlgBrowse->getWasStreamSelected() )
	//	{
 //           std::string fileStr = dlgBrowse->getSelectedStreamInfo().getFullStreamName();
 //           QString fileName = dlgBrowse->getSelectedStreamInfo().getFullStreamName().c_str();
 //           if( VxStreamUtil::fileExists(fileStr.c_str()) )
 //           {
 //               playStream( fileName );
 //           }
 //           else
 //           {
 //               QMessageBox::information( this, QObject::tr("Stream does not exist"), fileName, QMessageBox::Ok );
 //           }
	//	}

	//	break;

	//default:
	//	break;
	//}
}

//============================================================================
void AppletPlayerStream::slotMediaStreamComboBoxSelectionChange( int cbIdx )
{
	if( cbIdx < 1 )
	{
		return;
	}

	cbIdx--;
	
	if( cbIdx >= m_StreamableAssets.size() )
	{
		LogMsg( LOG_ERROR, "%s invalid combo box index", __func__ );
		return;
	}

	VxGUID lclSessionId;
	lclSessionId.initializeWithNewVxGUID();
	m_MyApp.getPlayerMgr().playStream( m_StreamableAssets.at(cbIdx), lclSessionId, 0 );
}

//============================================================================
bool AppletPlayerStream::playMedia( AssetBaseInfo& assetInfo, int pos0to100000 )
{
	AppletPlayerBase::setAssetInfo( assetInfo );
    if( assetInfo.isStream() )
	{
		m_LclSessionId.initializeWithNewVxGUID();
		return playStream( assetInfo, m_LclSessionId, pos0to100000 );
	}

	return IMediaPlayerRequests::getNlcPlayer().fromGuiPlayMedia( assetInfo, pos0to100000 );
}

//============================================================================
bool AppletPlayerStream::playStream( AssetBaseInfo& assetInfo, VxGUID lclSessionId, int pos0to100000 )
{
	bool canPlay = GetVirtStreamMgr().fromGuiPlayStream( assetInfo, lclSessionId, pos0to100000 );
	if( canPlay )
	{
		canPlay &= IMediaPlayerRequests::getNlcPlayer().fromGuiPlayStream( assetInfo, lclSessionId, pos0to100000 );
	}
	
	return canPlay;
}

//========================================================================
void AppletPlayerStream::startMediaPlay( int startPos )
{
	bool playStarted = m_Engine.fromGuiAssetAction( eAssetActionPlayBegin, m_AssetInfo, startPos );
	updateGuiPlayControls( playStarted );
	if( false == playStarted )
	{
		m_MyApp.toGuiStatusMessage( "Video Play FAILED TO Begin" );
	}
}

//============================================================================
void AppletPlayerStream::slotPlayButtonClick( void )
{
	if( !ui.m_ReplayButton->isEnabled() )
	{
		return;
	}

	int cbIdx = ui.m_StreamsComboBox->currentIndex();
	if( cbIdx < 1 )
	{
		return;
	}

	cbIdx--;
	
	if( cbIdx >= m_StreamableAssets.size() )
	{
		LogMsg( LOG_ERROR, "%s invalid combo box index", __func__ );
		return;
	}

	VxGUID lclSessionId;
	lclSessionId.initializeWithNewVxGUID();
	m_MyApp.getPlayerMgr().playStream( m_StreamableAssets.at(cbIdx), lclSessionId, 0 );
}

//============================================================================
void AppletPlayerStream::onMediaPlayerNlcReady( bool isReady )
{
	if( isReady )
	{
		ui.m_StreamsComboBox->setEnabled( isReady );
		ui.m_ReplayButton->setEnabled( isReady );
	}
}

//============================================================================
void AppletPlayerStream::onPlaybackStopped( VxGUID& feedId )
{
	GetVirtStreamMgr().onPlaybackStopped( feedId );
	AppletPlayerNlcBase::onPlaybackStopped( feedId );
}

//============================================================================
void AppletPlayerStream::onPlaybackEnded( VxGUID& feedId )
{
	GetVirtStreamMgr().onPlaybackEnded( feedId );
	AppletPlayerNlcBase::onPlaybackEnded( feedId );
	getRenderConsumer()->showAppIcon();
}
