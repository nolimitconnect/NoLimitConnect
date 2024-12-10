//============================================================================
// Copyright (C) 2003 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <BigListLib/BigListMgr.h>

#include <BigListLib/BigListInfo.h>
#include <GuiInterface/IToGui.h>
#include <Network/StayConnected.h>
#include <Network/NetworkMgr.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxTimeDefs.h>

#include <PktLib/PktAnnList.h>

#include <memory.h>
#include <string.h>
#include <string>

#define BIGLIST_VERSION 0x100

#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif //_MSC_VER

//============================================================================
const char* DescribePktAnnUpdateType( EPktAnnUpdateType pktAnnUpdateType )
{
	switch( pktAnnUpdateType )
	{
	case ePktAnnUpdateTypeIgnored:
		return "ePktAnnUpdateTypeIgnored";
	case ePktAnnUpdateTypeNewContact:
		return "ePktAnnUpdateTypeNewContact";
	case ePktAnnUpdateTypeContactIsSame:
		return "ePktAnnUpdateTypeContactIsSame";
	case ePktAnnUpdateTypeContactChanged:
		return "ePktAnnUpdateTypeContactChanged";
	default:
		return "PktAnnUpdate Type Unknown";
	}
}

//============================================================================
BigListMgr::BigListMgr( P2PEngine& engine )
: BigListDb( engine, *this )
{	
}

//============================================================================
BigListMgr::~BigListMgr()
{
	bigListMgrShutdown();
}

//============================================================================
//! startup
RCODE BigListMgr::bigListMgrStartup( const char* pDbFileName )
{
	if( m_BigListMgrInitialized )
	{
		bigListMgrShutdown();
	}

    m_BigListMgrInitialized = true;
	return bigListDbStartup( pDbFileName );
}

//============================================================================
//! shutdown
RCODE BigListMgr::bigListMgrShutdown( void )
{
    m_BigListMgrInitialized = false;
	return bigListDbShutdown();
}

//============================================================================
RCODE BigListMgr::updateBigListDatabase( BigListInfo * poInfo, const char* networkName )
{
	RCODE rc = 0;
	if( 0 == poInfo )
	{
		return rc;
	}

	if( m_Engine.shouldInfoBeInDatabase( poInfo ))
	{
		// insert into database
		rc = dbUpdateBigListInfo( poInfo, networkName );
	}
	else
	{
		rc = dbRemoveBigListInfo( poInfo->getMyOnlineId() );
		poInfo->setIsInDatabase( false );
	}

	return rc;
}

//============================================================================
bool BigListMgr::getFriendships( VxGUID& hisOnlineId, EFriendState& retMyFriendshipToHim, EFriendState& retHisFriendshipToMe )
{
	retMyFriendshipToHim = eFriendStateAnonymous;
	retHisFriendshipToMe = eFriendStateAnonymous;

	BigListAutoLock bigListAutoLock( *this );
	BigListInfo* poInfo = findBigListInfo( hisOnlineId, true );	// id of friend to look for
	if( poInfo )
	{
		retMyFriendshipToHim = poInfo->getMyFriendshipToHim();
		retHisFriendshipToMe = poInfo->getHisFriendshipToMe();
		return true;
	}

	return false;
}

//============================================================================
bool BigListMgr::isUserIgnored( VxGUID& hisOnlineId )
{
	EFriendState myFriendshipToHim{ eFriendStateAnonymous };
	EFriendState hisFriendshipToMe{ eFriendStateAnonymous };
	if( getFriendships( hisOnlineId, myFriendshipToHim, hisFriendshipToMe ) )
	{
		return myFriendshipToHim == eFriendStateIgnore;
	}

	return false;
}

//============================================================================
bool BigListMgr::getOnlineName( VxGUID& hisOnlineId, std::string& onlineName )
{
	BigListAutoLock bigListAutoLock( *this );
	BigListInfo * poInfo = findBigListInfo( hisOnlineId, true );	// id of friend to look for
	if( poInfo )
	{
		onlineName = poInfo->getOnlineName();
		return true;
	}

	onlineName = "";
	return false;
}

