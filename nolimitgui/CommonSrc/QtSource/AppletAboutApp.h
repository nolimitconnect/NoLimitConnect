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

#include "AppletBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletAboutAppUi;
}
QT_END_NAMESPACE

class AppletAboutApp : public AppletBase
{
	Q_OBJECT
public:
    AppletAboutApp( AppCommon& app, QWidget* parent );
	virtual ~AppletAboutApp();

protected slots:
    void						gotoWebsite( void );
    void                        slotShowAppInfo( void );

protected:
    void						setupAboutMe( void );

    Ui::AppletAboutAppUi&        ui;
};


