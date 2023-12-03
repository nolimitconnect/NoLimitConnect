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
class GuiHostedListMgr;
class HostedInfo;

class GuiHosted : public QWidget
{
public:
    GuiHosted() = delete;
    GuiHosted( AppCommon& app );
    GuiHosted( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId );
    GuiHosted( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId, HostedInfo& hostedInfo );
    GuiHosted( const GuiHosted& rhs );
	virtual ~GuiHosted() = default;

    GuiHostedListMgr&           getHostedListMgr( void )        { return m_HostedListMgr; }

    void                        setUser( GuiUser* guiUser )     { m_GuiUser = guiUser; }
    GuiUser*                    getUser( void )                 { return m_GuiUser; }

    void                        setHostedId( HostedId& hostedId ) { m_HostedId = hostedId; }
    HostedId&                   getHostedId( void )             { return m_HostedId; }

    virtual void				setHostOnlineId( VxGUID& onlineId ) { m_HostedId.setHostOnlineId( onlineId ); }
    virtual VxGUID&             getHostOnlineId( void )             { return m_HostedId.getHostOnlineId(); }

    virtual void			    setHostType( EHostType hostType ) { m_HostedId.setHostType( hostType ); }
    virtual EHostType	        getHostType( void )             { return m_HostedId.getHostType(); }

    virtual void				setSessionId( VxGUID& sessionId ) { m_SessionId = sessionId; }
    virtual VxGUID&             getSessionId( void )            { return m_SessionId; }

    virtual void				setThumbId( VxGUID& thumbId )   { m_ThumbId = thumbId; }
    virtual VxGUID&             getThumbId( void )              { return m_ThumbId; }

    virtual void                setIsFavorite( bool isFavorite ) { m_IsFavorite = isFavorite; }
    virtual bool                getIsFavorite( void )           { return m_IsFavorite; }

    virtual void			    setConnectedTimestamp( int64_t timestampMs ) { m_ConnectedTimestampMs = timestampMs; }
    virtual int64_t             getConnectedTimestamp( void )   { return m_ConnectedTimestampMs; }
    virtual void			    setJoinedTimestamp( int64_t timestampMs ) { m_JoinedTimestampMs = timestampMs; }
    virtual int64_t             getJoinedTimestamp( void )      { return m_JoinedTimestampMs; }
    virtual void			    setHostInfoTimestamp( int64_t timestampMs ) { m_HostInfoTimestampMs = timestampMs; }
    virtual int64_t             getHostInfoTimestamp( void )    { return m_HostInfoTimestampMs; }

    virtual void			    setHostInviteUrl( bool ipv6, std::string hostUrl )  { ipv6 ? m_HostInviteUrlIpv6 = hostUrl : m_HostInviteUrlIpv4 = hostUrl; }
    virtual std::string&        getHostInviteUrl( bool ipv6 )                       { return ipv6 ? m_HostInviteUrlIpv6 : m_HostInviteUrlIpv4; }

    virtual void                setHostTitle( std::string hostTitle ) { m_HostTitle = hostTitle; }
    virtual std::string&        getHostTitle( void )            { return m_HostTitle; }

    virtual void                setHostDescription( std::string hostDesc ) { m_HostDesc = hostDesc; }
    virtual std::string&        getHostDescription( void )      { return m_HostDesc; }

    virtual void                setIsIgnored( bool isIgnored )  { m_IsIgnored = isIgnored; }
    virtual bool                getIsIgnored( void )            { return m_IsIgnored; }

    virtual bool                readyForClientLaunch( void );

    virtual GroupieId           getMyGroupieId( void );

protected:
    AppCommon&                  m_MyApp;
    GuiHostedListMgr&           m_HostedListMgr;

    GuiUser*                    m_GuiUser{ nullptr };
    HostedId                    m_HostedId;
    VxGUID                      m_SessionId;
    bool                        m_IsFavorite{ false };
    bool                        m_IsIgnored{ false };
    int64_t                     m_ConnectedTimestampMs{ 0 };
    int64_t                     m_JoinedTimestampMs{ 0 };
    int64_t                     m_HostInfoTimestampMs{ 0 };
    std::string                 m_HostInviteUrlIpv4;
    std::string                 m_HostInviteUrlIpv6;
    std::string                 m_HostTitle;
    std::string                 m_HostDesc;
    VxGUID                      m_ThumbId;
};
