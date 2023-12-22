//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletRandomConnectHostAdmin.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiUserMultiListWidget.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletRandomConnectHostAdmin::AppletRandomConnectHostAdmin( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_RANDOM_CONNECT_HOST_ADMIN, app, parent )
{
    setAppletType( eAppletRandomConnectHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

    GroupieId hostAdminId( m_MyApp.getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypeRandomConnect );
    ui.m_UserListWidget->setHostAdminId( hostAdminId );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeRandomConnect );

    std::set<VxGUID> memberList;
    m_MyApp.getHostJoinMgr().getHostedMembers( eHostTypeChatRoom, memberList );
    for( auto onlineId : memberList )
    {
        ui.m_UserListWidget->getUserListWidget()->addUser( onlineId );

        //RandomConnectieId groupieId(onlineId, m_MyApp.getMyOnlineId(), eHostTypeChatRoom);
        //GuiHostJoin* hostJoin = m_MyApp.getHostJoinMgr().getHostJoin( groupieId );
        //if( hostJoin )
        //{
        //    ui.m_UserListWidget->addUser( hostJoin );
        //}
        //else
        //{
        //    LogMsg( LOG_ERROR, "AppletRandomConnectHostAdmin hostJoin not found %s", m_MyApp.describeRandomConnectieId( groupieId ) );
        //}
           
    }

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletRandomConnectHostAdmin::~AppletRandomConnectHostAdmin()
{
    m_MyApp.activityStateChange( this, false );
}
