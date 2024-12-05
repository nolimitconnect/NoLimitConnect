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

#include "AppletClientBase.h"

#include <CoreLib/MediaCallbackInterface.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletCamClientUi;
}
QT_END_NAMESPACE

class AppletCamClient : public AppletBase, public MediaCallbackInterface
{
	Q_OBJECT
public:
	AppletCamClient( AppCommon& app, QWidget* parent );
	virtual ~AppletCamClient();

    void						setupCamFeed( GuiUser* feedNetIdent );

    void						webCamSourceOffline();
    void                        startCamFeed( void );
    void                        stopCamFeed( void );
    void                        setIsCamFeedStarted( bool isStarted ) { m_CamFeedStarted = isStarted; }
    bool                        isCamFeedStarted( void ) { return m_CamFeedStarted; }
    bool                        isMyself( void ) { return m_IsMyself; }

signals:
    void						signalPlayAudio( unsigned short* pu16PcmData, unsigned short u16PcmDataLen );
    void						signalSnapshotImage( QImage snapshotImage );

protected slots:
    void						slotToGuiRxedOfferReply( GuiOfferSession* offerSession );
    void						slotToGuiSessionEnded( GuiOfferSession* offerSession );
    void						slotToGuiContactOffline( VxNetIdent* hisIdent );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;
    virtual void				closeEvent( QCloseEvent* ev ) override;

    void						resizeBitmapToFitScreen( QLabel* VideoScreen, QImage& oPicBitmap );

    void                        toGuiContactOffline( GuiUser* guiUser ) override;

    //=== vars ===//
    Ui::AppletCamClientUi&      ui;
    bool						m_IsMyself{ false };
    bool 					    m_CameraSourceAvail{ false };
    QTimer*                     m_CloseAppletTimer{ nullptr };
    QImage	                    m_ImageBitmap;
    GuiUser*                    m_CamFeedIdent{ nullptr };
    VxGUID                      m_CamFeedId;
    bool                        m_CamFeedStarted{ false };
};


