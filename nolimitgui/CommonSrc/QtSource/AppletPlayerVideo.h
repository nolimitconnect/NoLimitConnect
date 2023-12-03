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
#include "ui_AppletPlayerVideo.h"

class AppletPlayerVideo : public AppletPlayerBase, public GuiPlayerCallback
{
	Q_OBJECT
public:
	AppletPlayerVideo( AppCommon& app, QWidget* parent );
	virtual ~AppletPlayerVideo() = default;

	virtual void				setAssetInfo( AssetInfo& assetInfo ) override;

    virtual void				callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 ) override;
	virtual void				callbackGuiPlayVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame ) override;

	virtual void				showShredder( bool show );
	virtual void				showXferProgress( bool show );
	virtual void				setXferProgress( int sendProgress );

	virtual void				toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;

protected slots:
	void						slotPlayButtonClicked( void );
	void						slotShredAsset( void );
	void						slotSliderPressed( void );
	void						slotSliderReleased( void );

	void						slotPlayProgress( int pos0to100000 );
	void						slotPlayEnd( void );
    void                        slotMenuItemSelected( int menuId, EMenuItemType menuItemType );

protected:
	void						initAppletPlayerVideo( void );
	void						onAppletStop( void );

    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;
    void						resizeEvent( QResizeEvent* ev ) override;

	void						setReadyForCallbacks( bool isReady );
	void						updateGuiPlayControls( bool isPlaying );
	void						startMediaPlay( int startPos );
	void						stopMediaIfPlaying( void );
    void                        setupBottomMenu( VxMenuButton * menuButton );

	//=== vars ===//
	bool						m_ActivityCallbacksEnabled;
	bool						m_IsPlaying;
	bool						m_SliderIsPressed;

	Ui::AppletVideoPlayerClass	ui;
};


