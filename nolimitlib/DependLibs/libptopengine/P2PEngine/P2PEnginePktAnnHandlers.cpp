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

#include <BigListLib/BigListInfo.h>
#include <Membership/MemberActiveMgr.h>
#include <Membership/MemberConfirmMgr.h>
#include <SendQueue/SendQueueMgr.h>

#include <NetLib/VxSktBase.h>
#include <CoreLib/VxDebug.h>

//============================================================================
bool P2PEngine::onFirstPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, enum EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    pktAnn->clearIsJoined();
    if( pktAnn->getMyOnlineId() == getMyOnlineId() )
    {
        VxReportHack( eHackerLevelSevere, eHackerReasonPktOnlineIdMeFromAnotherIp, sktBase, "P2PEngine::%s", __func__ );
        sktBase->closeSkt( eSktCloseHackLevelSevere );
        return false;
    }

    bool updateOk{ true };

    LogModule( eLogUserConnect, LOG_VERBOSE, "%s name %s %s at ip %s pktAnn his friendship %s my friendship %s", __func__,
                bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str(),
                DescribeFriendState( pktAnn->getHisFriendshipToMe() ),
                DescribeFriendState( bigListInfo->getMyFriendshipToHim() ) );


    if( !sktBase->getIsPeerPktAnnSet() )
    {
        LogModule( eLogUserConnect, LOG_VERBOSE, "%s set peer %s", __func__, pktAnn->describeUser().c_str() );

        if( sktBase->setPeerPktAnn( *pktAnn ) )
        {
            if( pktAnn->getIsPktAnnTempConnection() )
            {
                sktBase->setIsTempConnection( true );
            }

            if( !sktBase->isTempConnection() )
            {

                GroupieId groupieId( bigListInfo->getMyOnlineId(), bigListInfo->getMyOnlineId(), eHostTypePeerUser );
                getConnectIdListMgr().addConnection( sktBase->getSocketId(), groupieId, false );
                getConnectList().addConnection( sktBase, bigListInfo, (ePktAnnUpdateTypeNewContact == pktAnnUpdateType) );
            }

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
bool P2PEngine::onConnectionPktAnnounceUpdated( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, enum EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    if( pktAnn->getMyOnlineId() == getMyOnlineId() )
    {
        VxReportHack( eHackerLevelSevere, eHackerReasonPktOnlineIdMeFromAnotherIp, sktBase, "P2PEngine::onConnectionPktAnnounceUpdated" );
        sktBase->closeSkt( eSktCloseHackLevelSevere );
        return false;
    }

    // the updates to user should have been done in m_BigListMgr.updatePktAnn
    LogModule( eLogConnect, LOG_VERBOSE, "%s %s %s at ip %s", __func__,
               bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );
    
    // update the pkt ann set into the connection
    sktBase->setPeerPktAnn( *pktAnn ); 

    // again request thumbs in case they have changed
    getThumbMgr().requestThumbs( sktBase, bigListInfo->getVxNetIdent() );
    // tell gui the user has updated

    return true;
}

//============================================================================
bool P2PEngine::onHostedUserPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, enum EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    if( pktAnn->getMyOnlineId() == getMyOnlineId() )
    {
        VxReportHack( eHackerLevelSevere, eHackerReasonPktOnlineIdMeFromAnotherIp, sktBase, "P2PEngine::onHostedUserPktAnnounce" );
        sktBase->closeSkt( eSktCloseHackLevelSevere );
        return false;
    }

    bool updateOk{ true };

    GroupieId groupieId( pktAnn->getMyOnlineId(), pktAnn->getHostOnlineId(), pktAnn->getHostType() );
    if( !sktBase->isTempConnection() )
    {
        pktAnn->setIsJoined( pktAnn->getHostType(), true );

        LogModule( eLogConnect, LOG_VERBOSE, "onHostedUserPktAnnounce %s %s at ip %s",
                   bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );
        LogModule( eLogUserEvent, LOG_VERBOSE, "onHostedUserPktAnnounce %s %s", pktAnn->describeUser().c_str(), describeGroupieId( groupieId ).c_str() );
    }

    getConnectIdListMgr().addConnection( sktBase->getSocketId(), groupieId, true );
    getMemberActiveMgr().updateMemberActive( groupieId, true );

    if( !getConnectIdListMgr().isDirectConnected( pktAnn->getMyOnlineId() ) )
    {
        GetMemberConfirmMgr().addMemberConfirm( sktBase, pktAnn->getMyOnlineId() );
    }

    return updateOk && onPktAnnounceCommonHandler( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
}

//============================================================================
bool P2PEngine::onRelayedUserPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, enum EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    if( pktAnn->getMyOnlineId() == getMyOnlineId() )
    {
        VxReportHack( eHackerLevelSevere, eHackerReasonPktOnlineIdMeFromAnotherIp, sktBase, "P2PEngine::onRelayedUserPktAnnounce" );
        sktBase->closeSkt( eSktCloseHackLevelSevere );
        return false;
    }

    if( !sktBase->isTempConnection() )
    {
        // the updates to user should have been done in m_BigListMgr.updatePktAnn
        LogModule( eLogConnect, LOG_VERBOSE, "onRelayedUserPktAnnounce %s %s at ip %s",
                   bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );

        LogModule( eLogUserEvent, LOG_VERBOSE, "onRelayedUserPktAnnounce %s from %s", pktAnn->describeUser().c_str(), bigListInfo->describeUser().c_str() );
    }

    // TODO determine host type so can add hosted connection
    //GroupieId groupieId( pktAnn->getMyOnlineId(), pktAnn->getHostOnlineId(), pktAnn->getHostType() );
    //getConnectIdListMgr().addConnection( sktBase->getSocketId(), groupieId, true );

    // again request thumbs in case they have changed
    getThumbMgr().requestThumbs( sktBase, bigListInfo->getVxNetIdent() );

    GetMemberConfirmMgr().pktAnnRecieved( pktAnn->getMyOnlineId() );

    return onPktAnnounceCommonHandler( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
}

//============================================================================
bool P2PEngine::onUnexpectedPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, enum EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
{
    if( pktAnn->getMyOnlineId() == getMyOnlineId() )
    {
        VxReportHack( eHackerLevelSevere, eHackerReasonPktOnlineIdMeFromAnotherIp, sktBase, "P2PEngine::onUnexpectedPktAnnounce" );
        sktBase->closeSkt( eSktCloseHackLevelSevere );
        return false;
    }

    bool updateOk{ true };

    LogModule( eLogConnect, LOG_ERROR, "onUnexpectedPktAnnounce %s %s at ip %s",
               bigListInfo->getOnlineName(), bigListInfo->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );
    return updateOk;
}

//============================================================================
bool P2PEngine::onPktAnnounceCommonHandler( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, enum EPktAnnUpdateType pktAnnUpdateType, BigListInfo* bigListInfo )
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

        getSendQueueMgr().pktAnnRecieved( pktAnn->getMyOnlineId() );
	}	
   
    return updateOk;
}
