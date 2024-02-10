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
#include "ui_AssetVoiceWidget.h"

#include <vector>

class QTimer;

class AssetVoiceWidget : public AssetBaseWidget
{
	Q_OBJECT

public:
	AssetVoiceWidget( QWidget* parent = nullptr );
	AssetVoiceWidget( AppCommon& appCommon, QWidget* parent = nullptr );

    virtual void				setAssetInfo( AssetBaseInfo& assetInfo ) override;
    virtual void				onActivityStop( void ) override;

    virtual void				showSendFail( bool show, bool permissionErr = false ) override;
    virtual void				showResendButton( bool show ) override;
    virtual void				showShredder( bool show ) override;
    virtual void				showXferProgress( bool show ) override;
    virtual void				setXferProgress( int sendProgress ) override;

protected slots:
	void						slotPlayButtonClicked( void );
	void						slotSliderPressed( void );
	void						slotSliderReleased( void );
	void						slotUpdatePlayerControls( void );

protected:
	virtual void				toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos ) override;
	void						initAssetVoiceWidget( void );

	void						setReadyForCallbacks( bool isReady );
	void						updateGuiPlayControls( bool isPlaying );
	void						startMediaPlay( int startPos );
	void						stopMediaIfPlaying( void );

	void						queueUpdatePlayerControls( bool enable );

	//=== vars ===//
	bool						m_ActivityCallbacksEnabled{ false };
	bool						m_IsPlaying{ false };
	bool						m_SliderIsPressed{ false };

	std::vector<bool>			m_QueuedPlayerControlUpdate;
	QTimer*						m_QueueUpdateTimer{ nullptr };

	Ui::AssetVoiceWidget		ui;

};