//============================================================================
//! add a or update remote friend.. return true 
EPktAnnUpdateType BigListMgr::updatePktAnn(	PktAnnounce *		poPktAnnIn,	
											BigListInfo **		ppoRetInfo,
											EHostType			hostType,
											bool				useMyFriendshipFromPktAnn,
											bool				useHisFriendshipFromPktAnn )	
{
	// temp for debug
	std::string onlineName = poPktAnnIn->getOnlineName();
	bool debugUser = onlineName == "Dev A8 tab" || onlineName == "Win2 Dev";
	if( debugUser )
	{
		LogMsg( LOG_WARNING, "updatePktAnn %s", onlineName.c_str() );
	}

	EFriendState myFriendship = poPktAnnIn->getMyFriendshipToHim();
	EFriendState hisFriendship = poPktAnnIn->getHisFriendshipToMe();

	BigListAutoLock bigListAutoLock( *this );
	EPktAnnUpdateType eUpdateType = ePktAnnUpdateTypeContactIsSame;
	bool hostedUserUpdate = IsHostARelayForUsers( hostType ); // update is from host.. do not lower his friendship to you
	bool isMySelf = poPktAnnIn->getMyOnlineId() == m_Engine.getMyOnlineId();
	if( isMySelf )
	{
		poPktAnnIn->setMyFriendshipToHim( eFriendStateFriend );
		poPktAnnIn->setHisFriendshipToMe( eFriendStateFriend );
		LogMsg( LOG_WARNING, "updatePktAnn updating myself" );
	}
	else
	{
		if( IsHostARelayForUsers( hostType ) )
		{
			LogModule( eLogConnect, LOG_VERBOSE, "updatePktAnn updating host %s my friendship %s his friendship %s",
					   DescribeHostType( hostType ), DescribeFriendState( myFriendship ), DescribeFriendState( hisFriendship ) );
		}
		else
		{
			LogModule( eLogConnect, LOG_VERBOSE, "updatePktAnn updating my friendship %s his friendship %s",
					   DescribeFriendState( myFriendship ), DescribeFriendState( hisFriendship ) );
		}
	}

	BigListInfo * poInfo = findBigListInfo( poPktAnnIn->getMyOnlineId(), true );	// id of friend to look for
	if( poInfo )
	{
		if( !isMySelf )
		{
			if( poInfo->isIgnored() )
			{
				// ignore this person
				poPktAnnIn->setMyFriendshipToHim( eFriendStateIgnore );
				return ePktAnnUpdateTypeIgnored;
			}

			if( poPktAnnIn->getLastSessionTimeMs() > poInfo->getLastSessionTimeMs() )
			{
				poInfo->setLastSessionTimeMs( poPktAnnIn->getLastSessionTimeMs() );
			}
			else
			{
				poPktAnnIn->setLastSessionTimeMs( poInfo->getLastSessionTimeMs() );
			}

			EFriendState hisFriendshipToMe = poPktAnnIn->getHisFriendshipToMe();
			if( !useHisFriendshipFromPktAnn && poInfo->getHisFriendshipToMe() > eFriendStateGuest )
			{
				// use last known friendship. This is for the case that the pkt ann is from host and not directly from user
				hisFriendshipToMe = poInfo->getHisFriendshipToMe();
			}

			bool friendshipChanged = hisFriendshipToMe  != poInfo->getHisFriendshipToMe();
			poInfo->setHisFriendshipToMe( hisFriendshipToMe );
			poPktAnnIn->setHisFriendshipToMe( hisFriendshipToMe );

			// update permission levels to guest if needed
			if( useMyFriendshipFromPktAnn )
			{
				// just assume changed
				poInfo->setMyFriendshipToHim( poPktAnnIn->getMyFriendshipToHim() );
				updateVectorList( poPktAnnIn->getMyFriendshipToHim(), poInfo );
				friendshipChanged = true;
			}
			else
			{
				if( hostedUserUpdate && poInfo->isAnonymous() )
				{
					poPktAnnIn->setMyFriendshipToHim( eFriendStateGuest );
					poInfo->makeGuest();
					updateVectorList( eFriendStateGuest, poInfo );
					friendshipChanged = true;
				}
			}

			poPktAnnIn->setMyFriendshipToHim( poInfo->getMyFriendshipToHim() );
			if( friendshipChanged )
			{
				//m_Engine.toGuiContactFriendshipChange( poInfo );
				eUpdateType = ePktAnnUpdateTypeContactChanged;
			}

			poPktAnnIn->getOnlineName()[ MAX_ONLINE_NAME_LEN - 1 ] = 0;
			if( 0 != strcmp( poPktAnnIn->getOnlineName(), poInfo->getOnlineName() ) )
			{
				memcpy( poInfo->getOnlineName(), poPktAnnIn->getOnlineName(), MAX_ONLINE_NAME_LEN );
				//m_Engine.toGuiContactNameChange( poInfo );
				eUpdateType = ePktAnnUpdateTypeContactChanged;
			}

			poPktAnnIn->getOnlineDescription()[ MAX_ONLINE_DESC_LEN - 1 ] = 0;
			if( 0 != strcmp( poPktAnnIn->getOnlineDescription(), poInfo->getOnlineDescription() ) )
			{
				memcpy( poInfo->getOnlineDescription(), poPktAnnIn->getOnlineDescription(), MAX_ONLINE_DESC_LEN );
				//m_Engine.toGuiContactDescChange( poInfo );
				eUpdateType = ePktAnnUpdateTypeContactChanged;
			}

			if( poPktAnnIn->getSearchFlags() != poInfo->getSearchFlags() )
			{
				poInfo->setSearchFlags( poPktAnnIn->getSearchFlags() );
				eUpdateType = ePktAnnUpdateTypeContactChanged;
			}

			if( 0 != memcmp( poPktAnnIn->getPluginPermissions(), poInfo->getPluginPermissions(), PERMISSION_ARRAY_SIZE ) )
			{
				memcpy( poInfo->getPluginPermissions(), poPktAnnIn->getPluginPermissions(), PERMISSION_ARRAY_SIZE );
				//m_Engine.toGuiPluginPermissionChange( poInfo );
				eUpdateType = ePktAnnUpdateTypeContactChanged;
			}

			bool contactInfoChanged = false;
			if( ( poPktAnnIn->m_DirectConnectId != poInfo->m_DirectConnectId ) ||
				( poPktAnnIn->m_u8RelayFlags != poInfo->m_u8RelayFlags ) )
			{
				contactInfoChanged = true;
				eUpdateType = ePktAnnUpdateTypeContactChanged;
			}

			memcpy( poInfo, poPktAnnIn, sizeof( VxNetIdent ) );

			if( ePktAnnUpdateTypeContactIsSame != eUpdateType )
			{
				m_Engine.toGuiContactAnythingChange( poInfo );
			}

			if( m_Engine.shouldInfoBeInDatabase( poInfo ) )
			{
				updateBigListDatabase( poInfo, m_Engine.getNetworkMgr().getNetworkKey().c_str() );
			}
			else
			{
				dbRemoveBigListInfo( poInfo->getMyOnlineId() );
			}
		}
	}
	else
	{
		if( true == canAddFriend() )
		{
			// new friend
			if( !isMySelf )
			{
				poPktAnnIn->setMyFriendshipToHim( hostedUserUpdate ? eFriendStateGuest : eFriendStateAnonymous );
			}

			poInfo = new BigListInfo();
			memcpy( poInfo, poPktAnnIn, sizeof( PktAnnounce ) );
			//LogMsg( LOG_INFO, "BigListMgr::updatePktAnn: new contact %s Hi 0x%llX, Lo 0x%llX\n", poInfo->getOnlineName(), poInfo->getMyOnlineId().getVxGUIDHiPart(), poInfo->getMyOnlineId().getVxGUIDLoPart() );

			bigInsertInfo( poInfo->getMyOnlineId(), poInfo, true );

			if( m_Engine.shouldInfoBeInDatabase( poInfo ) )
			{
				updateBigListDatabase( poInfo, m_Engine.getNetworkMgr().getNetworkKey().c_str() );
			}
			else
			{
				dbRemoveBigListInfo( poInfo->getMyOnlineId() );
			}

			//! notify new contact found
			eUpdateType = ePktAnnUpdateTypeNewContact;
		}
		else
		{
			LogMsg( LOG_ERROR, "Could not add %s to BigList", poPktAnnIn->getOnlineName() );
			eUpdateType = ePktAnnUpdateTypeIgnored;
		}
	}

	if( ppoRetInfo )
	{
		*ppoRetInfo = poInfo;
	}

	if( ePktAnnUpdateTypeContactIsSame != eUpdateType )
	{
		LogMsg( LOG_DEBUG, "Update BigListInfo %s for %s", DescribePktAnnUpdateType( eUpdateType ),  poInfo->getOnlineName() );
	}

	return eUpdateType;
}

