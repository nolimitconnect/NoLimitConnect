#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletGalleryImageUi;
}
QT_END_NAMESPACE

class ImageListEntry;
class AssetMgr;
class ThumbnailViewWidget;

class AppletGalleryImage : public AppletBase
{
	Q_OBJECT
public:
	AppletGalleryImage( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletGalleryImage() override = default;

signals:
    void                        signalImageSelected( ThumbnailViewWidget * imageEntry );

private slots:
	void						slotImageClicked( ThumbnailViewWidget * imageEntry );

protected:

	//=== vars ===//
	Ui::AppletGalleryImageUi&	ui;
    AssetMgr&					m_AssetMgr;
};
