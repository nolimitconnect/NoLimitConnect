//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBase.h"

#include "P2PSession.h"
#include "PluginMgr.h"
#include "RxSession.h"
#include "TxSession.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxPtopUrl.h>
#include <CoreLib/VxTime.h>

#include <PktLib/PktsHostInvite.h>
#include <PktLib/PktsPluginOffer.h>

#include <NetLib/VxSktBase.h>

//============================================================================
std::string PluginBase::getThumbXferDbName( EPluginType pluginType )
{
    std::string thumbXferName( "ThumbXferDb" );
    thumbXferName += std::to_string( (int)pluginType );
    return thumbXferName + ".db3";
}

//============================================================================
std::string PluginBase::getThumbXferThreadName( EPluginType pluginType )
{
    std::string thumbXferName( "ThumbXferDb" );
    thumbXferName += std::to_string( (int)pluginType );
    return thumbXferName + ".db3";
}

//============================================================================
PluginBase::PluginBase( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PktPluginHandlerBase()
, m_ePluginType( pluginType )
, m_Engine( engine )
, m_PluginMgr( pluginMgr )
, m_MyIdent( myIdent )
, m_PluginMutex()
, m_AssetMgr( engine.getAssetMgr() )
, m_ThumbMgr( engine.getThumbMgr() )
, m_ThumbXferInterface( *this )
, m_ThumbXferMgr( engine, engine.getThumbMgr(), m_ThumbXferInterface )
{
    m_PluginSetting.setPluginType( pluginType );
}

//============================================================================
IToGui& PluginBase::getToGui()
{ 
    return m_Engine.getToGui(); 
}

//============================================================================
void PluginBase::setPluginType( EPluginType pluginType )
{
    m_ePluginType = pluginType;
    m_PluginSetting.setPluginType( pluginType );
}

//============================================================================
void PluginBase::pluginStartup( void )
{
    if( getPluginType() != ePluginTypeInvalid )
    {
        m_Engine.getPluginSetting( getPluginType(), m_PluginSetting );
    }
}

//============================================================================
bool PluginBase::setPluginSetting( PluginSetting& pluginSetting, int64_t modifiedTimeMs )
{
    m_PluginSetting = pluginSetting;
    onPluginSettingChange( m_PluginSetting, modifiedTimeMs );

    return true;
}

//============================================================================
EHostType PluginBase::getHostType( void )
{
    EHostType hostType = eHostTypeUnknown;
    switch( getPluginType() )
    {
    case ePluginTypeClientChatRoom:
    case ePluginTypeHostChatRoom:
        hostType = eHostTypeChatRoom;
        break;

    case ePluginTypeClientConnectTest:
    case ePluginTypeHostConnectTest:
        hostType = eHostTypeConnectTest;
        break;

    case ePluginTypeClientGroup:
    case ePluginTypeHostGroup:
        hostType = eHostTypeGroup;
        break;

    case ePluginTypeClientRandomConnect:
    case ePluginTypeHostRandomConnect:
        hostType = eHostTypeRandomConnect;
        break;

    case ePluginTypeClientNetwork:
    case ePluginTypeHostNetwork:
    case ePluginTypeNetworkSearchList:
        hostType = eHostTypeNetwork;
        break;

    default:
        break;
    }

    return hostType;
}

//============================================================================
EPluginType	PluginBase::getClientPluginType( void )
{
    EPluginType clientPluginType = getPluginType();
    switch( clientPluginType )
    {

    case ePluginTypeAboutMePageServer:
        clientPluginType = ePluginTypeAboutMePageClient;
        break;

    case ePluginTypeCamServer:
        clientPluginType = ePluginTypeCamClient;
        break;

    case ePluginTypeFileShareServer:
        clientPluginType = ePluginTypeFileShareClient;
        break;

    case ePluginTypeHostChatRoom:
        clientPluginType = ePluginTypeClientChatRoom;
        break;

    case ePluginTypeHostConnectTest:
        clientPluginType = ePluginTypeClientConnectTest;
        break;

    case ePluginTypeHostGroup:
        clientPluginType = ePluginTypeClientGroup;
        break;

    case ePluginTypeHostRandomConnect:
        clientPluginType = ePluginTypeClientRandomConnect;
        break;

    case ePluginTypeStoryboardServer:
        clientPluginType = ePluginTypeStoryboardClient;
        break;

    default:
        break;
    }

    return clientPluginType;
}

//============================================================================
EPluginType	PluginBase::getServerPluginType( void )
{
    EPluginType serverPluginType = getPluginType();
    switch( serverPluginType )
    {

    case ePluginTypeAboutMePageClient:
        serverPluginType = ePluginTypeAboutMePageServer;
        break;

    case ePluginTypeCamClient:
        serverPluginType = ePluginTypeCamServer;
        break;

    case ePluginTypeClientChatRoom:
        serverPluginType = ePluginTypeHostChatRoom;
        break;

    case ePluginTypeClientConnectTest:
        serverPluginType = ePluginTypeHostConnectTest;
        break;

    case ePluginTypeClientGroup:
        serverPluginType = ePluginTypeHostGroup;
        break;

    case ePluginTypeClientRandomConnect:
        serverPluginType = ePluginTypeHostRandomConnect;
        break;

    case ePluginTypeFileShareClient:
        serverPluginType = ePluginTypeFileShareServer;
        break;

    case ePluginTypeStoryboardClient:
        serverPluginType = ePluginTypeStoryboardServer;
        break;

    default:
        break;
    }

    return serverPluginType;
}

//============================================================================
EAppState PluginBase::getPluginState( void )
{
	if( eFriendStateIgnore == getPluginPermission() )
	{
		return eAppStatePermissionErr;
	}

	return m_ePluginState;
}

//============================================================================
EFriendState PluginBase::getPluginPermission( void )
{ 
	return m_Engine.getMyPktAnnounce().getPluginPermission( m_ePluginType ); 
}

//============================================================================
void PluginBase::setPluginPermission( EFriendState eFriendState )		
{ 
	EFriendState prevState = m_Engine.getMyPktAnnounce().getPluginPermission( m_ePluginType ); 
	if( prevState != eFriendState )
	{
		m_Engine.getMyPktAnnounce().setPluginPermission( m_ePluginType, eFriendState );
		m_Engine.doPktAnnHasChanged( false );
	}
};

//============================================================================
bool PluginBase::isAccessAllowed( VxNetIdent* netIdent, bool logAccessError, const char* accessReason )
{
    if( IsClientPluginType( getPluginType() ) )
    {
        // clients make requests
        return true;
    }

	EFriendState curPermission = m_Engine.getMyPktAnnounce().getPluginPermission( m_ePluginType ); 
    if( eFriendStateIgnore != curPermission && eFriendStateIgnore != netIdent->getMyFriendshipToHim() )
    {
        if( netIdent->getMyFriendshipToHim() >= curPermission || netIdent->getMyOnlineId() == m_Engine.getMyOnlineId() )
        {
            return true;
        }

        if( netIdent->getMyFriendshipToHim() == eFriendStateAnonymous && m_Engine.isMemberGuest( getPluginType(), netIdent->getMyOnlineId() ) &&
            eFriendStateGuest >= curPermission )
        {
            // has elevated to guest permission
            return true;
        }
    }

    if( logAccessError )
    {
        if( accessReason )
        {
            LogMsg( LOG_WARN, "%s %s Access Not Allowed to %s %s %s", DescribePluginType( getPluginType() ), accessReason,
                netIdent->getMyOnlineId().describeVxGUID().c_str(), DescribeFriendState( netIdent->getMyFriendshipToHim() ), netIdent->getOnlineName() );
        }
        else
        {
            LogMsg( LOG_WARN, "%s Access Not Allowed to %s %s %s", DescribePluginType( getPluginType() ),
                netIdent->getMyOnlineId().describeVxGUID().c_str(), DescribeFriendState( netIdent->getMyFriendshipToHim() ), netIdent->getOnlineName() );
        }
    }

	return false;
}

//============================================================================
bool PluginBase::isPluginEnabled( void )
{
	return ( eFriendStateIgnore != getPluginPermission() );
}

//============================================================================
void PluginBase::onSessionStart( PluginSessionBase* poSession, bool pluginIsLocked )
{
	m_Engine.onSessionStart( poSession->getPluginType(), poSession->getSendToId() );
}

//============================================================================
bool PluginBase::txPacket( VxGUID onlineId, std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* poPkt, EPluginType overridePlugin )
{
    if( nullptr == sktBase )
    {
        LogMsg( LOG_WARN, "PluginBase::txPacket NULL sktBase" );
        return false;
    }

    return m_PluginMgr.pluginApiTxPacket( m_ePluginType, onlineId, sktBase, poPkt, overridePlugin );
}

//============================================================================
void PluginBase::fromGuiGetFileShareSettings( FileShareSettings& fileShareSettings )
{
}

//============================================================================
void PluginBase::fromGuiSetFileShareSettings( FileShareSettings& fileShareSettings )
{
}

//============================================================================
EXferError PluginBase::fromGuiFileXferControl( VxGUID& onlineId, EXferAction xferAction, FileInfo& fileInfo )
{
	return eXferErrorBadParam;
}

//============================================================================ 
bool PluginBase::fromGuiInstMsg( VxGUID& onlineId, const char* pMsg )
{
	return false;
}

//============================================================================ 
bool PluginBase::fromGuiPushToTalk( VxGUID& onlineId, bool enableTalk )
{
    return false;
}

//============================================================================ 
void PluginBase::makeShortFileName( const char* pFullFileName, std::string& strShortFileName )
{
    VxFileUtil::makeShortFileName( pFullFileName, strShortFileName );
}

//============================================================================ 
EPluginAccess PluginBase::canAcceptNewSession( VxNetIdent* netIdent ) 
{ 
    return netIdent->getHisAccessPermissionFromMe( m_ePluginType ); 
}

//============================================================================ 
P2PSession* PluginBase::createP2PSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
    P2PSession* p2pSession = new P2PSession( sktBase, onlineId, m_ePluginType );
	p2pSession->setPluginType( m_ePluginType );
	return p2pSession;
}

