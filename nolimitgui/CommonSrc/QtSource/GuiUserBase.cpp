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

#include "GuiUserBase.h"

#include "AppCommon.h"
#include "GuiUserMgr.h"
#include "GuiParams.h"

#include <CoreLib/VxTime.h>

//============================================================================
GuiUserBase::GuiUserBase( AppCommon& app )
    : QWidget( &app )
    , m_MyApp( app )
{
}

//============================================================================
GuiUserBase::GuiUserBase( AppCommon& app, VxNetIdent* netIdent, VxGUID& sessionId )
    : QWidget( &app )
    , m_MyApp( app )
    , m_NetIdent( *netIdent )
    , m_OnlineId( netIdent->getMyOnlineId() )
    , m_SessionId( sessionId )
{
}


//============================================================================
GuiUserBase::GuiUserBase( const GuiUserBase& rhs )
    : QWidget()
    , m_MyApp( rhs.m_MyApp )
    , m_NetIdent( rhs.m_NetIdent )
    , m_OnlineId( rhs.m_OnlineId )
    , m_SessionId( rhs.m_SessionId )
    , m_NearbyTimeOrZero( rhs.m_NearbyTimeOrZero )
    , m_IsDirectConnect( rhs.m_IsDirectConnect )
    , m_IsRelayed( rhs.m_IsRelayed )
    , m_HostSet( rhs.m_HostSet )
{
}

//============================================================================
bool GuiUserBase::isMyself( void )
{
    return getMyOnlineId() == m_MyApp.getMyOnlineId();
}

//============================================================================
bool GuiUserBase::isOnline( void )
{
    return m_MyApp.getUserMgr().isUserOnline( m_OnlineId );
}

//============================================================================
bool GuiUserBase::updateIsNearby( void )
{
    if( !isMyself() )
    {
        m_NearbyTimeOrZero = m_MyApp.getConnectIdListMgr().isNearbyTime( getMyOnlineId() );
    }

    return isNearby();
}

//============================================================================
bool GuiUserBase::setNearbyStatus( int64_t nearbyTimeOrZeroIfNotd ) // return false if nearbyTime is zero
{
    m_NearbyTimeOrZero = nearbyTimeOrZeroIfNotd;
    return isNearby();
}

//============================================================================
bool GuiUserBase::isNearby( void )
{
    return m_NearbyTimeOrZero && m_MyApp.elapsedMilliseconds() - m_NearbyTimeOrZero < NEARBY_TIMEOUT_MS;
}

//============================================================================
bool GuiUserBase::updateIsDirectConnect( void )
{
    bool isDirectConnect = false;
    if( isMyself() )
    {
        isDirectConnect = true;
    }
    else
    {
        isDirectConnect = m_MyApp.getConnectIdListMgr().isDirectConnect( getMyOnlineId() );
    }

    m_IsDirectConnect = isDirectConnect;
    return m_IsDirectConnect;
}

//============================================================================
bool GuiUserBase::setDirectConnectStatus( bool isDirectConnect ) // return false if nearbyTime is zero
{
    m_IsDirectConnect = isDirectConnect;
    return m_IsDirectConnect;
}

//============================================================================
bool GuiUserBase::updateIsRelayed( void )
{
    bool isRelayed = false;
    if( isMyself() )
    {
        isRelayed = false;
    }
    else
    {
        isRelayed = m_MyApp.getConnectIdListMgr().isRelayed( getMyOnlineId() );
    }

    m_IsRelayed = isRelayed;
    return isRelayed;
}

//============================================================================
EPluginAccess GuiUserBase::getMyAccessPermissionFromHim( EPluginType pluginType, bool inGroup )
{ 
    if( m_MyApp.getMyOnlineId() == m_NetIdent.getMyOnlineId() )
    {
        return ePluginAccessOk;
    }

    // cannot override a object that is sent over the network so this is mostly a copy of NetIdent::getMyAccessPermissionFromHim
    // this is so that do not have to constantly update the net ident online state
	EFriendState friendState = getHisFriendshipToMe();
	if( inGroup && friendState == eFriendStateAnonymous )
	{
		friendState = eFriendStateGuest;
	}

	EPluginAccess accessState = m_NetIdent.getPluginAccessState( pluginType, friendState );
	if( ePluginAccessOk == accessState )
	{
		if( ( ePluginTypeFileShareServer == pluginType ) 
			&& ( false == m_NetIdent.hasSharedFiles() ) )
		{
			// no files shared
			return ePluginAccessInactive;
		}

		if( ( ePluginTypeCamServer == pluginType ) 
			&& ( false ==  m_NetIdent.hasSharedWebCam() ) )
		{
			// no shared web cam
			return ePluginAccessInactive;
		}

		if( ( ePluginTypeAboutMePageServer == pluginType )
			|| ( ePluginTypeStoryboardServer == pluginType ) )
		{
			if( false == isOnline() )
			{
				accessState = ePluginAccessRequiresOnline;
			}
		}
		else if( ePluginTypeMessenger != pluginType )
		{
			if( false == isOnline() )
			{
				accessState = ePluginAccessRequiresOnline;
			}
		}		
	}

	return accessState;
}

//============================================================================
bool GuiUserBase::isMyAccessAllowedFromHim( enum EPluginType pluginType, bool inGroup )
{ 
    if( m_MyApp.getMyOnlineId() == m_NetIdent.getMyOnlineId() && eFriendStateIgnore != m_NetIdent.getPluginPermission( pluginType, inGroup ) )
    {
        return true;
    }

    return m_NetIdent.isMyAccessAllowedFromHim( pluginType, inGroup ); 
}

//============================================================================
QString GuiUserBase::describeMyFriendshipToHim( bool inGroup )
{
    return GuiParams::describeFriendship( m_NetIdent.getMyFriendshipToHim( inGroup ) );
}

//============================================================================
QString GuiUserBase::describeHisFriendshipToMe( bool inGroup )
{
    return GuiParams::describeFriendship( m_NetIdent.getHisFriendshipToMe( inGroup ) );
}

//============================================================================
void GuiUserBase::setNetIdent( VxNetIdent* netIdent )
{ 
    if( netIdent )
    {
        m_NetIdent = *netIdent; 
        m_OnlineId = m_NetIdent.getMyOnlineId();
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiUserBase::setNetIdent null ident" );
    }
}

//============================================================================
bool GuiUserBase::setRelayStatus( bool isRelayed )
{
    bool relayStateChanged = isRelayed != m_IsRelayed;
    if( relayStateChanged )
    {
        m_IsRelayed = isRelayed;
    }

    return relayStateChanged;
}

//============================================================================
bool GuiUserBase::isInSession( void )
{
    return isOnline() && m_SessionId.isVxGUIDValid();
}

//============================================================================
void GuiUserBase::addHostType( EHostType hostType )
{
    if( hostType > eHostTypeUnknown && hostType < eMaxHostType )
    {
        m_HostSet.insert( hostType );
    }
}

//============================================================================
QString GuiUserBase::describeHosts( void )
{
    QString hosts;
    for( auto hostType : m_HostSet )
    {
        hosts += " ";
        hosts += GuiParams::describeHostType( hostType );
    }

    return hosts;
}

//============================================================================
QString GuiUserBase::describeUser( bool verbose )
{
    QString descUser( m_NetIdent.getOnlineName() );
    descUser += " -> ";
    descUser += describeMyFriendshipToHim( false );
    descUser += " <- ";
    descUser += describeHisFriendshipToMe( false );
    if( verbose )
    {
        if( isHosted() )
        {
            descUser += " h(s) ";
            descUser += describeHosts();
        }
    }

    return descUser;
}