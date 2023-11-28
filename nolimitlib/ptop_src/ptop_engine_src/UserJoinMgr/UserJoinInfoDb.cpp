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

#include "UserJoinInfoDb.h"
#include "UserJoinInfo.h"

#include <CoreLib/VxPtopUrl.h>

namespace
{
    std::string 		TABLE_USER_HOST	 				= "tblUserJoin";

    std::string 		CREATE_COLUMNS_USER_HOST		= " (onlineId TEXT, thumbId TEXT, infoModMs BIGINT, pluginType INTEGER, friendState INTEGER, joinState INTEGER, lastConnMs BIGINT, lastJoinMs BIGINT, hostFlags INTEGER, hostUrlIpv4 TEXT, hostUrlIpv6 TEXT) ";

    const int			COLUMN_ONLINE_ID			    = 0;
    const int			COLUMN_HOST_THUMB_ID			= 1;
    const int			COLUMN_INFO_MOD_MS				= 2;
    const int			COLUMN_PLUGIN_TYPE			    = 3; // this is actually host type TODO: change column name to hostType
    const int			COLUMN_FRIEND_STATE			    = 4;
    const int			COLUMN_JOIN_STATE			    = 5;
    const int			COLUMN_LAST_CONN_MS				= 6;
    const int			COLUMN_LAST_JOIN_MS			    = 7;
    const int			COLUMN_HOST_FLAGS			    = 8;
    const int			COLUMN_HOST_URL_IPV4			= 9;
    const int			COLUMN_HOST_URL_IPV6			= 10;
}

//============================================================================
UserJoinInfoDb::UserJoinInfoDb( P2PEngine& engine, UserJoinMgr& hostListMgr, const char* dbName )
    : DbBase( dbName )
    , m_Engine( engine )
    , m_UserJoinMgr( hostListMgr )
{
}

//============================================================================
//! create tables in database 
RCODE UserJoinInfoDb::onCreateTables( int iDbVersion )
{
    lockDb();
    std::string strCmd = "CREATE TABLE " + TABLE_USER_HOST + CREATE_COLUMNS_USER_HOST;
    RCODE rc = sqlExec(strCmd);
    unlockDb();
    return rc;
}

//============================================================================
// delete tables in database
RCODE UserJoinInfoDb::onDeleteTables( int iOldVersion ) 
{
    lockDb();
    std::string strCmd = "DROP TABLE IF EXISTS " + TABLE_USER_HOST;
    RCODE rc = sqlExec(strCmd);
    unlockDb();
    return rc;
}

//============================================================================
void UserJoinInfoDb::purgeAllUserJoins( void ) 
{
    lockDb();
    std::string strCmd = "DELETE FROM " + TABLE_USER_HOST;
    RCODE rc = sqlExec( strCmd );
    unlockDb();
    if( rc )
    {
        LogMsg( LOG_ERROR, "UserJoinInfoDb::purgeAllUserJoins error %d", rc );
    }
    else
    {
        LogMsg( LOG_INFO, "UserJoinInfoDb::purgeAllUserJoins success" );
    }
}

//============================================================================
void UserJoinInfoDb::removeUserJoin( GroupieId& groupieId )
{
    std::string onlineIdStr = groupieId.getHostOnlineId().toHexString();
    DbBindList bindList( onlineIdStr.c_str() );
    bindList.add( (int)groupieId.getHostType() );
    sqlExec( "DELETE FROM tblUserJoin WHERE onlineId=? AND pluginType=?", bindList );
}

//============================================================================
bool UserJoinInfoDb::addUserJoin(   GroupieId&      groupieId,
                                    VxGUID&			thumbId,
                                    uint64_t		infoModTime,
                                    EJoinState      joinState,
                                    uint64_t		lastConnectMs,
                                    uint64_t		lastJoinMs,
                                    EFriendState    friendState,
                                    uint32_t        hostFlags,
                                    std::string     hostUrlIpv4,
                                    std::string     hostUrlIpv6
                                   )
{
    // always change is granted to was granted so when loaded the app knows have not rejoined yet
    joinState = joinState == eJoinStateJoinIsGranted ? eJoinStateJoinWasGranted : joinState;
    removeUserJoin( groupieId );

    std::string onlineIdStr = groupieId.getHostOnlineId().toHexString();
    std::string thumbIdStr = thumbId.toHexString();

    DbBindList bindList( onlineIdStr.c_str() );
    bindList.add( thumbIdStr.c_str() );
    bindList.add( infoModTime );
    bindList.add( (int)groupieId.getHostType() );
    bindList.add( (int)friendState );
    bindList.add( (int)joinState );
    bindList.add( lastConnectMs );
    bindList.add( lastJoinMs ); 
    bindList.add( (int)hostFlags );
    bindList.add( hostUrlIpv4.c_str() );
    bindList.add( hostUrlIpv6.c_str() );
   
    RCODE rc = sqlExec( "INSERT INTO tblUserJoin (onlineId, thumbId, infoModMs, pluginType, friendState, joinState, lastConnMs, lastJoinMs, hostFlags, hostUrlIpv4, hostUrlIpv6) values(?,?,?,?,?,?,?,?,?,?,?)",
        bindList );
    vx_assert( 0 == rc );
    if( rc )
    {
        LogMsg( LOG_ERROR, "UserJoinInfoDb::addUserJoin error %d", rc );
    }

    return ( 0 == rc );
}