//============================================================================ 
P2PSession* PluginBase::createP2PSession( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
    P2PSession* p2pSession = new P2PSession( lclSessionId, sktBase, onlineId, m_ePluginType );
	p2pSession->setPluginType( m_ePluginType );
	return p2pSession;
}

//============================================================================ 
RxSession *	PluginBase::createRxSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
    RxSession * rxSession = new RxSession( sktBase, onlineId, m_ePluginType );
	rxSession->setPluginType( m_ePluginType );
	return rxSession;
}

//============================================================================ 
RxSession *	PluginBase::createRxSession( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
    RxSession * rxSession =  new RxSession( lclSessionId, sktBase, onlineId, m_ePluginType );
	rxSession->setPluginType( m_ePluginType );
	return rxSession;
}

//============================================================================ 
TxSession *	PluginBase::createTxSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
    TxSession * txSession = new TxSession( sktBase, onlineId, m_ePluginType );
	txSession->setPluginType( m_ePluginType );
	return txSession;
}

//============================================================================ 
TxSession *	PluginBase::createTxSession( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
    TxSession * txSession = new TxSession( lclSessionId, sktBase, onlineId, m_ePluginType );
	txSession->setPluginType( m_ePluginType );
	return txSession;
}

//============================================================================ 
void PluginBase::onAppStartup( void )
{
}

