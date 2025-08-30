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
    class AppletEditStoryboardUi;
}
QT_END_NAMESPACE

class AppletEditStoryboard : public AppletBase
{
	Q_OBJECT
public:
    AppletEditStoryboard( AppCommon& app, QWidget* parent );
	virtual ~AppletEditStoryboard();

private slots:
    void						slotStoryBoardSavedModified( void );
    void						slotViewStoryboardButClick( void );

protected:
    //=== vars ===//
    Ui::AppletEditStoryboardUi&	ui;
    std::string					m_strSavedCwd;
    std::string					m_strStoryBoardDir;
    std::string					m_strStoryBoardFile;
};


