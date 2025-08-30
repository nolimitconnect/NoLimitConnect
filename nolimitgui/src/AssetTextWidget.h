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

QT_BEGIN_NAMESPACE
namespace Ui {
    class AssetTextWidget;
}
QT_END_NAMESPACE

class AssetTextWidget : public AssetBaseWidget
{
	Q_OBJECT

public:
	AssetTextWidget( QWidget* parent = nullptr );
	AssetTextWidget( AppCommon& appCommon, QWidget* parent = nullptr );

    virtual void				showSendFail( bool show, bool permissionErr = false ) override;
    virtual void				showResendButton( bool show ) override;
    virtual void				showShredder( bool show ) override;
    virtual void				showXferProgress( bool show ) override;
    virtual void				setXferProgress( int sendProgress ) override;

protected slots:
	void						slotAssetWasClicked( void );
    void						slotShredAsset( void ) override;
	void						slotCopyTextToClipboardButClick( void );

protected:
	void						initAssetTextWidget( void );
    void						setAssetInfo( AssetBaseInfo& assetInfo ) override;
	void 						calculateHint( void );

	//=== vars ===//
	Ui::AssetTextWidget&		ui;

};