//============================================================================
bool BigListMgr::canAddFriend( void )
{
	// first limit size.. remove old if possible
	LimitListSize();
	if( MAX_BIGLIST_ITEMS > m_BigList.size() )
	{
		return true;
	}
	LogMsg( LOG_ERROR, "BigListMgr::canAddFriend false" );
	return false;
}

//============================================================================
//! remove from big list.. also from db if bRemoveStorage = true 
RCODE BigListMgr::removeFriend( PktAnnounce * poPktAnn, bool  bRemoveStorage )
{
	BigListAutoLock bigListAutoLock( *this );
	RCODE rc = 0;
	bigRemoveInfo( poPktAnn->getMyOnlineId(), true );
	if( bRemoveStorage )
	{
		rc = dbRemoveBigListInfo( *((VxGUID *)poPktAnn) );
	}

	return rc;
}

//============================================================================
RCODE BigListMgr::FillAnnList(	PktAnnList * poPktAnnList, 
								int iMaxListLen,
								int64_t s64ContactTimeLimitMs,
								bool bIncludeThisNode )
{
	BigListAutoLock bigListAutoLock( *this );

	RCODE rc;
	BigListInfo * poInfo;
	int iemptyLen = poPktAnnList->emptyLen();

	std::map< VxGUID, BigListInfo *, cmp_vxguid >::iterator oMapIter;
	for( oMapIter = m_BigList.begin(); oMapIter != m_BigList.end(); ++oMapIter )
	{
		delete oMapIter->second;
		poInfo = oMapIter->second;
		if( poInfo->getElapsedMsTcpLastContact() <= s64ContactTimeLimitMs )
		{
			//is a node we can add if fits
			if( ((int)sizeof( VxNetIdent ) + poPktAnnList->getPktLength() ) <= (iMaxListLen + iemptyLen) )
			{
			
				if( ( false == bIncludeThisNode ) 
					&& ( m_Engine.getMyPktAnnounce().getMyOnlineId() == poInfo->getMyOnlineId() ) )
				{
					// don't include ourself
					continue;
				}
				rc = poPktAnnList->addAnn( poInfo );
				if( rc )
				{
					//list is full
					return -1;
				}
			}
			else
			{
				//filled to limit
				return -1;
			}
		}
	}
	return 0;
}

