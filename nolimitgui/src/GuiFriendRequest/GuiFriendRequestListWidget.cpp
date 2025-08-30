//============================================================================
// Copyright (C) 2025 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiFriendRequestListItem.h"
#include "GuiFriendRequestListWidget.h"
#include "GuiFriendRequest.h"

#include "AppletPopupMenu.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "MyIconsDefs.h"
#include "VxPushButton.h"

#include <CoreLib/VxDebug.h>
 
//============================================================================
GuiFriendRequestListWidget::GuiFriendRequestListWidget( QWidget* parent )
: ListWidgetBase( parent )
{
    GetAppInstance().getUserMgr().wantGuiUserUpdateCallbacks( this, true );
}

//============================================================================
GuiFriendRequestListWidget::~GuiFriendRequestListWidget()
{
    GetAppInstance().getUserMgr().wantGuiUserUpdateCallbacks( this, false );
}

//============================================================================
void GuiFriendRequestListWidget::clearRequestList( void )
{
    for(int i = 0; i < count(); ++i)
    {
        QListWidgetItem* hostItem = item(i);
        delete ((GuiFriendRequestListItem *)hostItem);
    }

    clear();
}

//============================================================================
void GuiFriendRequestListWidget::addFriendRequest( GuiFriendRequest* friendRequest )
{
    GuiFriendRequestListItem* item = friendRequestToWidget( friendRequest );
    if( item )
    {
        QWidget* widget = dynamic_cast<QWidget*>( item );
        addItem( item );    
        setItemWidget( item, widget );
    }
}

//============================================================================
void GuiFriendRequestListWidget::removeFriendRequest( VxGUID& requestId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiFriendRequestListItem* widgetItem =  dynamic_cast<GuiFriendRequestListItem*>(item( iRow ));
        if( widgetItem )
        {
            QListWidgetItem* listItem = dynamic_cast<QListWidgetItem*>( widgetItem );
            GuiFriendRequest * friendRequest = widgetItem->getFriendRequest();
            if( friendRequest && friendRequest->getRequestId() == requestId )
            {
                removeItemWidget( listItem );
                takeItem( iRow );
            }
        }
    }
}

//============================================================================
GuiFriendRequestListItem* GuiFriendRequestListWidget::friendRequestToWidget( GuiFriendRequest* friendRequest )
{
    GuiFriendRequestListItem* friendRequestItem = new GuiFriendRequestListItem(this);
    QSize widgetSizeHint( friendRequestItem->sizeHint() );
	friendRequestItem->setSizeHint( widgetSizeHint );

    friendRequestItem->setFriendRequest( friendRequest );

    connect( friendRequestItem, SIGNAL(signalAcceptButtonClicked(GuiFriendRequestListItem*)),       this, SLOT(slotAcceptButtonClicked(GuiFriendRequestListItem*)) );
    connect( friendRequestItem, SIGNAL(signalDetailsButtonClicked(GuiFriendRequestListItem*)),	    this, SLOT(slotDetailsButtonClicked(GuiFriendRequestListItem*)) );
    connect( friendRequestItem, SIGNAL(signalFriendshipButtonClicked(GuiFriendRequestListItem*)),   this, SLOT(slotFriendshipButtonClicked(GuiFriendRequestListItem*)) );
    connect( friendRequestItem, SIGNAL(signalRejectButtonClicked(GuiFriendRequestListItem*)),	    this, SLOT(slotRejectButtonClicked(GuiFriendRequestListItem*)) );

    friendRequestItem->updateWidgetFromInfo();

    return friendRequestItem;
}

//============================================================================
GuiFriendRequest* GuiFriendRequestListWidget::widgetToFriendRequest( GuiFriendRequestListItem * item )
{
    return item->getFriendRequest();
}

//============================================================================
GuiFriendRequest * GuiFriendRequestListWidget::findFriendRequest( VxGUID& requestId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiFriendRequestListItem* listItem =  (GuiFriendRequestListItem*)item( iRow );
        if( listItem )
        {
            GuiFriendRequest * friendRequest = listItem->getFriendRequest();
            if( friendRequest && friendRequest->getRequestId() == requestId )
            {
                return friendRequest;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiFriendRequestListItem* GuiFriendRequestListWidget::findListItemWidgetByRequestId( VxGUID& requestId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiFriendRequestListItem*  hostItem = (GuiFriendRequestListItem*)item( iRow );
        if( hostItem )
        {
            GuiFriendRequest * friendRequest = hostItem->getFriendRequest();
            if( friendRequest && friendRequest->getRequestId() == requestId )
            {
                return hostItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiFriendRequestListItem* GuiFriendRequestListWidget::findListItemWidgetByOnlineId( VxGUID& onlineId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiFriendRequestListItem*  hostItem = (GuiFriendRequestListItem*)item( iRow );
        if( hostItem )
        {
            GuiFriendRequest * friendRequest = hostItem->getFriendRequest();
            if( friendRequest && friendRequest->getMyOnlineId() == onlineId )
            {
                return hostItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
void GuiFriendRequestListWidget::slotAcceptButtonClicked( GuiFriendRequestListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    GuiFriendRequest* friendRequest = hostItem->getFriendRequest();
    if( friendRequest )
    {
        emit signalAcceptButtonClicked( friendRequest, hostItem );
    }
    else
    {
        LogMsg( LOG_DEBUG, "GuiFriendRequestListWidget::%s this %p null getFriendRequest", __func__, hostItem);
    }
}

//============================================================================
void GuiFriendRequestListWidget::slotDetailsButtonClicked( GuiFriendRequestListItem* hostItem )
{
	if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
	{
		return;
	}

	m_ClickEventTimer.startTimer();
    GuiFriendRequest* friendRequest = hostItem->getFriendRequest();
    if( friendRequest )
    {
        emit signalDetailsButtonClicked( friendRequest, hostItem );
    }
    else
    {
        LogMsg( LOG_DEBUG, "GuiFriendRequestListWidget::%s this %p null getFriendRequest", __func__, hostItem);
    }
}

//============================================================================
void GuiFriendRequestListWidget::slotFriendshipButtonClicked( GuiFriendRequestListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    GuiFriendRequest* friendRequest = hostItem->getFriendRequest();
    if( friendRequest )
    {
        GuiUser* guiUser = m_MyApp.getUserMgr().getUser( friendRequest->getRequestInfo()->getUserOnlineId() );
        if( guiUser )
        {
            launchChangeFriendship( guiUser );
        }
    }
    else
    {
        LogMsg( LOG_DEBUG, "GuiFriendRequestListWidget::%s this %p null getFriendRequest", __func__, hostItem);
    }
}

//============================================================================
void GuiFriendRequestListWidget::slotRejectButtonClicked( GuiFriendRequestListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    GuiFriendRequest* friendRequest = hostItem->getFriendRequest();
    if( friendRequest )
    {
        emit signalRejectButtonClicked( friendRequest, hostItem );
    }
}

//============================================================================
void GuiFriendRequestListWidget::updateUser( GuiUser* guiUser )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiFriendRequestListItem* listItem = ( GuiFriendRequestListItem* )item( iRow );
        if( listItem )
        {
            GuiFriendRequest* friendRequest = listItem->getFriendRequest();
            if( friendRequest && friendRequest->getMyOnlineId() == guiUser->getMyOnlineId() )
            {
                //friendRequest->updateUser( guiUser );
                //listItem->updateUser( guiUser );
                //addOrUpdateHostSession( friendRequest );
            }
        }
    }
}

//============================================================================
void GuiFriendRequestListWidget::callbackUserUpdated( GuiUser* guiUser )
{
    updateUser( guiUser );
}
