//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "GuiHostedListItem.h"

#include "AppletPopupMenu.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiHostedListWidget.h"
#include "GuiHostedListSession.h"
#include "MyIconsDefs.h"
#include "VxPushButton.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
 
//============================================================================
GuiHostedListWidget::GuiHostedListWidget( QWidget* parent )
: ListWidgetBase( parent )
{
	// QListWidget::setSortingEnabled( true );
	// sortItems( Qt::DescendingOrder );

    connect( this, SIGNAL(itemClicked(QListWidgetItem*)),          this, SLOT(slotGuiHostedListItemClicked(QListWidgetItem*))) ;
    connect( this, SIGNAL(itemDoubleClicked(QListWidgetItem*)),    this, SLOT(slotGuiHostedListItemClicked(QListWidgetItem*))) ;
    GetAppInstance().getUserMgr().wantGuiUserUpdateCallbacks( this, true );
}

//============================================================================
GuiHostedListWidget::~GuiHostedListWidget()
{
    GetAppInstance().getUserMgr().wantGuiUserUpdateCallbacks( this, false );
}

//============================================================================
GuiHostedListItem* GuiHostedListWidget::sessionToWidget( GuiHostedListSession* hostSession )
{
    GuiHostedListItem* hostItem = new GuiHostedListItem( getHostType(), this );
    hostItem->setSizeHint( hostItem->calculateSizeHint() );
    hostItem->setIsHostView( getIsHostView() );

    hostItem->setHostSession( hostSession );

    connect( hostItem, SIGNAL(signalGuiHostedListItemClicked(QListWidgetItem*) ),	        this, SLOT(slotGuiHostedListItemClicked(QListWidgetItem*) ) );
    connect( hostItem, SIGNAL(signalIconButtonClicked(GuiHostedListItem*) ),	            this, SLOT(slotIconButtonClicked(GuiHostedListItem*) ) );
    connect( hostItem, SIGNAL(signalFriendshipButtonClicked(GuiHostedListItem*) ),         this, SLOT(slotFriendshipButtonClicked(GuiHostedListItem*) ) );
    connect( hostItem, SIGNAL(signalMenuButtonClicked(GuiHostedListItem*) ),	            this, SLOT(slotMenuButtonClicked(GuiHostedListItem*) ) );
    connect( hostItem, SIGNAL(signalJoinButtonClicked(GuiHostedListItem*) ),		        this, SLOT(slotJoinButtonClicked(GuiHostedListItem*) ) );
    connect( hostItem, SIGNAL(signalConnectButtonClicked(GuiHostedListItem*) ),            this, SLOT(slotConnectButtonClicked(GuiHostedListItem*) ) );
    connect( hostItem, SIGNAL(signalKickButtonClicked(GuiHostedListItem*) ),               this, SLOT(slotKickButtonClicked(GuiHostedListItem*) ) );
    connect( hostItem, SIGNAL(signalIgnoreButtonClicked(GuiHostedListItem*) ),             this, SLOT(slotIgnoreButtonClicked(GuiHostedListItem*) ) );


    hostItem->updateWidgetFromInfo();

    return hostItem;
}

//============================================================================
GuiHostedListSession* GuiHostedListWidget::widgetToSession( GuiHostedListItem * item )
{
    return item->getHostSession();
}

