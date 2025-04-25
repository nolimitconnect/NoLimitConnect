//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletFriendRequestList.h"

#include "ActivityMessageBox.h"
#include "AppCommon.h"
#include "AppletAboutUser.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include "ui_AppletFriendRequestList.h"

//============================================================================
AppletFriendRequestList::AppletFriendRequestList( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_FRIEND_REQUEST_LIST, app, parent )
, ui(*(new Ui::AppletFriendRequestListUi))
{
    setAppletType( eAppletFriendRequestList );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    updateRequestList();

    connect( this,					    SIGNAL(finished(int)),						this, SLOT(slotBackButtonClicked()) );

    connect( ui.m_GuiFriendRequestListWidget,      SIGNAL(signalAcceptButtonClicked(GuiFriendRequest*,GuiFriendRequestListItem*)),  this, SLOT(slotAcceptButtonClicked(GuiFriendRequest*,GuiFriendRequestListItem*)) );
    connect( ui.m_GuiFriendRequestListWidget,      SIGNAL(signalDetailsButtonClicked(GuiFriendRequest*,GuiFriendRequestListItem*)),  this, SLOT(slotDetailsButtonClicked(GuiFriendRequest*,GuiFriendRequestListItem*)) );
    connect( ui.m_GuiFriendRequestListWidget,      SIGNAL(signalRejectButtonClicked(GuiFriendRequest*,GuiFriendRequestListItem*)), this, SLOT(slotRejectButtonClicked(GuiFriendRequest*,GuiFriendRequestListItem*)) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletFriendRequestList::~AppletFriendRequestList()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletFriendRequestList::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
    wantActivityCallbacks( true );
}

//============================================================================
void AppletFriendRequestList::hideEvent( QHideEvent* ev )
{
    wantActivityCallbacks( false );
    ActivityBase::hideEvent( ev );
}

//============================================================================
void AppletFriendRequestList::updateRequestList( void )
{
    ui.m_GuiFriendRequestListWidget->clearRequestList();
    std::vector<GuiFriendRequest*>&  requestList = m_MyApp.getFriendRequestMgr().getRequestList();
    for( auto friendRequest : requestList )
    {
        ui.m_GuiFriendRequestListWidget->addFriendRequest( friendRequest );
    }

    if( requestList.empty() )
    {
        ui.m_StatusLabel->setText( QObject::tr( "No Friend Requests Available" ) );
    }
}

//============================================================================
void AppletFriendRequestList::slotAcceptButtonClicked( GuiFriendRequest* friendRequest, GuiFriendRequestListItem* hostItem )
{
    GuiUser* guiUser = m_MyApp.getUserMgr().getUser( friendRequest->getRequestInfo()->getUserOnlineId() );
    if( guiUser )
    {
        if( !(guiUser->getMyFriendshipToHim() >= eFriendStateFriend) )
        {
            GuiHelpers::showRequiresFriendshipError( this );
            return;
        }

        m_MyApp.getFriendRequestMgr().friendAccepted( friendRequest );
    }
    else
    {
        GuiHelpers::showUserNotFoundError( this );
    }

    ui.m_GuiFriendRequestListWidget->removeFriendRequest( friendRequest->getRequestId() );
}

//============================================================================
void AppletFriendRequestList::slotDetailsButtonClicked( GuiFriendRequest* friendRequest, GuiFriendRequestListItem* hostItem )
{
    GuiUser* guiUser = m_MyApp.getUserMgr().getUser( friendRequest->getRequestInfo()->getUserOnlineId() );
    if( guiUser )
    {
	    AppletAboutUser * applet = dynamic_cast<AppletAboutUser*>(m_MyApp.launchApplet( eAppletAboutUser, getParentPageFrame() ));
	    if( applet )
	    {
		    applet->setUser( guiUser );
            std::string userMsg = friendRequest->getRequestMsg();
            if( !userMsg.empty() )
            {
                applet->setUserMessage( userMsg.c_str() );
            }
	    }
    }
    else
    {
        GuiHelpers::showUserNotFoundError( this );
    }
}

//============================================================================
void AppletFriendRequestList::slotRejectButtonClicked( GuiFriendRequest* friendRequest, GuiFriendRequestListItem* hostItem )
{
    m_MyApp.getFriendRequestMgr().friendRejected( friendRequest );
    ui.m_GuiFriendRequestListWidget->removeFriendRequest( friendRequest->getRequestId() );
}