//============================================================================ 
void PluginBase::onAppShutdown( void )
{
}

//============================================================================ 
EPluginType PluginBase::getDestinationPluginOverride( EHostType hostType )
{
    if( eHostTypeNetwork == hostType )
    {
        // no matter which plugin we send from if the destination host is network host
        // the always set the packet plugin destination to network host
        if( ePluginTypeHostNetwork == m_ePluginType )
        {
            LogMsg( LOG_ERROR, "Tried to send to host %s from plugin %s", DescribeHostType( hostType ), DescribePluginType( m_ePluginType ) );
            vx_assert( false );
            return ePluginTypeInvalid;
        }

        return ePluginTypeHostNetwork;
    }

    if( eHostTypePeerUser == hostType )
    {
        // directly connected peer users can only access peer type plugins (Not Client/Host)
        return ePluginTypeInvalid;
    }

    if( eHostTypeGroup == hostType )
    {
        switch( m_ePluginType )
        {
        case ePluginTypeClientGroup:
            return ePluginTypeHostGroup;
        case ePluginTypeHostGroup:
            return ePluginTypeClientGroup;
        default:
            LogMsg( LOG_ERROR, "Tried to send to host %s from plugin %s", DescribeHostType( hostType ), DescribePluginType( m_ePluginType ) );
            vx_assert( false );
            return ePluginTypeInvalid;
        }
    }

    if( eHostTypeChatRoom == hostType )
    {
        switch( m_ePluginType )
        {
        case ePluginTypeClientChatRoom:
            return ePluginTypeHostChatRoom;
        case ePluginTypeHostChatRoom:
            return ePluginTypeClientChatRoom;
        default:
            LogMsg( LOG_ERROR, "Tried to send to host %s from plugin %s", DescribeHostType( hostType ), DescribePluginType( m_ePluginType ) );
            vx_assert( false );
            return ePluginTypeInvalid;
        }
    }

    if( eHostTypeRandomConnect == hostType )
    {
        switch( m_ePluginType )
        {
        case ePluginTypeClientRandomConnect:
            return ePluginTypeHostRandomConnect;
        case ePluginTypeHostRandomConnect:
            return ePluginTypeClientRandomConnect;
        default:
            LogMsg( LOG_ERROR, "Tried to send to host %s from plugin %s", DescribeHostType( hostType ), DescribePluginType( m_ePluginType ) );
            vx_assert( false );
            return ePluginTypeInvalid;
        }
    }

    if( eHostTypeConnectTest == hostType )
    {
        switch( m_ePluginType )
        {
        case ePluginTypeClientConnectTest:
            return ePluginTypeHostConnectTest;
        case ePluginTypeHostConnectTest:
            return ePluginTypeClientConnectTest;
        default:
            LogMsg( LOG_ERROR, "Tried to send to host %s from plugin %s", DescribeHostType( hostType ), DescribePluginType( m_ePluginType ) );
            vx_assert( false );
            return ePluginTypeInvalid;
        }
    }

    LogMsg( LOG_ERROR, "Tried to send to Unknown Host Type %d from plugin %s", hostType, DescribePluginType( m_ePluginType ) );
    vx_assert( false );
    return ePluginTypeInvalid;
}

