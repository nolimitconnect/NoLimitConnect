//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPlayerNlcBase.h"

#include "AppCommon.h"
#include "SoundMgr.h"

#include <AppInterface/INlc.h>

#include "MediaPlayerNlc.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>

namespace
{
	const int PROCESS_QT_DEFAULT_MS = 50;

	void ProcessQtEvents( int ms = PROCESS_QT_DEFAULT_MS )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
	}
}

//============================================================================
AppletPlayerNlcBase::AppletPlayerNlcBase( const char* ObjName, AppCommon& app, QWidget* parent )
: AppletPlayerBase( ObjName, app, parent )
{
	initAppletPlayerNlcBase();
}

//============================================================================
void AppletPlayerNlcBase::initAppletPlayerNlcBase( void )
{
	connect( this, SIGNAL(signalInternalInitLevel(int,bool)), this, SLOT(slotInternalInitLevel(int,bool)), Qt::QueuedConnection );

	connect( this, SIGNAL(signalInternalPlayFile(VxGUID)), this, SLOT(slotInternalPlayFile(VxGUID)), Qt::QueuedConnection );
	connect( this, SIGNAL(signalInternalPlayStarted(VxGUID)), this, SLOT(slotInternalPlayStarted(VxGUID)), Qt::QueuedConnection );

	connect( this, SIGNAL(signalInternalStopPlaying(VxGUID)), this, SLOT(slotInternalStopPlaying(VxGUID)), Qt::QueuedConnection );
	connect( this, SIGNAL(signalInternalPlaybackStopped(VxGUID)), this, SLOT(slotInternalPlaybackStopped(VxGUID)), Qt::QueuedConnection );
	connect( this, SIGNAL(signalInternalPlaybackEnded(VxGUID)), this, SLOT(slotInternalPlaybackEnded(VxGUID)), Qt::QueuedConnection );

	INlc::getINlc().getNlcPlayer().wantMediaPlayerCallback( this, true );
}

//============================================================================
AppletPlayerNlcBase::~AppletPlayerNlcBase()
{
	stopMediaIfPlaying();
	INlc::getINlc().getNlcPlayer().wantMediaPlayerCallback( this, false );

	m_MyApp.getPlayerMgr().wantPlayVideoCallbacks( this, false );
	m_MyApp.getSoundMgr().setPlayerNlcActive( false );
	m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletPlayerNlcBase::showEvent( QShowEvent* showEvent )
{
	AppletBase::showEvent( showEvent );
}

//============================================================================
void AppletPlayerNlcBase::hideEvent( QHideEvent* hideEvent )
{
	AppletBase::hideEvent( hideEvent );
}

//============================================================================
void AppletPlayerNlcBase::resizeEvent( QResizeEvent* ev )
{
	AppletBase::resizeEvent( ev );
}

//============================================================================
void AppletPlayerNlcBase::onAppletInitialized( void )
{
	getPlayPosSlider()->setRange(0, 100000);

	connect( getPlayPosSlider(), SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()) );
	connect( getPlayPosSlider(), SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()) );

	connect( getReplayButton(), SIGNAL(clicked()), this, SLOT(slotReplayButtonClick()) );

	connect( this, SIGNAL(signalPlayProgress(int)), this, SLOT(slotPlayProgress(int)) );
	connect( this, SIGNAL(signalPlayEnd()), this, SLOT(slotPlayEnd()) );

	m_MyApp.activityStateChange( this, true );
	m_MyApp.getSoundMgr().setPlayerNlcActive( true );
	m_MyApp.getPlayerMgr().wantPlayVideoCallbacks( this, true );
}

//============================================================================
void AppletPlayerNlcBase::setReadyForCallbacks( bool isReady )
{
	if( m_ActivityCallbacksEnabled != isReady )
	{
		m_ActivityCallbacksEnabled = isReady;
		m_MyApp.wantToGuiActivityCallbacks( this, isReady );
	}
}

//============================================================================
bool AppletPlayerNlcBase::playMedia( AssetBaseInfo& assetInfo, int pos0to100000 )
{
	if( !waitForPlayerThread() )
	{
		return false;
	}

	stopMediaIfPlaying();
	AppletPlayerBase::setAssetInfo( assetInfo );

	return INlc::getINlc().getNlcPlayer().fromGuiPlayMedia( assetInfo, pos0to100000 );
}

