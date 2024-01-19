#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPlayerBase.h"

#include "GuiPlayerCallback.h"

#include <GuiInterface/IMediaPlayerInterface.h>

#include <QElapsedTimer>

class RenderGlWidget;
class QMediaPlayer;
class QPushButton;
class QSlider;

class AppletPlayerNlcBase : public AppletPlayerBase, public GuiPlayerCallback, public IMediaPlayerCallback
{
	Q_OBJECT
public:
	AppletPlayerNlcBase( const char* ObjName, AppCommon& app, QWidget* parent );
	virtual ~AppletPlayerNlcBase();

	virtual EAppModule			getAppModule( void ) { return m_AppModule; }

	virtual RenderGlWidget*		getRenderConsumer( void ) = 0;
	virtual QSlider*			getPlayPosSlider( void ) = 0;
	virtual QPushButton*		getReplayButton( void ) = 0;

	virtual void				onMediaPlayerNlcReady( bool isReady ) {};

    void						toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;

    void						callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 ) override {};

	
	void						fromMediaPlayerInitLevel( int level, bool success ) override;

	void						fromMediaPlayerPlayFile( VxGUID& feedId ) override;
	void						fromMediaPlayerPlayStarted( VxGUID& feedId ) override;

	void						fromMediaPlayerStopPlaying( VxGUID& feedId ) override;
	void						fromMediaPlayerPlaybackStopped( VxGUID& feedId ) override;
	void						fromMediaPlayerPlaybackEnded( VxGUID& feedId ) override;

signals:
	void						signalInternalInitLevel( int level, bool success );

	void						signalInternalPlayFile( VxGUID feedId );
	void						signalInternalPlayStarted( VxGUID feedId );

	void						signalInternalStopPlaying( VxGUID feedId );
	void						signalInternalPlaybackStopped( VxGUID feedId );
	void						signalInternalPlaybackEnded( VxGUID feedId );

protected slots:
	void						slotInternalInitLevel( int level, bool success );

	void						slotInternalPlayFile( VxGUID feedId );
	void						slotInternalPlayStarted( VxGUID feedId );

	void						slotInternalStopPlaying( VxGUID feedId );
	void						slotInternalPlaybackStopped( VxGUID feedId );
	void						slotInternalPlaybackEnded( VxGUID feedId );

	void						slotPlayButtonClicked( void );
	void						slotSliderPressed( void );
	void						slotSliderReleased( void );

	void						slotPlayProgress( int pos0to100000 );
	void						slotPlayEnd( void );

	void                        slotReplayButtonClick( void );

protected:
	void						initAppletPlayerNlcBase( void );

	void						onAppletInitialized( void ); // must be called after derived class applet ui is initialized so can connect widgets to slots

	void						showEvent( QShowEvent* ev ) override;
	void						hideEvent( QHideEvent* ev ) override;
	void						resizeEvent( QResizeEvent* ev ) override;

	virtual bool				playMedia( AssetBaseInfo& assetInfo, int pos0to100000 ) override;
	bool						playMediaFile( std::string fileStr, int pos0to100000, bool isStream );

	void						setReadyForCallbacks( bool isReady );
	void						updateGuiPlayControls( bool isPlaying );

	void						startMediaPlay( int startPos );
	void						stopMediaIfPlaying( void );

	bool						waitForPlayerThread( void );

	virtual void				onStopPlaying( VxGUID& feedId ) {};
	virtual void				onPlaybackStopped( VxGUID& feedId ) {};
	virtual void				onPlaybackEnded( VxGUID& feedId ) {};

private:
	void						callbackGuiMediaPlayerNlcReady( bool isReady ) override;



	//=== vars ===//
	EAppModule					m_AppModule{ eAppModulePlayerNlc };
	bool						m_ActivityCallbacksEnabled{ false };
	bool						m_IsPlaying{ false };
	bool						m_SliderIsPressed{ false };

	QMediaPlayer*				m_QMediaPlayer{ nullptr };
	QElapsedTimer				m_ElapsedTimer;

	bool						m_LastPlayedIsStream{ false };
	bool						m_LastPlayedIsFile{ false };
	std::string					m_LastPlayedMedia;
};


