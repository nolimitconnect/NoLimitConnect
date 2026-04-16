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
    class AppletHostJoinChooseUi;
}
QT_END_NAMESPACE

class AppletHostJoinChoose : public AppletBase
{
	Q_OBJECT
public:
    AppletHostJoinChoose( AppCommon& app, QWidget* parent );
	virtual ~AppletHostJoinChoose() override;

    void                        setHostType( EHostType hostType );
    EHostType                   getHostType( void )                                 { return m_HostType; }

protected slots:
    void                        slotViewCurrentButtonClicked( void );
    void                        slotRejoinButtonClicked( void );
    void                        slotSearchButtonClicked( void );
    void                        slotLeaveButtonClicked( void );

protected:
    //=== vars ===//
    Ui::AppletHostJoinChooseUi&	ui;
    EHostType                   m_HostType{ eHostTypeUnknown };
};