//============================================================================
bool AppletPlayerNlcBase::playMediaFile( std::string mediaFile, int pos0to100000, bool isStream )
{
	if( !waitForPlayerThread() )
	{
		return false;
	}

	if( VxFileUtil::fileExists( mediaFile.c_str() ) )
    {
		return AppletPlayerBase::playFile( mediaFile.c_str(), pos0to100000, isStream, false );
	}
    else
    {
		QString fileName( mediaFile.c_str() );
        QMessageBox::information( this, QObject::tr("File does not exist"), fileName, QMessageBox::Ok );
		return false;
    }
}

//============================================================================
bool AppletPlayerNlcBase::waitForPlayerThread( void )
{
	m_ElapsedTimer.start();
	while( !INlc::getINlc().getNlcPlayer().fromGuiIsModuleRunning( eAppModulePlayerNlc ) )
	{
		ProcessQtEvents( 100 );
		if( m_ElapsedTimer.elapsed() > 6000 )
		{
			LogMsg( LOG_ERROR, "Media Player Failed To Start" );
			return false;
		}
	}

	return true;
}

//============================================================================
void AppletPlayerNlcBase::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	AppletPlayerBase::toGuiClientAssetAction( assetAction, assetId, pos0to100000 );
	switch( assetAction )
	{
	case eAssetActionPlayProgress:
		if( false == m_SliderIsPressed )
		{
			updateGuiPlayControls( true );
			getPlayPosSlider()->setValue(pos0to100000);
		}

		break;

	case eAssetActionPlayEnd:
		if( false == m_SliderIsPressed )
		{
			updateGuiPlayControls( false );
		}

		break;

	default:
		break;
	}
}

//============================================================================
void AppletPlayerNlcBase::slotSliderPressed( void )
{
	m_SliderIsPressed = true;
}

//============================================================================
void AppletPlayerNlcBase::slotSliderReleased( void )
{
	m_SliderIsPressed = false;
	int posVal = getPlayPosSlider()->value();
	startMediaPlay( posVal );
}

//============================================================================
void AppletPlayerNlcBase::slotPlayButtonClicked( void )
{
	if( m_IsPlaying )
	{
		stopMediaIfPlaying();
	}
	else
	{
		startMediaPlay( 0 );
	}
}

//========================================================================
void AppletPlayerNlcBase::startMediaPlay( int startPos )
{
	bool playStarted = m_Engine.fromGuiAssetAction( eAssetActionPlayBegin, m_AssetInfo, startPos );
	updateGuiPlayControls( playStarted );
	if( false == playStarted )
	{
		m_MyApp.toGuiStatusMessage( "Video Play FAILED TO Begin" );
	}
}

//========================================================================
void AppletPlayerNlcBase::updateGuiPlayControls( bool isPlaying )
{
	if( m_IsPlaying != isPlaying )
	{
		m_IsPlaying = isPlaying;
		if( m_IsPlaying )
		{
			// start playing
			//getPlayPauseButton()->setIcons(eMyIconPauseNormal);
			setReadyForCallbacks( true );
		}
		else
		{
			// stop playing
			//getPlayPauseButton()->setIcons( eMyIconPlayNormal );
			getPlayPosSlider()->setValue(0);
		}
	}
}

//============================================================================
void AppletPlayerNlcBase::stopMediaIfPlaying( void )
{
	if( m_IsPlaying )
	{
		m_MyApp.toGuiStatusMessage( "" );
		m_Engine.fromGuiAssetAction( eAssetActionPlayEnd, m_AssetInfo, 0 );
		startBusySpinner();
	}

	updateGuiPlayControls( false );
}

//============================================================================
void AppletPlayerNlcBase::slotPlayProgress( int pos0to100000 )
{
	if( m_IsPlaying && (false == m_SliderIsPressed) )
	{
		getPlayPosSlider()->setValue(pos0to100000);
	}
}

//============================================================================
void AppletPlayerNlcBase::slotPlayEnd( void )
{
	updateGuiPlayControls( false );
}