//============================================================================
EPluginAccess PluginBase::getPluginAccessState( VxNetIdent* netIdent )
{
    EPluginAccess pluginAccess = ePluginAccessNotSet;

    EFriendState pluginState = m_MyIdent->getPluginPermission( getPluginType() );
    if( eFriendStateIgnore == pluginState )
    {
        // we are not enabled
        pluginAccess = ePluginAccessDisabled;
    }
    else
    {
        if( netIdent->isIgnored() )
        {
            pluginAccess = ePluginAccessIgnored;
        }
        else
        {
            EFriendState friendState = netIdent->getMyFriendshipToHim();
            // everybody gets at least guest permission
            if( m_Engine.getConnectIdListMgr().isHosted( netIdent->getMyOnlineId() ) && friendState == eFriendStateAnonymous )
            {
                friendState = eFriendStateGuest;
            }

            if( friendState < pluginState )
            {
                // not enough permission
                pluginAccess = ePluginAccessLocked;
            }
            else
            {
                pluginAccess = ePluginAccessOk;
            }
        }
    }

    return pluginAccess;
}

//============================================================================
void PluginBase::logCommError( ECommErr commErr, const char* desc, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    LogMsg( LOG_ERROR, "%s %s from %s %s", desc, DescribeCommError( commErr ), netIdent->getOnlineName(), sktBase->describeSktConnection().c_str() );
}

//============================================================================
ECommErr PluginBase::getCommAccessState( VxNetIdent* netIdent )
{
    ECommErr commErr{ eCommErrNone };
    if( !isPluginEnabled() )
    {
        commErr = eCommErrPluginNotEnabled;
    }
    else
    {
        EPluginAccess pluginAccess = getPluginAccessState( netIdent );
        if( ePluginAccessOk != pluginAccess )
        {
            commErr = eCommErrPluginPermission;
        }
    }

    return commErr;
}

//============================================================================
void PluginBase::toGuiFileUploadStart( VxGUID& onlineId, VxGUID& lclSessionId, FileInfo& fileInfo )
{
    m_Engine.getToGui().toGuiFileUploadStart( onlineId, getPluginType(), lclSessionId, fileInfo );
}

//============================================================================
void PluginBase::toGuiFileDownloadStart( VxGUID& onlineId, VxGUID& lclSessionId, FileInfo& fileInfo )
{
    m_Engine.getToGui().toGuiFileDownloadStart( onlineId, getPluginType(), lclSessionId, fileInfo );
}

//============================================================================
void PluginBase::toGuiFileXferState( VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param )
{
    m_Engine.getToGui().toGuiFileXferState( getPluginType(), lclSessionId, xferDir, xferState, xferErr, param );
}

