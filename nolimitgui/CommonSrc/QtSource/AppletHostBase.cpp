//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostBase.h"

#include "AppCommon.h"
#include "GuiHostSession.h"
#include "GuiUserMultiListWidget.h"

#include <P2PEngine/P2PEngine.h>

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
    switch( getHostType() )
    {
    case eHostTypeGroup:
        userList->setUserViewType( eUserViewTypeGroup );
        break;

    case eHostTypeChatRoom:
        userList->setUserViewType( eUserViewTypeChatRoom );
        break;

    case eHostTypeRandomConnect:
        userList->setUserViewType( eUserViewTypeRandomConnect );
        break;

    default:
        userList->setUserViewType( eUserViewTypeFriends );
    }

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
        if( hostSession->getOnlineId() != m_MyApp.getMyOnlineId() )
        {
            VxGUID::generateNewVxGUID( m_HostSessionId );

            HostedId adminId( hostSession->getOnlineId(), hostSession->getHostType() );
            m_Engine.fromGuiJoinHost( adminId, m_HostSessionId, m_HostUrlIpv4, m_HostUrlIpv6 );
        }
        else
        {
            QString warnJoinTitle = QObject::tr( "Cannot join our host as user" );
            QString warnJoinBody = QObject::tr( "Cannot join our host as user.\n You can join host from host admin page instead." );

            QMessageBox warnStorage( QMessageBox::Icon::Information, warnJoinTitle, warnJoinBody, QMessageBox::Ok );
            warnStorage.exec();
        }
    }
}

//============================================================================
void AppletHostBase::userJoinedHost( GuiHosted* guiHosted )
{
    if( guiHosted )
    {
        GuiUser* adminUser = guiHosted->getUser();
        if( adminUser )
        {
            HostedId adminId( adminUser->getMyOnlineId(), guiHosted->getHostType());
            GroupieId groupieId( m_MyApp.getMyOnlineId(), adminId );

            if( guiHosted != m_LastGuiHosted )
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
          
                if( adminId.isValid() )
                {                 
                    if( m_UserMultiList )
                    {                   
                        m_UserMultiList->setHostAdminId( groupieId );
                    }

                    if( !m_MyApp.getMemberActiveMgr().isMemberActive( groupieId ) )
                    {
                        m_Engine.fromGuiJoinHost( adminId, m_HostSessionId, m_HostUrlIpv4, m_HostUrlIpv6 );
                    }
                }
                else
                {
                    LogMsg( LOG_ERROR, "AppletHostBase::userJoinedHost invalid admin id %s", m_MyApp.describeHostedId( adminId ).c_str() );
                }
            }
            else
            {
                // TODO BRJ we should be able to just clear and again add members who are online
                if( m_UserMultiList )
                {
                    m_UserMultiList->setHostAdminId( groupieId );
                }
            }
        }
        else
        {
            LogMsg( LOG_ERROR, "AppletHostBase::userJoinedHost null adminUser" );
            vx_assert( false );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "AppletHostBase::userJoinedHost null guiHosted" );
        vx_assert( false );
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
