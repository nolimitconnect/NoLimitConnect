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

#include <PktLib/GroupieId.h>

#include <string>

class PktHostInviteAnnounceReq;

class HostedInfo
{
public:
	HostedInfo() = default;
	HostedInfo( const HostedInfo& rhs );
    HostedInfo( EHostType hostType, VxGUID& onlineId, std::string& hostUrlIpv4, std::string& hostUrlIpv6, VxGUID& thumbId );
    virtual ~HostedInfo() = default;

	HostedInfo&				    operator=( const HostedInfo& rhs ); 

    bool                        isHostInviteValid( void );

    virtual void				setHostedId( HostedId& hostedId )                   { m_HostedId = hostedId; }
    HostedId&                   getHostedId( void )                                 { return m_HostedId; }
    virtual void				setHostOnlineId( VxGUID& onlineId )                 { m_HostedId.setHostOnlineId( onlineId ); }
    virtual VxGUID&             getHostOnlineId( void )                             { return m_HostedId.getHostOnlineId(); }
    virtual void			    setHostType( enum EHostType hostType )              { m_HostedId.setHostType( hostType ); }
    virtual EHostType	        getHostType( void )                                 { return m_HostedId.getHostType(); }

    EPluginType                 getHostPluginType( void );
    EPluginType                 getClientPluginType( void );

    virtual void				setThumbId( VxGUID& thumbId )                       { m_ThumbId = thumbId; }
    virtual VxGUID&             getThumbId( void )                                  { return m_ThumbId; }

    virtual void                setIsFavorite( bool isFavorite )                    { m_IsFavorite = isFavorite; }
    virtual bool                getIsFavorite( void )                               { return m_IsFavorite; }

    virtual void			    setConnectedTimestamp( int64_t timestampMs )        { m_ConnectedTimestampMs = timestampMs; }
    virtual int64_t             getConnectedTimestamp( void )                       { return m_ConnectedTimestampMs; }
    virtual void			    setJoinedTimestamp( int64_t timestampMs )           { m_JoinedTimestampMs = timestampMs; }
    virtual int64_t             getJoinedTimestamp( void )                          { return m_JoinedTimestampMs; }
    virtual void			    setHostInfoTimestamp( int64_t timestampMs )         { m_HostInfoTimestampMs = timestampMs; }
    virtual int64_t             getHostInfoTimestamp( void )                        { return m_HostInfoTimestampMs; }

    virtual void			    setHostInviteUrl( bool ipv6, std::string hostUrl )  { ipv6 ? m_HostInviteUrlIpv6 = hostUrl : m_HostInviteUrlIpv4 = hostUrl; }
    virtual std::string&        getHostInviteUrl( bool ipv6 )                       { return ipv6 ? m_HostInviteUrlIpv6 : m_HostInviteUrlIpv4; }

    virtual void                setHostTitle( std::string hostTitle )               { m_HostTitle = hostTitle; }
    virtual std::string&        getHostTitle( void )                                { return m_HostTitle; }

    virtual void                setHostDescription( std::string hostDesc )          { m_HostDesc = hostDesc; }
    virtual std::string&        getHostDescription( void )                          { return m_HostDesc; }

    bool                        shouldSaveToDb( void );
    bool                        isValidForGui( void );

    bool                        fillFromHostInvite( PktHostInviteAnnounceReq* hostAnn );
    int                         getSearchBlobSpaceRequirement( void );
    bool                        fillSearchBlob( PktBlobEntry& blobEntry );
    bool                        extractFromSearchBlob( PktBlobEntry& blobEntry );

protected:
	//=== vars ===//
    HostedId                    m_HostedId;
    int64_t                     m_ConnectedTimestampMs{ 0 };
    int64_t                     m_JoinedTimestampMs{ 0 };
    int64_t                     m_HostInfoTimestampMs{ 0 };
    bool                        m_IsFavorite{ false };
    std::string                 m_HostInviteUrlIpv4;
    std::string                 m_HostInviteUrlIpv6;
    std::string                 m_HostTitle;
    std::string                 m_HostDesc;
    VxGUID                      m_ThumbId;
};
