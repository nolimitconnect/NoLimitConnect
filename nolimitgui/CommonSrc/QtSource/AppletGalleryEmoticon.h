#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
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
    class AppletGalleryEmoticonUi;
}
QT_END_NAMESPACE

class ThumbnailViewWidget;
class ThumbMgr;

class AppletGalleryEmoticon : public AppletBase
{
	Q_OBJECT
public:
	AppletGalleryEmoticon( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletGalleryEmoticon() override;

signals:
    void                        signalThumbSelected( AppletBase * applet, ThumbnailViewWidget * thumb );

private slots:
	void						slotImageClicked( ThumbnailViewWidget * thumb );

protected:
    void                        resizeEvent( QResizeEvent* ev ) override;
    void                        showEvent( QShowEvent* ev ) override;

    void                        loadAssets( void );

	//=== vars ===//
	Ui::AppletGalleryEmoticonUi&	ui;
    
    bool                        m_isShown = false;
    bool                        m_isLoaded = false;

};
