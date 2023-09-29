#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPlayerBase.h"

#include "GuiPlayerCallback.h"
#include "ui_AppletPlayerNlc.h"

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

	void						setReadyForCallbacks( bool isReady );
	void						updateGuiPlayControls( bool isPlaying );

	void						startMediaPlay( int startPos );
	void						stopMediaIfPlaying( void );

	void                        setupBottomMenu( VxMenuButton* menuButton );

	void						showEvent( QShowEvent* ev ) override;
	void						hideEvent( QHideEvent* ev ) override;
	void						resizeEvent( QResizeEvent* ev ) override;

	void						onFileSelected( FileInfo& fileInfo );

	//=== vars ===//
	Ui::AppletPlayerNlcUi		ui;
	EAppModule					m_AppModule{ eAppModulePlayerNlc };
	bool						m_ActivityCallbacksEnabled{ false };
	bool						m_IsPlaying{ false };
	bool						m_SliderIsPressed{ false };
};


