#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"
#include "GuiPlayerCallback.h"

#include <GuiInterface/IDefs.h>
#include <CoreLib/VxDefs.h>

#include <QString>
#include <QDialog>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ScanWebCamsDialog;
}
QT_END_NAMESPACE

class VxNetIdent;
class VxGUID;

class ActivityScanWebCams : public ActivityBase, public GuiPlayerCallback
{
	Q_OBJECT
public:
	ActivityScanWebCams(	AppCommon& app, 
							QWidget*		parent = nullptr );
	virtual ~ActivityScanWebCams() override;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget*     getTitleBarWidget( void ) override;
    virtual BottomBarWidget*    getBottomBarWidget( void ) override;

public:
	void						setTitle( QString strTitle );

    virtual void				toGuiClientScanSearchComplete( EScanType eScanType ) override;
    virtual void				toGuiScanResultSuccess( EScanType eScanType, GuiUser* guiUser ) override;
    virtual void				callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame,int motion0To100000 ) override;
	//! handle audio
	void						playAudio( uint16_t * pu16PcmData, uint16_t u16PcmDataLen, VxGUID& onlineId );

signals:
	void						signalNewWebCamSession( GuiUser* guiUser );
	void						signalPlayAudio( unsigned short * pu16PcmData, unsigned short u16PcmDataLen );

public slots:
	void						slotNewWebCamSession( GuiUser* guiUser );

    void						slotHomeButtonClicked( void ) override;

	void						slotStartScanClicked();
	void						slotPauseScanClicked();
	void						slotNextScanClicked( void );

	void						onCountdownTimer( void );
	void						slotIdentMenuClicked( void );

protected:
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

	void						playVideoFrameRotated( QImage oPicBitmap, int iRotate );

	void						setScanStatusText( QString strMsg );
	void						setupIdentWidget( GuiUser* guiUser );

	void						startCountdown();
	void						updateCountdownGui();
	void						startStopScan( bool startScan );
	void						showNextCam( void ); 
	void						doCamConnect( GuiUser* guiUser );
	void						updateAvailableGui( void );
	void						startWebCamSession( VxGUID& onlineId, bool startSession );
	void						setCamViewToOfflineImage( void );


	//=== vars ===//
	Ui::ScanWebCamsDialog&		ui;
    GuiUser*				    m_HisIdent;
	int							m_iCountdownCnt;
	bool						m_bPaused;
	QTimer *					m_CountdownTimer;
	EScanType					m_eScanType;
	bool						m_bIconConnected;
	bool						m_bSearchComplete;
	QVector<GuiUser*>		    m_ScanList;
	bool						m_ShowNextCam;
	bool						m_IsScanning;
	VxGUID						m_LclSessionId;
};

