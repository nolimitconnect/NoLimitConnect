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
    class AssetPhotoWidget;
}
QT_END_NAMESPACE

class AssetPhotoWidget : public AssetBaseWidget
{
	Q_OBJECT

public:
	AssetPhotoWidget( QWidget* parent = nullptr );
	AssetPhotoWidget( AppCommon& appCommon, QWidget* parent = nullptr );

    virtual void				setAssetInfo( AssetBaseInfo& assetInfo ) override;
    virtual void				showSendFail( bool show, bool permissionErr = false ) override;
    virtual void				showResendButton( bool show ) override;
    virtual void				showShredder( bool show ) override;
    virtual void				showLibraryButton( bool show ) override;
    virtual void				showXferProgress( bool show ) override;
    virtual void				setXferProgress( int sendProgress ) override;

protected slots:
	void						slotAssetWasClicked( void );
    void						slotFileNameClicked( void );

protected:
	void						initAssetPhotoWidget( void );
	//=== vars ===//

	Ui::AssetPhotoWidget&		ui;
};
