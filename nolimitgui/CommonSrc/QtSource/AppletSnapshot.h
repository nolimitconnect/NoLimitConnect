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

#include "AppletBase.h"
#include "ui_AppletSnapshot.h"

#include <CoreLib/MediaCallbackInterface.h>

class ThumbnailViewWidget;
class AssetMgr;
class IVxVidCap;

class AppletSnapshot : public AppletBase, public MediaCallbackInterface
{
	Q_OBJECT
public:
	AppletSnapshot( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletSnapshot() override;

signals:
    void						signalSnapshotImage( QImage snapshotImage );

public slots:
    void						onSnapShotButClick( void );
    void						onCancelButClick( void );
    void                        onDoneButClick( void );
    void                        onCamFrontBackButClick( void );
    void                        slotCameraDescription( QString camDescription );

protected:
    virtual void				callbackVideoJpgSmall( VxGUID& vidFeedId, uint8_t * jpgData, uint32_t jpgDataLen, int motion0to100000 ) override;
    virtual void                onCloseEvent( void ) override;


    Ui::AppletSnapshotUi		ui;
    bool 					    m_CameraSourceAvail = false;
    QTimer *                    m_CloseDlgTimer = nullptr;
    bool 					    m_SnapShotPending = false;
    QImage	                    m_ImageBitmap;
};