//============================================================================
void BigListMgr::LimitListSize( void )
{
	return; //NOTE: TODO NEED REWRITTEN>> THIS IS CRAP
	int iCnt = (int)m_BigList.size();
	if( MAX_BIGLIST_ITEMS >= iCnt )
	{
		// list is not to big
		return;
	}

	int iToRemoveCnt = (iCnt - MAX_BIGLIST_ITEMS) + 10; // remove a extra 10 so we don't have to do so often
	int iRemovedCnt = 0;
	BigListInfo * poCurInfo;
	//work the list backwards.. this will tend to put
	//the guests etc at the end of the list
	std::map< VxGUID, BigListInfo *, cmp_vxguid >::iterator oMapIter;
	for( oMapIter = m_BigList.end(); oMapIter != m_BigList.begin(); oMapIter-- )
	{
		poCurInfo = oMapIter->second;
		if( poCurInfo->isSafeToDelete() )
		{
			if( GetGmtTimeMs() - poCurInfo->getElapsedMsAnyContact() > WEEK_OF_MIILISECONDS )
			{
				// has not had contact in week and is not friend etc
				delete poCurInfo;
				m_BigList.erase( oMapIter );
				iRemovedCnt++;
				if( iRemovedCnt >= iToRemoveCnt )
				{
					// we have removed enough
					return;
				}
			}
		}
	}
	// couldn't remove enough.. try again with looser criteria
	for( oMapIter = m_BigList.end(); oMapIter != m_BigList.begin(); oMapIter-- )
	{
		poCurInfo = oMapIter->second;
		if( poCurInfo->isSafeToDelete() )
		{
			if( GetGmtTimeMs() - poCurInfo->getElapsedMsAnyContact() > HOUR_OF_MIILISECONDS * 3 )
			{
				// has not had contact in week and is not friend etc
				delete poCurInfo;
				m_BigList.erase( oMapIter );
				iRemovedCnt++;
				if( iRemovedCnt >= iToRemoveCnt )
				{
					// we have removed enough
					return;
				}
			}
		}
	}
}

