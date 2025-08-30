#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletAssetPlayerBase.h"

#include <CoreLib/MediaCallbackInterface.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletCamClipPlayerUi;
}
QT_END_NAMESPACE

class AssetMgr;
class AssetPlaySession;
class IVxVidCap;
class ThumbnailViewWidget;

class AppletPlayerCamClip : public AppletAssetPlayerBase, public MediaCallbackInterface
{
	Q_OBJECT
public:
	AppletPlayerCamClip( AppCommon& app, QWidget* parent = nullptr, VxGUID assetId = VxGUID::nullVxGUID() );
	virtual ~AppletPlayerCamClip() override;

	// override playMedia if is applet that plays media
	virtual bool				playMedia( AssetPlaySession& assetPlaySession, bool useExternalPlayer ) override;

	virtual void				setAssetInfo( AssetBaseInfo& assetInfo ) override;
	virtual void				setAssetInfo( AssetInfo& assetInfo ) override;
	virtual void				updateAssetInfo( void );
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

protected:
	void						toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;
	void						initAppletCamClipPlayer( void );

    void						showEvent( QShowEvent* ev ) override;
    void						resizeEvent( QResizeEvent* ev ) override;

	void						setReadyForCallbacks( bool isReady );
	void						updateGuiPlayControls( bool isPlaying );
	bool						startMediaPlay( int startPos );
	void						stopMediaIfPlaying( void );

	//=== vars ===//
	bool						m_ActivityCallbacksEnabled{ false };
	bool						m_IsPlaying{ false };
	bool						m_SliderIsPressed{ false };

    //=== vars ===//
    Ui::AppletCamClipPlayerUi&	ui;
};
