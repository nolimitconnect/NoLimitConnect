//============================================================================
// Copyright (C) 2023 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "P2PEngine.h"
#include <ptop_src/ptop_engine_src/BigListLib/BigListInfo.h>

#include <NetLib/VxSktBase.h>
#include <CoreLib/VxDebug.h>

//============================================================================
bool P2PEngine::onFirstPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    bool updateOk{ true };

    LogModule( eLogConnect, LOG_VERBOSE, "onFirstPktAnnounce %s %s at ip %s",
               bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );
    std::string debugUser{ "A8 tab" };
    if( pktAnn->getOnlineName() == debugUser )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "onFirstPktAnnounce %s %s at ip %s pktAnn his friendship %s bigList my friendship %s his friendship %s",
                   bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str(),
                   DescribeFriendState( pktAnn->getHisFriendshipToMe() ),
                   DescribeFriendState( bigListInfo->getMyFriendshipToHim() ),
                   DescribeFriendState( bigListInfo->getHisFriendshipToMe() )
                   );
    }

    if( !sktBase->getIsPeerPktAnnSet() )
    {
        if( sktBase->setPeerPktAnn( *pktAnn ) )
        {
            getConnectList().addConnection( sktBase, bigListInfo, (ePktAnnUpdateTypeNewContact == pktAnnUpdateType) );
            getConnectionMgr().onSktConnectedWithPktAnn( sktBase, bigListInfo );
        }
        else
        {
            getConnectList().addConnection( sktBase, bigListInfo, (ePktAnnUpdateTypeNewContact == pktAnnUpdateType) );
        }
    }

    return updateOk && onPktAnnounceCommonHandler( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
}

//============================================================================
bool P2PEngine::onConnectionPktAnnounceUpdated( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    // the updates to user should have been done in m_BigListMgr.updatePktAnn
    LogModule( eLogConnect, LOG_VERBOSE, "onConnectionPktAnnounceUpdated %s %s at ip %s",
               bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );
    
    // update the pkt ann set into the connection
    sktBase->setPeerPktAnn( *pktAnn ); 

    // again request thumbs in case they have changed
    getThumbMgr().requestThumbs( sktBase, bigListInfo->getVxNetIdent() );

    return true;
}

//============================================================================
bool P2PEngine::onHostedUserPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    bool updateOk{ true };

    LogModule( eLogConnect, LOG_VERBOSE, "onHostedUserPktAnnounce %s %s at ip %s",
               bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );

    return updateOk && onPktAnnounceCommonHandler( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
}

//============================================================================
bool P2PEngine::onUnexpectedPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    bool updateOk{ true };


    return updateOk;
}

//============================================================================
bool P2PEngine::onPktAnnounceCommonHandler( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    bool updateOk{ true };
    
    if( !sktBase->isTempConnection() )
	{
        
        LogModule( eLogConnect, LOG_VERBOSE, "onPktAnnounceCommonHandler %s %s at ip %s my frienship %s his friendship %s",
               bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str(),
               DescribeFriendState( bigListInfo->getMyFriendshipToHim() ), DescribeFriendState( bigListInfo->getHisFriendshipToMe() ) );

        getConnectIdListMgr().pktAnnRecieved( sktBase->getSocketId(), pktAnn->getMyOnlineId() );
		getToGui().toGuiContactAdded( bigListInfo->getVxNetIdent() );
        getThumbMgr().requestThumbs( sktBase, bigListInfo->getVxNetIdent() );
	}	
   
    return updateOk;
}