//============================================================================
bool BigListMgr::queryIdent( VxGUID& onlineId, VxNetIdent& netIdent )
{
	bool foundIdent = false;
	if( onlineId.isVxGUIDValid() )
	{
		BigListAutoLock bigListAutoLock( *this );
		auto iter = m_BigList.find( onlineId );
		if( iter != m_BigList.end() )
		{
			BigListInfo* bigListInfo = iter->second;
			if( bigListInfo )
			{
				netIdent = *bigListInfo;
				foundIdent = true;
			}
		}
	}

	return foundIdent;
}

//============================================================================
void BigListMgr::onMyFriendshipChanged( EFriendState prevMyFriendship, VxNetIdent* netIdent )
{
    if( netIdent->getMyOnlineId() == m_Engine.getMyOnlineId() )
    {
        LogMsg( LOG_ERROR, "BigListMgr::onMyFriendshipChanged isMyself true" );
        vx_assert( false );
        return;
    }

    BigListInfo* bigListInfo = findBigListInfo( netIdent->getMyOnlineId() );

    if( bigListInfo )
	{
        updateVectorList( prevMyFriendship, bigListInfo );
	}
	else
	{
		LogMsg( LOG_ERROR, "BigListMgr::onMyFriendshipChanged null BigListInfo for %s %s", 
				netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str() );
		vx_assert( false );
	}
}

//============================================================================
bool BigListMgr::updateMemberFriendship( VxGUID& onlineId, bool isMember )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        LogMsg( LOG_ERROR, "BigListMgr::updateMemberFriendship isMyself true" );
        vx_assert( false );
        return false;
    }

	BigListInfo* bigListInfo = findBigListInfo( onlineId );
	if( bigListInfo )
	{
		return updateMemberFriendship( bigListInfo, isMember );
	}

    LogMsg( LOG_ERROR, "BigListMgr::updateMemberFriendship invalid param null BigListInfo for id %s", onlineId.toOnlineIdString().c_str() );
	vx_assert( false );
	return false;
}

