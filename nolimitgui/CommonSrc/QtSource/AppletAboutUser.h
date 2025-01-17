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
    class AppletAboutUserUi;
}
QT_END_NAMESPACE

class QPlainTextEdit;

class AppletAboutUser : public AppletBase
{
	Q_OBJECT
public:
	AppletAboutUser( AppCommon& app, QWidget* parent );
	virtual ~AppletAboutUser();

    void                        setUser( GuiUser* guiUser ) override;

protected:
    void                        fillUserDetails( GuiUser* guiUser );
    QPlainTextEdit *            getInfoEdit( void );
    void                        infoMsg( const QString& text );
    void                        infoMsg( std::string text );
    void                        infoMsg( const char* msg, ... );

    //=== vars ===//
    Ui::AppletAboutUserUi&	    ui;

    GuiUser*                    m_GuiUser{ nullptr };
};


