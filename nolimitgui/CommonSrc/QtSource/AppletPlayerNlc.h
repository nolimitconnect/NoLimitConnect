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
#include "ui_AppletPlayerNlc.h"

#include <QElapsedTimer>

class QMediaPlayer;

class AppletPlayerNlc : public AppletPlayerBase, public GuiPlayerCallback
{
	Q_OBJECT
public:
	AppletPlayerNlc( AppCommon& app, QWidget* parent );
	virtual ~AppletPlayerNlc();

	EAppModule					getAppModule( void ) { return m_AppModule; }

	RenderGlWidget*				getRenderConsumer( void );

    void						toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;

    void						callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 ) override {};
	void						callbackGuiMediaPlayerNlcReady( bool isReady ) override;

protected slots:
	void						slotPlayButtonClicked( void );
	void						slotSliderPressed( void );
	void						slotSliderReleased( void );

	void						slotPlayProgress( int pos0to100000 );
	void						slotPlayEnd( void );

	void                        slotMenuItemSelected( int menuId, EMenuItemType menuItemType );

	void                        slotMediaFileComboBoxSelectionChange( int cbIdx );

	void                        slotReplayButtonClick( void );
	void						slotBrowseButtonClick( void );

protected:
	void						initAppletPlayerNlc( void );

	virtual bool				playMedia( AssetBaseInfo& assetInfo, int pos0to100000 = 0 ) override;
	bool						playMediaFile( std::string fileStr, int pos0to100000 = 0 );

	void						setReadyForCallbacks( bool isReady );
	void						updateGuiPlayControls( bool isPlaying );

	void						startMediaPlay( int startPos );
	void						stopMediaIfPlaying( void );

	void                        setupBottomMenu( VxMenuButton* menuButton );

	void						showEvent( QShowEvent* ev ) override;
	void						hideEvent( QHideEvent* ev ) override;
	void						resizeEvent( QResizeEvent* ev ) override;

	void						onFileSelected( FileInfo& fileInfo );

	bool						waitForPlayerThread( void );

	//=== vars ===//
	Ui::AppletPlayerNlcUi		ui;
	EAppModule					m_AppModule{ eAppModulePlayerNlc };
	bool						m_ActivityCallbacksEnabled{ false };
	bool						m_IsPlaying{ false };
	bool						m_SliderIsPressed{ false };
	QMediaPlayer*				m_QMediaPlayer{ nullptr };
	QElapsedTimer				m_ElapsedTimer;
};


