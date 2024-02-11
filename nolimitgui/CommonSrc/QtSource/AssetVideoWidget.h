#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AssetBaseWidget.h"
#include "ui_AssetVideoWidget.h"

class AssetVideoWidget : public AssetBaseWidget
{
	Q_OBJECT

public:
	AssetVideoWidget( QWidget* parent = nullptr );
	AssetVideoWidget( AppCommon& appCommon, QWidget* parent = nullptr );
	virtual ~AssetVideoWidget() = default;

    virtual void				setAssetInfo( AssetBaseInfo& assetInfo ) override;
    virtual void				onActivityStop( void ) override;

    virtual void				showSendFail( bool show, bool permissionErr = false ) override;
    virtual void				showResendButton( bool show ) override;
    virtual void				showShredder( bool show ) override;
    virtual void				showXferProgress( bool show ) override;
    virtual void				setXferProgress( int sendProgress ) override;

signals:
	void						signalPlayProgress( int pos );
	void						signalPlayEnd( void );

protected slots:
	void						slotPlayButtonClicked( void );
    void						slotShredAsset( void ) override;
	void						slotSliderPressed( void );
	void						slotSliderReleased( void );

	void						slotPlayProgress( int pos0to100000 );
	void						slotPlayEnd( void );

	void						slotFeedRotationChanged( int feedRotation );
	void						slotCamRotationChanged( int camRotation );

	void						slotReadyForVideo( void );

protected:
	virtual void				toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos ) override;
	void						initAssetVideoWidget( void );

    void						resizeEvent( QResizeEvent* ev ) override;

	void						setReadyForCallbacks( bool isReady );
	void						updateGuiPlayControls( bool isPlaying );
	void						startMediaPlay( int startPos );
	void						stopMediaIfPlaying( void );
	void						onAssetWidgetVisibleAndReady( bool isVisible, bool isReady ) override;

	//=== vars ===//
	bool						m_ActivityCallbacksEnabled{ false };
	bool						m_IsPlaying{ false };
	bool						m_SliderIsPressed{ false };

	Ui::AssetVideoWidget		ui;
	QTimer*						m_ReadyForVideoTimer{ nullptr };
};
