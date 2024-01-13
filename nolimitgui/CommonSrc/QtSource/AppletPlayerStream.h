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
	QSlider*					getPlayPosSlider( void ) override		{ return ui.m_PlayPosSlider; }
	QPushButton*				getReplayButton( void ) override		{ return ui.m_ReplayButton; }

	void						onMediaPlayerNlcReady( bool isReady ) override;

protected slots:
	void                        slotMenuItemSelected( int menuId, EMenuItemType menuItemType );

	void                        slotMediaStreamComboBoxSelectionChange( int cbIdx );

	void						slotBrowseButtonClick( void );

protected:
	void						initAppletPlayerStream( void );

	virtual bool				playMedia( AssetBaseInfo& assetInfo, int pos0to100000 = 0 ) override;
	bool						playStream( std::string fileStr, int pos0to100000 = 0 );

	void						setReadyForCallbacks( bool isReady );
	void						updateGuiPlayControls( bool isPlaying );

	void						startMediaPlay( int startPos );
	void						stopMediaIfPlaying( void );

	void                        setupBottomMenu( VxMenuButton* menuButton );


	//=== vars ===//
	Ui::AppletPlayerStreamUi	ui;
	EAppModule					m_AppModule{ eAppModulePlayerNlc };
	bool						m_ActivityCallbacksEnabled{ false };
	bool						m_IsPlaying{ false };
	bool						m_SliderIsPressed{ false };

	QMediaPlayer*				m_QMediaPlayer{ nullptr };
	QElapsedTimer				m_ElapsedTimer;
};


