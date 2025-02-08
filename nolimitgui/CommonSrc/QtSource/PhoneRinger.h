#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget>

#include "GuiOfferSession.h"

class AppCommon;
class GuiOfferMgrBase;
class QTimer;

class PhoneRinger : public QObject
{
    Q_OBJECT
public:

    explicit PhoneRinger( AppCommon& myApp, GuiOfferMgrBase& offerMgr, QObject *parent = nullptr );

    void                        startRinging( std::shared_ptr<GuiOfferSession>& offerSession );
    void                        stopRinging( std::shared_ptr<GuiOfferSession>& offerSession );

signals:
    void                        signalRingerTimeout( void );

protected slots:    
	void						slotOncePerSecondRingTimer( void );

protected:
    bool                        doesSessionExist( VxGUID& offerId );
    bool                        removeSession( VxGUID& offerId );
    bool                        checkForRingTimeout( void );
    
	//=== vars ===//


    AppCommon&                  m_MyApp;
    GuiOfferMgrBase&            m_OfferMgr;
    QTimer*                     m_RingTimer{nullptr};
    std::vector<std::shared_ptr<GuiOfferSession>> m_OfferSessions;
    int							m_RingTimerCycleCnt{ 0 };
	int							m_RingTimerSecondCnt{ 0 };
};