//============================================================================
bool UserJoinInfoDb::addUserJoin( UserJoinInfo* hostInfo )
{
    return addUserJoin(	hostInfo->getGroupieId(),
                        hostInfo->BaseInfo::getThumbId(),
                        hostInfo->BaseInfo::getInfoModifiedTime(),  
                        hostInfo->BaseJoinInfo::getJoinState(),		
                        hostInfo->BaseJoinInfo::getLastConnectTime(),	
                        hostInfo->BaseJoinInfo::getLastJoinTime(),
                        hostInfo->getFriendState(),
                        hostInfo->getHostFlags(),
                        hostInfo->getHostUrl( false ),
                        hostInfo->getHostUrl( true )
                        );
}

//============================================================================
void UserJoinInfoDb::getAllUserJoins( std::map<GroupieId, UserJoinInfo*>& UserJoinUserJoinList )
{
    lockDb();
    DbCursor * cursor = startQuery( "SELECT * FROM tblUserJoin" ); 
    if( NULL != cursor )
    {
        while( cursor->getNextRow() )
        {
            UserJoinInfo * hostInfo = new UserJoinInfo();

            hostInfo->setOnlineId( cursor->getString( COLUMN_ONLINE_ID ) );
            hostInfo->setThumbId( cursor->getString( COLUMN_HOST_THUMB_ID ) );
            hostInfo->setInfoModifiedTime( (uint64_t)cursor->getS64( COLUMN_INFO_MOD_MS ) );
            hostInfo->setHostType( (EHostType)cursor->getS32( COLUMN_PLUGIN_TYPE ) );
            hostInfo->setFriendState( (EFriendState)cursor->getS32( COLUMN_FRIEND_STATE ) );
            hostInfo->setJoinState( (EJoinState)cursor->getS32( COLUMN_JOIN_STATE ) );
            hostInfo->setLastConnectTime( (uint64_t)cursor->getS64( COLUMN_LAST_CONN_MS ) );
            hostInfo->setLastJoinTime(  (uint64_t)cursor->getS64( COLUMN_LAST_JOIN_MS ) ); 
            hostInfo->setHostFlags( (uint32_t)cursor->getS32( COLUMN_HOST_FLAGS ) );
            hostInfo->setHostUrl( false, cursor->getString( COLUMN_HOST_URL_IPV4 ) );
            hostInfo->setHostUrl( true, cursor->getString( COLUMN_HOST_URL_IPV6 ) );

            vx_assert( hostInfo->isValid() );

            VxGUID onlineId;
            VxPtopUrl ptopUrlIpv4( hostInfo->getHostUrl( false ) );
            VxPtopUrl ptopUrlIpv6( hostInfo->getHostUrl( true ) );
            if( ptopUrlIpv4.isValid() )
            {
                onlineId = ptopUrlIpv4.getOnlineId();
            }
            else if( ptopUrlIpv6.isValid() )
            {
                onlineId = ptopUrlIpv6.getOnlineId();
            }

            if( onlineId.isVxGUIDValid() && ( ptopUrlIpv4.isValid() || ptopUrlIpv6.isValid() ) && hostInfo->getOnlineId().isVxGUIDValid() )
            {
                GroupieId groupieId( hostInfo->getOnlineId(), onlineId, hostInfo->getHostType() );
                hostInfo->setGroupieId( groupieId );
                insertUserJoinInTimeOrder( hostInfo, UserJoinUserJoinList );
            }
            else
            {
                LogMsg( LOG_ERROR, "UserJoinInfoDb::getAllUserJoins invalid id or host url" );
            }         
        }

        cursor->close();
    }

    unlockDb();
} 

//============================================================================
void UserJoinInfoDb::insertUserJoinInTimeOrder( UserJoinInfo *hostInfo, std::map<GroupieId, UserJoinInfo*>& hostList )
{
    hostList[hostInfo->getGroupieId() ] = hostInfo;
    /*
    vx_assert( hostInfo && hostInfo->isValid() );
    if( hostInfo->isDefaultHost() )
    {
        hostList[ hostInfo->get( hostList.begin(), hostInfo );
        return;
    }

    std::vector<UserJoinInfo*>::iterator iter;
    for( iter = hostList.begin(); iter != hostList.end(); ++iter )
    {
        if( ( *iter )->isDefaultHost() )
        {
            // skip defaults so they are at front of list
            continue;
        }
        else if( (*iter)->getLastJoinTime() > hostInfo->getLastJoinTime() )
        {
            hostList.insert( iter, hostInfo );
            return;
        }
    }

    hostList.push_back( hostInfo );
    */
}

