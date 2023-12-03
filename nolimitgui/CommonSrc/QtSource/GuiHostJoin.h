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

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <GuiInterface/IDefs.h>

#include <PktLib/GroupieId.h>

#include <vector>

class AppCommon;
class GuiUser;
class GuiHostJoinMgr;

class GuiHostJoin : public QWidget
{
public:
    GuiHostJoin() = delete;
    GuiHostJoin( AppCommon& app );
    GuiHostJoin( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId );
    GuiHostJoin( const GuiHostJoin& rhs );
	virtual ~GuiHostJoin() = default;

    GuiHostJoinMgr&             getHostJoinMgr( void )                  { return m_HostJoinMgr; }

    void                        setUser( GuiUser* guiUser )             { m_GuiUser = guiUser; }
    GuiUser*                    getUser( void )                         { return m_GuiUser; }

    void                        setGroupieId( GroupieId& groupieId )    { m_GroupieId = groupieId; }
    GroupieId&                  getGroupieId( void )                    { return m_GroupieId; }

    bool                        setJoinState( EJoinState joinState );
    EJoinState                  getJoinState( void )                    { return m_JoinState; }

    bool                        isOnline( void );
    bool                        isInSession( void )                     { return isOnline() && m_SessionId.isVxGUIDValid(); }

    void                        setHostOnlineStatus( bool hostOnline )  { m_HostOnlineStatus = hostOnline; }
    bool                        getHostOnlineStatus( void )             { return m_HostOnlineStatus; }

    int                         getHostRequestCount( void );
    int                         getRequestStateCount( EJoinState joinState );
    void                        getRequestStateHosts( EJoinState joinState, std::vector<EHostType>& hostRequests );

    std::string                 getOnlineName( void );

protected:
    AppCommon&                  m_MyApp;
    GuiHostJoinMgr&             m_HostJoinMgr;

    GuiUser*                    m_GuiUser{ nullptr };
    GroupieId                   m_GroupieId;
    EJoinState                  m_JoinState{ eJoinStateNone };
    VxGUID                      m_SessionId;
    bool                        m_HostOnlineStatus{ false };
};