//============================================================================
GuiHostedListSession * GuiHostedListWidget::findSession( VxGUID& lclSessionId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiHostedListItem* listItem =  (GuiHostedListItem*)item( iRow );
        if( listItem )
        {
            GuiHostedListSession * hostSession = listItem->getHostSession();
            if( hostSession && hostSession->getSessionId() == lclSessionId )
            {
                return hostSession;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiHostedListSession* GuiHostedListWidget::findSession( HostedId& hostedId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiHostedListItem* listItem = ( GuiHostedListItem* )item( iRow );
        if( listItem )
        {
            GuiHostedListSession* hostSession = listItem->getHostSession();
            if( hostSession && hostSession->getHostedId() == hostedId )
            {
                return hostSession;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiHostedListItem* GuiHostedListWidget::findListItemWidgetByHostId( HostedId& hostedId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiHostedListItem*  hostItem = (GuiHostedListItem*)item( iRow );
        if( hostItem )
        {
            GuiHostedListSession * hostSession = hostItem->getHostSession();
            if( hostSession && hostSession->getHostedId() == hostedId )
            {
                return hostItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiHosted* GuiHostedListWidget::findGuiHostedByHostId( HostedId& hostedId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiHostedListItem* hostItem = ( GuiHostedListItem* )item( iRow );
        if( hostItem )
        {
            GuiHostedListSession* hostSession = hostItem->getHostSession();
            if( hostSession && hostSession->getHostedId() == hostedId )
            {
                return hostSession->getGuiHosted();
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiHostedListItem* GuiHostedListWidget::findListItemWidgetByOnlineId( VxGUID& onlineId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiHostedListItem*  hostItem = (GuiHostedListItem*)item( iRow );
        if( hostItem )
        {
            GuiHostedListSession * hostSession = hostItem->getHostSession();
            if( hostSession && hostSession->getGuiUser() && hostSession->getGuiUser()->getMyOnlineId() == onlineId )
            {
                return hostItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
void GuiHostedListWidget::slotGuiHostedListItemClicked( QListWidgetItem* hostItem )
{
	if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
	{
		return;
	}

	m_ClickEventTimer.startTimer();
    GuiHostedListItem* hostWidget = dynamic_cast<GuiHostedListItem*>(hostItem);
    if( hostWidget )
    {
        onGuiHostedListItemClicked(hostWidget);
    }
}

//============================================================================
void GuiHostedListWidget::slotIconButtonClicked( GuiHostedListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onIconButtonClicked(hostItem);
}

//============================================================================
void GuiHostedListWidget::slotFriendshipButtonClicked( GuiHostedListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onFriendshipButtonClicked( hostItem );
}

//============================================================================
void GuiHostedListWidget::slotMenuButtonClicked( GuiHostedListItem* hostItem )
{
	if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
	{
		return;
	}

	m_ClickEventTimer.startTimer();
    onMenuButtonClicked( hostItem );
}

//============================================================================
void GuiHostedListWidget::slotJoinButtonClicked( GuiHostedListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onJoinButtonClicked( hostItem );
}

//============================================================================
void GuiHostedListWidget::slotConnectButtonClicked( GuiHostedListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onConnectButtonClicked( hostItem );
}

//============================================================================
void GuiHostedListWidget::slotKickButtonClicked( GuiHostedListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onKickButtonClicked( hostItem );
}

//============================================================================
void GuiHostedListWidget::slotIgnoreButtonClicked( GuiHostedListItem* hostItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onIgnoreButtonClicked( hostItem );
}

//============================================================================
void GuiHostedListWidget::updateHostedList( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId )
{
    GuiHostedListSession* hostSession = findSession( hostedId );
    if( hostSession )
    {
        hostSession->setSessionId( sessionId );
        if( guiHosted )
        {
            hostSession->setGuiHosted( guiHosted );
        }

        addOrUpdateHostSession( hostSession );
    }
    else
    {
        GuiHostedListSession* hostSession = new GuiHostedListSession( hostedId, guiHosted, this );
        hostSession->setSessionId( sessionId );

        addOrUpdateHostSession( hostSession );
    }
}

//============================================================================
GuiHostedListItem* GuiHostedListWidget::addOrUpdateHostSession( GuiHostedListSession* hostSession )
{
    GuiUser* guiUser = hostSession->getGuiUser();
    GuiHostedListItem* hostItem = findListItemWidgetByHostId( hostSession->getHostedId() );
    if( hostItem )
    {
        GuiHostedListSession* hostOldSession = hostItem->getHostSession();
        if( hostOldSession != hostSession )
        {
            hostItem->setHostSession( hostSession );
            if( !hostOldSession->parent() )
            {
                hostOldSession->deleteLater();
            }
        }

        hostItem->updateWidgetFromInfo();
    }
    else
    {
        hostItem = sessionToWidget( hostSession );
        if( hostItem )
        {          
            if( 0 == count() )
            {
                LogModule( eLogHostJoin, LOG_INFO, "%s add host %s user %s", __func__, hostSession->getHostTitle().c_str(), guiUser ? guiUser->getOnlineName().c_str() : "" );
                addItem( hostItem );
            }
            else
            {
                LogModule( eLogHostJoin, LOG_INFO, "%s insert host %s user %s", __func__, hostSession->getHostTitle().c_str(), guiUser ? guiUser->getOnlineName().c_str() : "" );
                insertItem( 0, (QListWidgetItem*)hostItem );
            }

            setItemWidget( (QListWidgetItem*)hostItem, (QWidget*)hostItem );
            if( hostItem )
            {
                if( !hostItem->getIsThumbUpdated() )
                {
                    VxGUID thumbId = hostSession->getHostThumbId(); 
                    if( thumbId.isVxGUIDValid() )
                    {
                        GuiThumb* guiThumb = m_MyApp.getThumbMgr().getThumb( thumbId );
                        if( guiThumb )
                        {
                            QImage hostIconImage;
                            guiThumb->createImage( hostIconImage );
                            VxPushButton* hostImageButton = hostItem->getIdentAvatarButton();
                            if( hostImageButton && !hostIconImage.isNull() )
                            {
                                hostImageButton->setIconOverrideImage( hostIconImage );
                                hostItem->setIsThumbUpdated( true );
                            }
                        }
                    }
                }
            }         
        }
    }

    if( hostItem && guiUser )
    {
        hostItem->setJoinedState( m_Engine.fromGuiQueryJoinState( hostSession->getHostType(), guiUser->getNetIdent() ) );
    }

    return hostItem;
}

//============================================================================
void GuiHostedListWidget::clearHostList( void )
{
    for(int i = 0; i < count(); ++i)
    {
        QListWidgetItem* hostItem = item(i);
        delete ((GuiHostedListItem *)hostItem);
    }

    clear();
}

//============================================================================
void GuiHostedListWidget::onGuiHostedListItemClicked( GuiHostedListItem* hostItem )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "%s", __func__ );
    onJoinButtonClicked( hostItem );
}

//============================================================================
void GuiHostedListWidget::onIconButtonClicked( GuiHostedListItem* hostItem )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "%s", __func__ );
    GuiHostedListSession* hostSession = hostItem->getHostSession();
    if( hostSession )
    {
        emit signalIconButtonClicked( hostSession, hostItem );
    }
}

//============================================================================
void GuiHostedListWidget::onFriendshipButtonClicked( GuiHostedListItem* hostItem )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "%s", __func__ );
    if( hostItem )
    {
        GuiHostedListSession* hostSession = hostItem->getHostSession();
        if( hostSession )
        {
            if( hostSession && hostSession->getGuiUser() )
            {
                launchChangeFriendship( hostSession->getGuiUser() );
            }
        }
    }
}

//============================================================================
void GuiHostedListWidget::onMenuButtonClicked( GuiHostedListItem* hostItem )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
    if( hostItem )
    {
        GuiHostedListSession* hostSession = hostItem->getHostSession();
        if( hostSession )
        {
            AppletPopupMenu* popupMenu = dynamic_cast<AppletPopupMenu*>(m_MyApp.launchApplet( eAppletPopupMenu, GuiHelpers::getParentPageFrame( this ) ) );
            if( popupMenu )
            {
                popupMenu->showHostedListSessionMenu( hostSession );
            }
        }
    }
}

//============================================================================
void GuiHostedListWidget::onJoinButtonClicked( GuiHostedListItem* hostItem )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "%s", __func__ );
    GuiHostedListSession* hostSession = hostItem->getHostSession();
    if( hostSession )
    {
        emit signalJoinButtonClicked( hostSession, hostItem );
    }
}

//============================================================================
void GuiHostedListWidget::onConnectButtonClicked( GuiHostedListItem* hostItem )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "%s", __func__ );
    GuiHostedListSession* hostSession = hostItem->getHostSession();
    if( hostSession )
    {
        emit signalConnectButtonClicked( hostSession, hostItem );
    }
}

//============================================================================
void GuiHostedListWidget::onKickButtonClicked( GuiHostedListItem* hostItem )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "%s", __func__ );
    GuiHostedListSession* hostSession = hostItem->getHostSession();
    if( hostSession )
    {
        emit signalKickButtonClicked( hostSession, hostItem );
    }
}

//============================================================================
void GuiHostedListWidget::onIgnoreButtonClicked( GuiHostedListItem* hostItem )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "%s", __func__ );
    GuiHostedListSession* hostSession = hostItem->getHostSession();
    if( hostSession )
    {
        emit signalIgnoreButtonClicked( hostSession, hostItem );
    }
}