//============================================================================
void AppletPlayerNlcBase::slotReplayButtonClick( void )
{
	if( m_LastPlayedIsFile )
	{
		if( VxFileUtil::fileExists( m_LastPlayedMedia.c_str() ) )
		{
			playFile( m_LastPlayedMedia.c_str(), 0, false, false );
		}
		else
		{
			QMessageBox::information( this, QObject::tr( "File does not exist" ), QString(m_LastPlayedMedia.c_str()), QMessageBox::Ok );
		}
	}
}

//============================================================================
void AppletPlayerNlcBase::callbackGuiMediaPlayerNlcReady( bool isReady )
{
	LogMsg( LOG_DEBUG, "%s %d", __func__, isReady );
	onMediaPlayerNlcReady( isReady );
}

//============================================================================
void AppletPlayerNlcBase::fromMediaPlayerInitLevel( int level, bool success )
{
	emit signalInternalInitLevel( level, success );
}

//============================================================================
void AppletPlayerNlcBase::slotInternalInitLevel( int level, bool success )
{
	LogMsg( LOG_VERBOSE, "AppletPlayerNlcBase::slotInternalInitLevel level %d sucess ? %d", level, success );
}

//============================================================================
void AppletPlayerNlcBase::fromMediaPlayerPlayFile( VxGUID& feedId )
{
	emit signalInternalPlayFile( feedId );
}

//============================================================================
void AppletPlayerNlcBase::slotInternalPlayFile( VxGUID feedId )
{
	LogMsg( LOG_VERBOSE, "AppletPlayerNlcBase::%s", __func__ );
}

//============================================================================
void AppletPlayerNlcBase::fromMediaPlayerPlayStarted( VxGUID& feedId )
{
	emit signalInternalPlayStarted( feedId );
}

//============================================================================
void AppletPlayerNlcBase::slotInternalPlayStarted( VxGUID feedId )
{
	LogMsg( LOG_VERBOSE, "AppletPlayerNlcBase::%s", __func__ );
	onPlayStarted( feedId );
}

//============================================================================
void AppletPlayerNlcBase::fromMediaPlayerStopPlaying( VxGUID& feedId )
{
	emit signalInternalStopPlaying( feedId );
}

//============================================================================
void AppletPlayerNlcBase::slotInternalStopPlaying( VxGUID feedId )
{
	LogMsg( LOG_VERBOSE, "AppletPlayerNlcBase::%s", __func__ );
	onStopPlaying( feedId );
}

//============================================================================
void AppletPlayerNlcBase::fromMediaPlayerPlaybackStopped( VxGUID& feedId )
{
	emit signalInternalPlaybackStopped( feedId );
}

//============================================================================
void AppletPlayerNlcBase::slotInternalPlaybackStopped( VxGUID feedId )
{
	LogMsg( LOG_VERBOSE, "AppletPlayerNlcBase::%s", __func__ );
	onPlaybackStopped( feedId );
}

//============================================================================
void AppletPlayerNlcBase::fromMediaPlayerPlaybackEnded( VxGUID& feedId )
{
	emit signalInternalPlaybackEnded( feedId );
}

//============================================================================
void AppletPlayerNlcBase::slotInternalPlaybackEnded( VxGUID feedId )
{
	LogMsg( LOG_VERBOSE, "AppletPlayerNlcBase::%s", __func__ );
	onPlaybackEnded( feedId );
}

//============================================================================
void AppletPlayerNlcBase::onBackButtonClicked( void )
{
	stopMediaIfPlaying();
	AppletPlayerBase::onBackButtonClicked();
}

//============================================================================
void AppletPlayerNlcBase::onPlayStarted( VxGUID& feedId )
{
	updateGuiPlayControls( true );
}

//============================================================================
void AppletPlayerNlcBase::onStopPlaying( VxGUID& feedId )
{
	stopBusySpinner();
	updateGuiPlayControls( false );
}

//============================================================================
void AppletPlayerNlcBase::onPlaybackStopped( VxGUID& feedId )
{
	stopBusySpinner();
	updateGuiPlayControls( false );
}

//============================================================================
void AppletPlayerNlcBase::onPlaybackEnded( VxGUID& feedId )
{
	stopBusySpinner();
	updateGuiPlayControls( false );
}
