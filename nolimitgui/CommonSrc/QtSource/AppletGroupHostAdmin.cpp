//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGroupHostAdmin.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiUserMultiListWidget.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletGroupHostAdmin::AppletGroupHostAdmin( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_GROUP_HOST_ADMIN, app, parent )
{
    setAppletType( eAppletGroupHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );


    std::set<VxGUID> memberList;
    m_MyApp.getHostJoinMgr().getHostedMembers( eHostTypeChatRoom, memberList );
    for( auto onlineId : memberList )
    {
        ui.m_UserListWidget->getUserListWidget()->addUser( onlineId );

        //GroupieId groupieId(onlineId, m_MyApp.getMyOnlineId(), eHostTypeChatRoom);
        //GuiHostJoin* hostJoin = m_MyApp.getHostJoinMgr().getHostJoin( groupieId );
        //if( hostJoin )
        //{
        //    ui.m_UserListWidget->addUser( hostJoin );
        //}
        //else
        //{
        //    LogMsg( LOG_ERROR, "AppletGroupHostAdmin hostJoin not found %s", m_MyApp.describeGroupieId( groupieId ) );
        //}
           
    }

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletGroupHostAdmin::~AppletGroupHostAdmin()
{
    m_MyApp.activityStateChange( this, false );
}