//============================================================================
bool BigListMgr::updateMemberFriendship( BigListInfo* bigListInfo, bool isMember )
{
	if( !isMember )
	{
		// TODO should we downgrade to anonymouse
		return true;
	}

	if( !bigListInfo )
	{
		LogMsg( LOG_ERROR, "BigListMgr::updateMemberFriendship invalid param null BigListInfo" );
		vx_assert( false );
		return false;
	}

	if( bigListInfo->isIgnored() )
	{
		LogMsg( LOG_ERROR, "BigListMgr::updateMemberFriendship ignore BigListInfo for %s id %s", 
			bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str());
		return false;
	}

	
	if( bigListInfo->isMyself() )
	{
		LogMsg( LOG_ERROR, "BigListMgr::updateMemberFriendship isMyself true" );
		bigListInfo->setMyFriendshipToHim( eFriendStateFriend );
		bigListInfo->setHisFriendshipToMe( eFriendStateFriend );

		return true;
	}

	EFriendState prevMyFriendship = bigListInfo->getMyFriendshipToHim();
	EFriendState curMyFriendship = prevMyFriendship;
	if( prevMyFriendship == eFriendStateAnonymous )
	{
		curMyFriendship = eFriendStateGuest;
		bigListInfo->setMyFriendshipToHim( curMyFriendship );
		updateVectorList( prevMyFriendship, bigListInfo );
	}

	EFriendState prevHisFriendship = bigListInfo->getHisFriendshipToMe();
	EFriendState curHisFriendship = prevHisFriendship;
	if( prevMyFriendship == eFriendStateAnonymous )
	{
		curMyFriendship = eFriendStateGuest;
		bigListInfo->setHisFriendshipToMe( curMyFriendship );
	}

	if( prevMyFriendship != curMyFriendship || prevHisFriendship != curHisFriendship )
	{
		m_Engine.getToGui().toGuiContactAnythingChange( bigListInfo->getVxNetIdent() );
	}

	return true;
}

//============================================================================
bool BigListMgr::fromGuiDeleteUser( VxGUID& onlineId )
{
	if( onlineId == m_Engine.getMyOnlineId() )
    {
        LogMsg( LOG_ERROR, "BigListMgr::fromGuiDeleteUser cannot delete myself" );
        vx_assert( false );
        return false;
    }

	if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "BigListMgr::fromGuiDeleteUser invalid guid" );
        vx_assert( false );
        return false;
    }

	BigListInfo* bigListInfo = findBigListInfo( onlineId );
	if( !bigListInfo )
	{
        LogMsg( LOG_ERROR, "BigListMgr::fromGuiDeleteUser user not found %s", onlineId.toOnlineIdString().c_str() );
        vx_assert( false );
        return false;
	}

	bigListInfo->setMyFriendshipToHim( eFriendStateIgnore );
	bool wasDeleted = removeUserFromDatabase( onlineId ) == 0;
	return wasDeleted;
}

//============================================================================
void BigListMgr::updateVectorList( enum EFriendState oldFriendship, BigListInfo* poInfo )
{
	EFriendState newFriendship = poInfo->getMyFriendshipToHim();
	BigList::updateVectorList( oldFriendship, poInfo );

	if( newFriendship > eFriendStateGuest && oldFriendship <= eFriendStateGuest )
	{
		// became a friend we want to stay connected to
		if( poInfo->canDirectConnectToUser() )
		{
            //m_Engine.getStayConnected().addStayConnectedRequest( poInfo->getConnectInfo(), eConnectReasonStayConnected );
		}
	}
	else if( newFriendship <= eFriendStateGuest && oldFriendship > eFriendStateGuest )
	{
		// no longer a friend we want to stay connected to
        //m_Engine.getStayConnected().removeStayConnectedRequest( poInfo->getConnectInfo(), eConnectReasonStayConnected );
	}
}