//============================================================================
void PluginBase::toGuiFileDownloadComplete( VxGUID& lclSessionId, std::string& fileName, EXferError xferError )
{
    m_Engine.getToGui().toGuiFileDownloadComplete( getPluginType(), lclSessionId, fileName, xferError );
}

//============================================================================
void PluginBase::toGuiFileUploadComplete( VxGUID& lclSessionId, std::string& fileName, EXferError xferError )
{
    m_Engine.getToGui().toGuiFileUploadComplete( getPluginType(), lclSessionId, fileName, xferError );
}

//============================================================================
void PluginBase::handleToGuiOfferRequest( VxNetIdent* netIdent, PktPluginOfferReq* pktReq )
{
    OfferBaseInfo offerInfo;
    if( offerInfo.extractFromBlob( pktReq->getBlobEntry() ) )
    {
        IToGui::getIToGui().toGuiRxedPluginOffer( netIdent->getMyOnlineId(), offerInfo);
    }
}

//============================================================================
void PluginBase::handleToGuiOfferResponse( VxNetIdent* netIdent, PktPluginOfferReply* pktReply )
{
    OfferBaseInfo offerInfo;
    if( offerInfo.extractFromBlob( pktReply->getBlobEntry() ) )
    {
        IToGui::getIToGui().toGuiRxedOfferReply( netIdent->getMyOnlineId(), offerInfo );
    }
}

//============================================================================
void PluginBase::handlePktHostInviteSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktHostInviteSearchReply* pktReply = (PktHostInviteSearchReply*)pktHdr;
    if( pktReply->getCommError() )
    {
        LogModule( eLogHostSearch, LOG_DEBUG, "PluginBase::onPktHostInviteSearchReply comm err %s", DescribeCommError( pktReply->getCommError() ) );
        logCommError( pktReply->getCommError(), "PktHostInviteSearchReply", sktBase, netIdent );
        m_Engine.getHostedListMgr().hostSearchCompleted( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, pktReply->getCommError() );
    }
    else
    {
        updateFromHostInviteSearchBlob( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, pktReply->getBlobEntry(), pktReply->getInviteCountThisPkt() );

        if( pktReply->getMoreInvitesExist() )
        {
            if( !requestMoreHostInvitesFromNetworkHost( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, pktReply->getNextSearchOnlineId() ) )
            {
                m_Engine.getHostedListMgr().hostSearchCompleted( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, eCommErrNone );
            }
        }
        else
        {
            m_Engine.getHostedListMgr().hostSearchCompleted( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, eCommErrNone );
        }
    }
}

//============================================================================
void PluginBase::handlePktHostInviteMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktHostInviteMoreReply* pktReply = (PktHostInviteMoreReply*)pktHdr;
    if( pktReply->getCommError() )
    {
        logCommError( pktReply->getCommError(), "PktHostInviteSearchReply", sktBase, netIdent );
        m_Engine.getHostedListMgr().hostSearchCompleted( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, pktReply->getCommError() );
    }
    else
    {
        LogModule( eLogHostSearch, LOG_DEBUG, "PluginBase::onPktHostInviteMoreReply %d hosts", pktReply->getInviteCountThisPkt() );

        updateFromHostInviteSearchBlob( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, pktReply->getBlobEntry(), pktReply->getInviteCountThisPkt() );
        if( pktReply->getMoreInvitesExist() )
        {
            if( !requestMoreHostInvitesFromNetworkHost( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, pktReply->getNextSearchOnlineId() ) )
            {
                m_Engine.getHostedListMgr().hostSearchCompleted( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, eCommErrNone );
            }
        }
        else
        {
            m_Engine.getHostedListMgr().hostSearchCompleted( pktReply->getHostType(), pktReply->getSearchSessionId(), sktBase, netIdent, eCommErrNone );
        }
    }
}

