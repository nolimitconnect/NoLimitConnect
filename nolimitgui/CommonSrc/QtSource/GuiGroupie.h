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
#include <CoreLib/VxGUID.h>

#include <vector>

class AppCommon;
class GuiUser;
class GuiGroupieListMgr;
class GroupieInfo;

class GuiGroupie : public QWidget
{
public:
    GuiGroupie() = delete;
    GuiGroupie( AppCommon& app );
    GuiGroupie( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId );
    GuiGroupie( AppCommon& app, GuiUser* guiUser, GroupieId& groupieId, VxGUID& sessionId );
    GuiGroupie( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId, GroupieInfo& groupieInfo );
    GuiGroupie( const GuiGroupie& rhs );
	virtual ~GuiGroupie() = default;

    GuiGroupieListMgr&          getGroupieListMgr( void )                   { return m_GroupieListMgr; }

    void                        setUser( GuiUser* guiUser )                    { m_GuiUser = guiUser; }
    GuiUser*                    getUser( void )                             { return m_GuiUser; }

    void				        setGroupieId( GroupieId& groupieId )        { m_GroupieId = groupieId; }
    GroupieId&                  getGroupieId( void )                        { return m_GroupieId; }
    void				        setUserOnlineId( VxGUID& onlineId )         { m_GroupieId.setUserOnlineId( onlineId ); }
    VxGUID&                     getUserOnlineId( void )                     { return m_GroupieId.getUserOnlineId(); }

    void			            setHostOnlineId( VxGUID& onlineId )         { m_GroupieId.setHostOnlineId( onlineId ); }
    VxGUID&                     getHostOnlineId( void )                     { return m_GroupieId.getHostOnlineId(); }

    void			            setHostType( EHostType hostType )           { m_GroupieId.setHostType( hostType ); }
    EHostType	                getHostType( void )                         { return m_GroupieId.getHostType(); }

    virtual void                setIsFavorite( bool isFavorite )            { m_IsFavorite = isFavorite; }
    virtual bool                getIsFavorite( void )                       { return m_IsFavorite; }

    virtual void			    setGroupieInfoTimestamp( int64_t timestampMs ) { m_GroupieInfoTimestampMs = timestampMs; }
    virtual int64_t             getGroupieInfoTimestamp( void )             { return m_GroupieInfoTimestampMs; }

    virtual void			    setGroupieUrl( bool ipv6, std::string hostUrl )  { ipv6 ? m_GroupieUrlIpv6 = hostUrl : m_GroupieUrlIpv4 = hostUrl; }
    virtual std::string&        getGroupieUrl( bool ipv6 )                       { return ipv6 ? m_GroupieUrlIpv6 : m_GroupieUrlIpv4; }

    virtual void                setGroupieTitle( std::string hostTitle )    { m_GroupieTitle = hostTitle; }
    virtual std::string&        getGroupieTitle( void )                     { return m_GroupieTitle; }

    virtual void                setGroupieDescription( std::string hostDesc ) { m_GroupieDesc = hostDesc; }
    virtual std::string&        getGroupieDescription( void )               { return m_GroupieDesc; }

protected:
    AppCommon&                  m_MyApp;
    GuiGroupieListMgr&          m_GroupieListMgr;

    GuiUser*                    m_GuiUser{ nullptr };
    GroupieId                   m_GroupieId;
    VxGUID                      m_SessionId;
    
    bool                        m_IsFavorite{ false };
    int64_t                     m_GroupieInfoTimestampMs{ 0 };
    std::string                 m_GroupieUrlIpv4;
    std::string                 m_GroupieUrlIpv6;
    std::string                 m_GroupieTitle;
    std::string                 m_GroupieDesc;
};
