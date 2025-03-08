//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiUserBase.h"

#include "AppCommon.h"
#include "GuiMemberActiveMgr.h"
#include "GuiUserMgr.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>
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
    return m_MyApp.getConnectIdListMgr().isOnline( getMyOnlineId() );
}

//============================================================================
bool GuiUserBase::isDirectConnect( void )
{
    return m_MyApp.getConnectIdListMgr().isDirectConnect( getMyOnlineId() );
}

//============================================================================
bool GuiUserBase::isRelayed( void )
{
    return m_MyApp.getConnectIdListMgr().isRelayed( getMyOnlineId() );
}

//============================================================================
bool GuiUserBase::isHosted( void )
{ 
    return isGroupHosted() || isChatRoomHosted() || isRandomConnectHosted();
}

//============================================================================
bool GuiUserBase::isGroupHosted( void )
{ 
    return m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeGroup, getMyOnlineId() );
}
//============================================================================
bool GuiUserBase::isChatRoomHosted( void )                
{ 
    return m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeChatRoom, getMyOnlineId() );
}

//============================================================================
bool GuiUserBase::isRandomConnectHosted( void )           
{ 
    return m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeRandomConnect, getMyOnlineId() );
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
bool GuiUserBase::isInSession( void )
{
    return isOnline() && m_SessionId.isVxGUIDValid();
}

//============================================================================
QString GuiUserBase::describeUser( bool verbose )
{
    QString descUser( m_NetIdent.describeUser().c_str() );
    descUser += " -> ";
    descUser += describeMyFriendshipToHim( false );
    descUser += " <- ";
    descUser += describeHisFriendshipToMe( false );

    return descUser;
}