//============================================================================
void PluginBase::updateFromHostInviteSearchBlob( EHostType hostType, VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, PktBlobEntry& blobEntry, int hostInfoCount )
{
    LogModule( eLogHostSearch, LOG_DEBUG, "PluginBase::updateFromHostInviteSearchBlob rxed %d hosts", hostInfoCount );
    blobEntry.resetRead();
    for( int i = 0; i < hostInfoCount; i++ )
    {
        HostedInfo hostedInfo;
        if( hostedInfo.extractFromSearchBlob( blobEntry ) )
        {
            if( eHostTypeUnknown == hostedInfo.getHostType() && eHostTypeUnknown != hostType )
            {
                hostedInfo.setHostType( hostType );
            }

            if( netIdent->getMyOnlineId() == hostedInfo.getAdminOnlineId() )
            {
                // handle the case where network host is also hosting a group
                // normally the identity is never sent to the gui because this is a temporary connection
                // but gui needs the identity to show host list
                if( sktBase->isTempConnection() )
                {
                    m_Engine.getToGui().toGuiContactAnythingChange( netIdent );
                }
            }
            else
            {
                assureIdentityExist( hostedInfo );
            }

            m_Engine.getHostedListMgr().hostSearchResult( hostType, searchSessionId, sktBase, netIdent, hostedInfo );
        }
        else
        {
            LogMsg( LOG_ERROR, "Could not extract HostInviteSearchBlob" );
            break;
        }
    }
}

//============================================================================
bool PluginBase::requestMoreHostInvitesFromNetworkHost( EHostType hostType, VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, VxGUID& nextHostOnlineId )
{
    LogModule( eLogHostSearch, LOG_DEBUG, "PluginBase::requestMoreHostInvitesFromNetworkHost" );
    PktHostInviteMoreReq pktReq;
    pktReq.setHostType( hostType );
    pktReq.setSearchSessionId( searchSessionId );
    pktReq.setNextSearchOnlineId( nextHostOnlineId );
    pktReq.setPluginNum( (uint8_t)ePluginTypeHostNetwork );
    return txPacket( netIdent->getMyOnlineId(), sktBase, &pktReq, ePluginTypeHostNetwork );
}

//============================================================================
EPluginType PluginBase::getAssetOverridePluginType( void )
{
    // assets are normally only transfered between clients
    return HostPluginToClientPluginType( getPluginType() );
}

//============================================================================
bool PluginBase::assureIdentityExist( HostedInfo& hostedInfo )
{
    VxNetIdent* netIdent = m_Engine.getBigListMgr().findNetIdent( hostedInfo.getAdminOnlineId() );
    if( netIdent )
    {
        return true;
    }

    // for the case of host listing but user has not connected to host and does not have any identity info
    // create a temporary identity with minimal permissions for gui to use until pkt announce recieved
    bool hasIdent{ false };
    if( hostedInfo.getAdminOnlineId().isVxGUIDValid() )
    {
        // create a temporary identity until a connection is made
        VxNetIdent tempIdent;

        bool ipv6{ false };
        std::string url = hostedInfo.getHostInviteUrl();
        VxPtopUrl ptopUrl( url );
        if( ptopUrl.isValid() )
        {
            VxGUID ptopOnlineId = ptopUrl.getOnlineId();
            if( ptopOnlineId != hostedInfo.getAdminOnlineId() )
            {
                LogMsg( LOG_ERROR, "PluginBase::%s admin id %s does not match url id %s", __func__,
                        hostedInfo.getAdminOnlineId().toOnlineIdString().c_str(),
                        ptopOnlineId.toOnlineIdString().c_str() );
                return false;
            }

            tempIdent.setOnlinePort( ptopUrl.getPort() );
            tempIdent.setOnlineIpAddress( ptopUrl.getHost() );
            tempIdent.setMyOnlineId( ptopUrl.getOnlineId() );

            tempIdent.setOnlineName( hostedInfo.getHostTitle().c_str());
            tempIdent.setOnlineDescription( hostedInfo.getHostDescription().c_str() );
            tempIdent.setMyFriendshipToHim( eFriendStateAnonymous );
            tempIdent.setHisFriendshipToMe( eFriendStateAnonymous );
            tempIdent.setAvatarGuid( hostedInfo.getThumbId(), GetGmtTimeMs() );
            if( IsHostARelayForUsers( ptopUrl.getHostType() ) )
            {
                tempIdent.setPluginPermission( HostTypeToHostPlugin( ptopUrl.getHostType() ), eFriendStateAnonymous );
                m_Engine.getToGui().toGuiContactAnythingChange( &tempIdent );
                hasIdent = true;

                vx_assert( tempIdent.isValidNetIdent() );
            }
        }
    }

    if( !hasIdent )
    {
        LogMsg( LOG_ERROR, "PluginBase::%s could not create host temporary identity" );
    }

    return hasIdent;
}
