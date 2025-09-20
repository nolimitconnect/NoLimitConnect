#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
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
    class AppletHostLeaveUi;
}
QT_END_NAMESPACE

class AppletHostLeave : public AppletBase
{
	Q_OBJECT
public:
    AppletHostLeave( AppCommon& app, QWidget* parent );
	virtual ~AppletHostLeave() override;

    void                        setHostGroupieId( GroupieId groupieId );
    GroupieId                   getHostGroupieId( void )                            { return m_GroupieId; }
    EHostType                   getHostType( void )                                 { return m_HostType; }

signals:
    void						signalLeftHost( void );

protected slots:
    void                        slotLeaveButtonClicked( void );
    void                        slotBootButtonClicked( void );
    void                        slotBlockButtonClicked( void );
    void                        slotCancelButtonClicked( void );

protected:
    //=== vars ===//
    Ui::AppletHostLeaveUi&	    ui;
    GroupieId                   m_GroupieId;
    EHostType                   m_HostType{ eHostTypeUnknown };
};


