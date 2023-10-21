//============================================================================
// Copyright (C) 2019 Brett R. Jones
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

#include "AppletHostBase.h"

#include "AppCommon.h"
#include "GuiHostSession.h"
#include "GuiUserMultiListWidget.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletHostBase::AppletHostBase( const char* objName, AppCommon& app, QWidget* parent )
    : AppletBase( objName, app, parent )
{
}

//============================================================================
AppletHostBase::~AppletHostBase()
{
    m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, false );
}

//============================================================================
void AppletHostBase::manageUsers( GuiUserMultiListWidget* userList )
{
    m_UserMultiList = userList;
    if( m_UserMultiList && m_UserMultiList->getUserListWidget() )
    {
        manageUsers( m_UserMultiList->getUserListWidget() );
    }
}

//============================================================================
void AppletHostBase::manageUsers( GuiUserListWidget* userList )
{
    m_UserList = userList;
    if( m_UserList )
    {
        m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, true );
    }
}

//============================================================================
void AppletHostBase::manageHostSession( GuiHostSession* hostSession, bool requestJoin )
{
    m_HostUrlIpv4 = hostSession->getHostUrl( false );
    m_HostUrlIpv6 = hostSession->getHostUrl( true );

    if( requestJoin )
    {
        VxGUID::generateNewVxGUID( m_HostSessionId );

        m_Engine.fromGuiJoinHost( getHostType(), m_HostSessionId, m_HostUrlIpv4, m_HostUrlIpv6 );
    }
}

//============================================================================
void AppletHostBase::userJoinedHost( GuiHosted* guiHosted )
{
    if( guiHosted )
    {
        if( !m_HostSessionId.isVxGUIDValid() )
        {
            m_HostSessionId = guiHosted->getSessionId();
            if( !m_HostSessionId.isVxGUIDValid() )
            {
                VxGUID::generateNewVxGUID( m_HostSessionId );
                guiHosted->setSessionId( m_HostSessionId );
            }
        }
     
        m_HostUrlIpv4 = guiHosted->getHostInviteUrl( false );
        m_HostUrlIpv6 = guiHosted->getHostInviteUrl( true );

        m_Engine.fromGuiJoinHost( getHostType(), m_HostSessionId, m_HostUrlIpv4, m_HostUrlIpv6 );
        if( m_UserMultiList )
        {
            m_UserMultiList->userJoinedHost( guiHosted );
        }
    }
}

//============================================================================
void AppletHostBase::callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp )
{
}

//============================================================================
void AppletHostBase::callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId )
{
    if( m_UserList && listType == m_UserList->getUserViewType() )
    {
        removeUser( onlineId );
    }
}

//============================================================================
void AppletHostBase::callbackUserAdded( GuiUser* guiUser )
{
    updateUser( guiUser );
}

//============================================================================
void AppletHostBase::callbackUserUpdated( GuiUser* guiUser )
{
    updateUser( guiUser );
}

//============================================================================
void AppletHostBase::callbackUserRemoved( VxGUID& onlineId )
{
    removeUser( onlineId );
}

//============================================================================
void AppletHostBase::callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
    if( m_UserList )
    {
        m_UserList->callbackPushToTalkStatus( onlineId, pushToTalkStatus );
    }
}

//============================================================================
void AppletHostBase::updateUser( GuiUser* guiUser )
{
    if( guiUser && m_UserList )
    {
        m_UserList->updateUser( guiUser );
    }
}

//============================================================================
void AppletHostBase::removeUser( VxGUID& onlineId )
{
    if( m_UserList )
    {
        m_UserList->removeUser( onlineId );
    }   
}
