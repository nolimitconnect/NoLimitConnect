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
#include "GuiPlayerMgr.h"
#include "SoundMgr.h"

#include <AppInterface/INlc.h>

#include <P2PEngine/P2PEngine.h>
#include "MediaPlayerNlc.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#define ENABLE_NLC_PLAYER_STREAMS 1

namespace
{
	const int PROCESS_QT_DEFAULT_MS = 50;

	void ProcessQtEvents( int ms = PROCESS_QT_DEFAULT_MS )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
	}
}

//============================================================================
AppletPlayerStream::AppletPlayerStream( AppCommon& app, QWidget* parent )
: AppletPlayerNlcBase( OBJNAME_APPLET_PLAYER_STREAM, app, parent )
{
	initAppletPlayerStream();
}

//============================================================================
void AppletPlayerStream::initAppletPlayerStream( void )
{
	setAppletType( eAppletPlayerStream );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

	ui.setupUi( getContentItemsFrame() );
	ui.m_PlayPosSlider->setVisible( false );
    setMenuBottomVisibility( true );

    BottomBarWidget * bottomBar = getBottomBarWidget();
    if( bottomBar )
    {
        setupBottomMenu( bottomBar->getMenuButton() );
    }

#if defined(DEBUG)

	ui.m_StreamsComboBox->setEnabled( false ); // do not enable until media player is ready
	ui.m_StreamsComboBox->addItem( "Debug Streams" );

	std::vector<std::string> mediaTestStreams;

	mediaTestStreams.push_back( "https://127.0.0.1/MJPEGWithAAC.avi" );
	mediaTestStreams.push_back( "https://127.0.0.1/NlcTestAudio.wav" );
	mediaTestStreams.push_back( "https://127.0.0.1/NlcTestAudioVbrOn.opus" );
	mediaTestStreams.push_back( "https://127.0.0.1/Theres-the-roses-you-bought-meThanks.jpg" );
	
	for( auto& mediaStream : mediaTestStreams )
	{
		ui.m_StreamsComboBox->addItem( mediaStream.c_str() );
	}

#else
    ui.m_StreamsComboBox->setVisible( false );
    ui.m_BrowseButton->setVisible( false );
#endif // defined(DEBUG)

	connect( ui.m_StreamsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMediaStreamComboBoxSelectionChange(int)) );

	connect( ui.m_BrowseButton, SIGNAL( clicked() ), this, SLOT( slotBrowseButtonClick() ) );

	onAppletInitialized();
}

//============================================================================
AppletPlayerStream::~AppletPlayerStream()
{
	stopMediaIfPlaying();
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
void AppletPlayerStream::setReadyForCallbacks( bool isReady )
{
	if( m_ActivityCallbacksEnabled != isReady )
	{
		m_ActivityCallbacksEnabled = isReady;
		m_MyApp.wantToGuiActivityCallbacks( this, isReady );
	}
}

//============================================================================
void AppletPlayerStream::slotMediaStreamComboBoxSelectionChange( int cbIdx )
{
    std::string mediaStream = ui.m_StreamsComboBox->currentText().toUtf8().constData();
	playStream( mediaStream, 0 );
}

//============================================================================
bool AppletPlayerStream::playMedia( AssetBaseInfo& assetInfo, int pos0to100000 )
{
	AppletPlayerBase::setAssetInfo( assetInfo );
#if ENABLE_NLC_PLAYER_STREAMS
	return INlc::getINlc().getNlcPlayer().fromGuiPlayMedia( assetInfo, pos0to100000 );
#endif // ENABLE_NLC_PLAYER_STREAMS
	return true;
}

//============================================================================
bool AppletPlayerStream::playStream( std::string mediaStream, int pos0to100000 )
{
	return INlc::getINlc().getNlcPlayer().fromGuiPlayStream( mediaStream, pos0to100000 );
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

//========================================================================
void AppletPlayerStream::updateGuiPlayControls( bool isPlaying )
{
	if( m_IsPlaying != isPlaying )
	{
		m_IsPlaying = isPlaying;
		if( m_IsPlaying )
		{
			// start playing
			//ui.m_PlayPauseButton->setIcons( eMyIconPauseNormal );
			setReadyForCallbacks( true );
		}
		else
		{
			// stop playing
			//ui.m_PlayPauseButton->setIcons( eMyIconPlayNormal );
			ui.m_PlayPosSlider->setValue( 0 );
		}
	}
}

//============================================================================
void AppletPlayerStream::stopMediaIfPlaying( void )
{
	if( m_IsPlaying )
	{
		m_MyApp.toGuiStatusMessage( "" );
		m_Engine.fromGuiAssetAction( eAssetActionPlayEnd, m_AssetInfo, 0 );
	}

	updateGuiPlayControls( false );
}


//============================================================================
void AppletPlayerStream::slotBrowseButtonClick( void )
{
    //ActivityBrowseStreams dlg( m_MyApp, eStreamFilterVideo, this, true );

    //dlg.exec();
    //if( dlg.getWasStreamSelected() )
    //{
    //    onStreamSelected( dlg.getSelectedStreamInfo() );
    //}
}

//============================================================================
void AppletPlayerStream::onMediaPlayerNlcReady( bool isReady )
{
	ui.m_StreamsComboBox->setEnabled( true );
}