//============================================================================
void GuiHostedListWidget::callbackUserAdded( GuiUser* guiUser )
{
    if( getIsHostView() )
    {
        updateUser( guiUser );
    }
}

//============================================================================
void GuiHostedListWidget::callbackUserUpdated( GuiUser* guiUser )
{
    if( getIsHostView() )
    {
        updateUser( guiUser );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
    updateHostJoinState( groupieId.getHostedId(), eJoinStateJoinRequested );
}

//============================================================================
void GuiHostedListWidget::callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if( getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
        updateHostJoinState( groupieId.getHostedId(), eJoinStateJoinWasGranted );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if( getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
        updateHostJoinState( groupieId.getHostedId(), eJoinStateJoinIsGranted );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if( getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
        updateHostJoinState( groupieId.getHostedId(), eJoinStateJoinLeaveHost );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if( getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
        updateHostJoinState( groupieId.getHostedId(), eJoinStateJoinDenied );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiHostJoinLeaveHost( GroupieId& groupieId )
{
    if( getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
        updateHostJoinState( groupieId.getHostedId(), eJoinStateJoinLeaveHost );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiHostUnJoin( GroupieId& groupieId )
{
    if( getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
        updateHostJoinState( groupieId.getHostedId(), eJoinStateNone );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiHostJoinRemoved( GroupieId& groupieId )
{
    if( getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s", __func__ );
        removeFromList( groupieId.getHostedId() );
    }
}
//============================================================================
//============================================================================

//============================================================================
void GuiHostedListWidget::callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if( !getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "AppletGroupJoin::%s", __func__ );
        updateUserJoinState( groupieId.getHostedId(), eJoinStateJoinRequested );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if( !getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "AppletGroupJoin::%s", __func__ );
        updateUserJoinState( groupieId.getHostedId(), eJoinStateJoinWasGranted );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if( !getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "AppletGroupJoin::%s", __func__ );
        updateUserJoinState( groupieId.getHostedId(), eJoinStateJoinIsGranted );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if( !getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "AppletGroupJoin::%s", __func__ );
        updateUserJoinState( groupieId.getHostedId(), eJoinStateJoinLeaveHost );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if( !getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "AppletGroupJoin::%s", __func__ );
        updateUserJoinState( groupieId.getHostedId(), eJoinStateJoinDenied );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiUserJoinLeaveHost( GroupieId& groupieId )
{
    if( !getIsHostView() )
    {
        LogModule( eLogHostJoin, LOG_VERBOSE, "AppletGroupJoin::%s", __func__ );
        updateUserJoinState( groupieId.getHostedId(), eJoinStateJoinLeaveHost );
    }
}

//============================================================================
void GuiHostedListWidget::callbackGuiUserJoinRemoved( GroupieId& groupieId )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "AppletGroupJoin::%s", __func__ );
    if( !getIsHostView() )
    {
        removeFromList( groupieId.getHostedId() );
    }
}

//============================================================================
void GuiHostedListWidget::updateHostJoinState( HostedId& hostedId, EJoinState joinState )
{
    if( getIsHostView() )
    {
        updateJoinState( hostedId, joinState );
    }
}

//============================================================================
void GuiHostedListWidget::updateUserJoinState( HostedId& hostedId, EJoinState joinState )
{
    if( !getIsHostView() )
    {
        updateJoinState( hostedId, joinState );
    }
}

//============================================================================
void GuiHostedListWidget::updateJoinState( HostedId& hostedId, EJoinState joinState )
{
    GuiHostedListItem* listItem = findListItemWidgetByHostId( hostedId );
    if( listItem )
    {
        listItem->setJoinedState( joinState );
    }
}

//============================================================================
void GuiHostedListWidget::removeFromList( HostedId& hostedId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiHostedListItem* hostItem = ( GuiHostedListItem* )item( iRow );
        if( hostItem )
        {
            GuiHostedListSession* hostSession = hostItem->getHostSession();
            if( hostSession && hostSession->getHostedId() == hostedId )
            {
                removeItemWidget( hostItem );
                break;
            }
        }
    }
}

//============================================================================
void GuiHostedListWidget::updateUser( GuiUser* guiUser )
{
    LogModule( eLogHostJoin, LOG_VERBOSE, "GuiHostedListWidget::%s %s", __func__, guiUser->getOnlineName().c_str() );

    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiHostedListItem* hostItem = ( GuiHostedListItem* )item( iRow );
        if( hostItem )
        {
            GuiHostedListSession* hostSession = hostItem->getHostSession();
            if( hostSession && hostSession->getHostOnlineId() == guiUser->getMyOnlineId() )
            {
                hostItem->updateUser( guiUser );
                addOrUpdateHostSession( hostSession );
            }
        }
    }
}
