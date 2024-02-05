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

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include "GuiUserBase.h"

class GuiUserMgr;

class GuiUser : public GuiUserBase
{
    Q_OBJECT
public:
    GuiUser() = delete;
    GuiUser( AppCommon& app );
    GuiUser( AppCommon& app, VxNetIdent* netIdent, VxGUID& sessionId );
    GuiUser( const GuiUser& rhs );
	virtual ~GuiUser() override = default;

    GuiUserMgr&                 getUserMgr( void )                          { return m_UserMgr; }

    virtual bool                canDirectConnectToUser( void );
    virtual VxGUID              getAvatarThumbGuid( void );

    virtual bool                onlineNameMatch( QString& searchText );
    virtual bool                compareLessThan( GuiUser* guiOther );

    virtual bool                getIsFavorite( void );

    virtual std::string         dumpUserInfo( void ); // for debug

protected:
    GuiUserMgr&                 m_UserMgr;
};
