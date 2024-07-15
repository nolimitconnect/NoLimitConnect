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
    class AppletGalleryThumbUi;
}
QT_END_NAMESPACE

class ThumbnailViewWidget;
class ThumbMgr;

class AppletGalleryThumb : public AppletBase
{
	Q_OBJECT
public:
	AppletGalleryThumb( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletGalleryThumb() override;

signals:
    void                        signalThumbSelected( AppletBase * applet, ThumbnailViewWidget * thumb );

private slots:
	void						slotImageClicked( ThumbnailViewWidget * thumb );

protected:
    void                        resizeEvent( QResizeEvent* ev ) override;
    void                        showEvent( QShowEvent* ev ) override;

    void                        loadAssets( void );

	//=== vars ===//
	Ui::AppletGalleryThumbUi&	ui;
    ThumbMgr&					m_ThumbMgr;
    bool                        m_isShown = false;
    bool                        m_isLoaded = false;

};
