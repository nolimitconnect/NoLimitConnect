#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <PktLib/GroupieId.h>

#include <string>

class PktGroupieAnnounceReq;
class GroupieId;

class GroupieInfo
{
public:
	GroupieInfo() = default;
	GroupieInfo( const GroupieInfo& rhs );
    GroupieInfo( VxGUID& groupieOnlineId, VxGUID& hostOnlineId, enum EHostType hostType, std::string& groupieUrlIpv4, std::string& groupieUrlIpv6 );
    GroupieInfo( GroupieId& groupieId, std::string& groupieUrlIpv4, std::string& groupieUrlIpv6, std::string& groupieTitle, std::string& groupieDesc, int64_t timeModified );
    virtual ~GroupieInfo() = default;

	GroupieInfo&				operator=( const GroupieInfo& rhs ); 

    bool                        isGroupieValid( void );
    bool                        isIdMatch( GroupieId& groupieId );
    bool                        isSearchTextMatch( std::string& searchText );

    bool                        setGroupieUrlAndTitleAndDescription( std::string& groupieUrlIpv4, std::string& groupieUrlIpv6, std::string& groupieTitle, std::string& groupieDesc, int64_t& lastModifiedTime );
    bool                        getGroupieUrlAndTitleAndDescription( std::string& groupieUrlIpv4, std::string& groupieUrlIpv6, std::string& groupieTitle, std::string& groupieDesc, int64_t& lastModifiedTime );

    void				        setGroupieId( GroupieId& onlineId )                 { m_GroupieId = onlineId; }
    GroupieId&                  getGroupieId( void )                                { return m_GroupieId; }

    void				        setUserOnlineId( VxGUID& onlineId )                 { m_GroupieId.setUserOnlineId( onlineId ); }
    VxGUID&                     getUserOnlineId( void )                             { return m_GroupieId.getUserOnlineId(); }

    void				        setHostOnlineId( VxGUID& onlineId )                 { m_GroupieId.setHostOnlineId( onlineId ); }
    VxGUID&                     getHostOnlineId( void )                             { return m_GroupieId.getHostOnlineId(); }

    void			            setHostType( enum EHostType hostType )              { m_GroupieId.setHostType( hostType ); }
    EHostType	                getHostType( void )                                 { return m_GroupieId.getHostType(); }

    EPluginType	                getHostPluginType( void );
    EPluginType	                getClientPluginType( void );

    virtual void                setIsFavorite( bool isFavorite )                    { m_IsFavorite = isFavorite; }
    virtual bool                getIsFavorite( void )                               { return m_IsFavorite; }

    virtual void			    setConnectedTimestamp( int64_t timestampMs )        { m_ConnectedTimestampMs = timestampMs; }
    virtual int64_t             getConnectedTimestamp( void )                       { return m_ConnectedTimestampMs; }
    virtual void			    setJoinedTimestamp( int64_t timestampMs )           { m_JoinedTimestampMs = timestampMs; }
    virtual int64_t             getJoinedTimestamp( void )                          { return m_JoinedTimestampMs; }
    virtual void			    setGroupieInfoTimestamp( int64_t timestampMs )      { m_GroupieInfoTimestampMs = timestampMs; }
    virtual int64_t             getGroupieInfoTimestamp( void )                     { return m_GroupieInfoTimestampMs; }

    virtual void			    setGroupieUrl( bool ipv6, std::string hostUrl )     { ipv6 ? m_GroupieUrlIpv6 = hostUrl : m_GroupieUrlIpv4 = hostUrl; }
    virtual std::string&        getGroupieUrl( bool ipv6 )                          { return ipv6 ? m_GroupieUrlIpv6 : m_GroupieUrlIpv4; }

    virtual void                setGroupieTitle( std::string hostTitle )            { m_GroupieTitle = hostTitle; }
    virtual std::string&        getGroupieTitle( void )                             { return m_GroupieTitle; }

    virtual void                setGroupieDescription( std::string hostDesc )       { m_GroupieDesc = hostDesc; }
    virtual std::string&        getGroupieDescription( void )                       { return m_GroupieDesc; }

    bool                        shouldSaveToDb( void );
    bool                        isValidForGui( void );

    bool                        fillFromGroupie( PktGroupieAnnounceReq* hostAnn );
    int                         getSearchBlobSpaceRequirement( void );
    bool                        fillSearchBlob( PktBlobEntry& blobEntry );
    bool                        extractFromSearchBlob( PktBlobEntry& blobEntry );

protected:
	//=== vars ===//
    GroupieId                   m_GroupieId;
    int64_t                     m_ConnectedTimestampMs{ 0 };
    int64_t                     m_JoinedTimestampMs{ 0 };
    int64_t                     m_GroupieInfoTimestampMs{ 0 };
    bool                        m_IsFavorite{ false };
    std::string                 m_GroupieUrlIpv4;
    std::string                 m_GroupieUrlIpv6;
    std::string                 m_GroupieTitle;
    std::string                 m_GroupieDesc;
};
