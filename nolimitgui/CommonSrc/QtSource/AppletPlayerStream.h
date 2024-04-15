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

#include "AppletPlayerNlcBase.h"

#include "GuiPlayerCallback.h"
#include "ui_AppletPlayerStream.h"

#include <QElapsedTimer>

class QMediaPlayer;

class AppletPlayerStream : public AppletPlayerNlcBase
{
	Q_OBJECT
public:
	AppletPlayerStream( AppCommon& app, QWidget* parent );
	virtual ~AppletPlayerStream();

	RenderGlWidget*				getRenderConsumer( void ) override		{ return ui.m_RenderWidget; }
	QSlider*					getPlayPosSlider( void ) override		{ return ui.m_PlayControlWidget->getPlayPosSlider(); }
	QPushButton*				getReplayButton( void ) override		{ return ui.m_ReplayButton; }
	VxPushButton*				getPlayPauseButton( void ) override		{ return ui.m_PlayControlWidget->getPlayPauseButton(); }
	PlayControlWidget*			getPlayControlWidget( void ) override	{ return ui.m_PlayControlWidget; }

	void						onMediaPlayerNlcReady( bool isReady ) override;

	bool						playStream( AssetBaseInfo& assetInfo, VxGUID lclSessionId, int pos0to100000 );

protected slots:
	void                        slotMenuItemSelected( int menuId, EMenuItemType menuItemType );

	void                        slotMediaStreamComboBoxSelectionChange( int cbIdx );

	void						slotPlayButtonClick( void );

	void						slotAppletClosing( void );

protected:
	void						initAppletPlayerStream( void );

	virtual bool				playMedia( AssetBaseInfo& assetInfo, int pos0to100000 = 0 ) override;

	void						startMediaPlay( int startPos );

	void                        setupBottomMenu( VxMenuButton* menuButton );

	void						onPlaybackStopped( VxGUID& feedId ) override;
	void						onPlaybackEnded( VxGUID& feedId ) override;


	//=== vars ===//
	Ui::AppletPlayerStreamUi	ui;
	EAppModule					m_AppModule{ eAppModulePlayerNlc };
	bool						m_ActivityCallbacksEnabled{ false };
	bool						m_IsPlaying{ false };
	bool						m_SliderIsPressed{ false };

	QElapsedTimer				m_ElapsedTimer;

	std::vector<AssetBaseInfo>	m_StreamableAssets;
	VxGUID						m_LclSessionId;
};


