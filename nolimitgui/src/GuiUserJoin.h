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

#include <QWidget>

#include <CoreLib/GroupieId.h>

#include <vector>

class AppCommon;
class GuiUser;
class GuiUserJoinMgr;
class UserJoinInfo;

class GuiUserJoin : public QWidget
{
public:
    GuiUserJoin() = delete;
    GuiUserJoin( AppCommon& app );
    GuiUserJoin( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId, bool online = false );
    GuiUserJoin( AppCommon& app, GuiUser* guiUser, UserJoinInfo* userJoinInfo );
    GuiUserJoin( const GuiUserJoin& rhs );
    virtual ~GuiUserJoin() = default;

    GuiUserJoinMgr&             getUserJoinMgr( void )                  { return m_UserJoinMgr; }

    void                        setUser( GuiUser* guiUser )             { m_GuiUser = guiUser; }
    GuiUser*                    getUser( void )                         { return m_GuiUser; }

    void                        setGroupieId( GroupieId& groupieId )    { m_GroupieId = groupieId; }
    GroupieId&                  getGroupieId( void )                    { return m_GroupieId; }

    bool                        setJoinState( EJoinState joinState );
    EJoinState                  getJoinState( void )                    { return m_JoinState; }

    bool                        isOnline( void );
    bool                        isInSession( void )                     { return isOnline() && m_SessionId.isValid(); }

    std::string                 getOnlineName( void );
    VxGUID                      getMyOnlineId( void );

protected:
    AppCommon&                  m_MyApp;
    GuiUserJoinMgr&             m_UserJoinMgr;

    GuiUser*                    m_GuiUser{ nullptr };
    GroupieId                   m_GroupieId;
    EJoinState                  m_JoinState{ eJoinStateNone };
    VxGUID                      m_SessionId;
};
