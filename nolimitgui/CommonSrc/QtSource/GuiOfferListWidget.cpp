//============================================================================
// Copyright (C) 2022 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "GuiOfferListItem.h"
#include "GuiUser.h"
#include "GuiOfferListWidget.h"
#include "GuiOfferSession.h"

#include "MyIcons.h"
#include "AppletPopupMenu.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiParams.h"
#include "VxPushButton.h"

#include <CoreLib/VxDebug.h>

#include <vector>
 
//============================================================================
GuiOfferListWidget::GuiOfferListWidget( QWidget* parent )
: ListWidgetBase( parent )
{
	QListWidget::setSortingEnabled( true );
	sortItems( Qt::DescendingOrder );

    GetAppInstance().getUserMgr().wantGuiUserUpdateCallbacks( this, true );
    GetAppInstance().getThumbMgr().wantGuiThumbCallbacks( this, true );
    GetAppInstance().getOfferMgr().wantGuiOfferCallbacks( this, true );
}

//============================================================================
GuiOfferListWidget::~GuiOfferListWidget()
{
    GetAppInstance().getOfferMgr().wantGuiOfferCallbacks( this, false );
    GetAppInstance().getThumbMgr().wantGuiThumbCallbacks( this, false );
    GetAppInstance().getUserMgr().wantGuiUserUpdateCallbacks( this, false );
}

//============================================================================
void GuiOfferListWidget::callbackMyIdentUpdated( GuiUser* guiUser )
{
    //if( guiUser )
    //{
    //    callbackUserUpdated( guiUser );
    //}
}

//============================================================================
void GuiOfferListWidget::clearOfferList( void )
{
    for( int i = 0; i < count(); ++i )
    {
        QListWidgetItem* offerItem = item( i );
        delete ( ( GuiOfferListItem* )offerItem );
    }

    clear();
}

//============================================================================
GuiOfferListItem* GuiOfferListWidget::sessionToWidget( GuiOfferSession* offerSession )
{
    GuiOfferListItem* offerItem = new GuiOfferListItem(this);
    offerItem->setSizeHint( offerItem->calculateSizeHint() );

    offerItem->setOfferSession( offerSession );

    //connect( offerItem, SIGNAL( signalGuiOfferListItemClicked(GuiOfferListItem*) ), this, SLOT( slotUserListItemClicked(GuiOfferListItem*) ) );
    connect( offerItem, SIGNAL( signalAvatarButtonClicked(GuiOfferListItem*) ), this, SLOT( slotAvatarButtonClicked(GuiOfferListItem*) ) );
    connect( offerItem, SIGNAL( signalFriendshipButtonClicked(GuiOfferListItem*) ), this, SLOT( slotFriendshipButtonClicked(GuiOfferListItem*) ) );
    connect( offerItem, SIGNAL( signalOfferViewButtonClicked(GuiOfferListItem*) ), this, SLOT( slotOfferViewButtonClicked(GuiOfferListItem*) ) );
    connect( offerItem, SIGNAL( signalOfferAcceptButtonClicked(GuiOfferListItem*) ), this, SLOT( slotOfferAcceptButtonClicked(GuiOfferListItem*) ) );
    connect( offerItem, SIGNAL( signalOfferRejectButtonClicked(GuiOfferListItem*) ), this, SLOT( slotOfferRejectButtonClicked(GuiOfferListItem*) ) );
    connect( offerItem, SIGNAL( signalPushToTalkButtonClicked(GuiOfferListItem*) ), this, SLOT( slotPushToTalkButtonClicked(GuiOfferListItem*) ) );
    connect( offerItem, SIGNAL( signalMenuButtonClicked(GuiOfferListItem*) ), this, SLOT( slotMenuButtonClicked(GuiOfferListItem*) ) );

    offerItem->updateWidgetFromInfo();

    return offerItem;
}

//============================================================================
GuiOfferSession* GuiOfferListWidget::widgetToSession( GuiOfferListItem * item )
{
    return item->getOfferSession();
}

//============================================================================
void GuiOfferListWidget::updateUser( GuiUser* guiUser )
{
    if( guiUser )
    {
        int iCnt = count();
        for( int iRow = 0; iRow < iCnt; iRow++ )
        {
            GuiOfferListItem* offerItem = (GuiOfferListItem*)item( iRow );
            if( offerItem )
            {
                GuiOfferSession* offerSession = offerItem->getOfferSession();
                if( offerSession && (offerSession->getUser() == guiUser) )
                {
                    onListItemUpdated( offerItem->getOfferSession(), offerItem );
                    offerItem->update();
                }
            }
        }
    }
}

//============================================================================
void GuiOfferListWidget::updateEntryWidget( VxGUID& onlineId )
{
    GuiOfferListItem* entryWidget = findListEntryWidgetByOnlineId( onlineId );
    if( entryWidget )
    {
        entryWidget->update();
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiOfferListWidget::updateUser failed to find session for user" );
    }
}

//============================================================================
void GuiOfferListWidget::removeSession( VxGUID& uniqueId )
{
    GuiOfferListItem* offerItem = findListEntryWidgetByUniqueId( uniqueId );
    if( offerItem )
    {
        QListWidgetItem* listItem = dynamic_cast<QListWidgetItem*>( offerItem );
        removeItemWidget(listItem);
        delete listItem; // Qt documentation warnings you to destroy item to effectively remove it from QListWidget.
    }  
}

//============================================================================
GuiOfferSession * GuiOfferListWidget::findSession( VxGUID& lclSessionId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiOfferListItem* listItem =  (GuiOfferListItem*)item( iRow );
        if( listItem )
        {
            GuiOfferSession * offerSession = listItem->getOfferSession();
            if( offerSession && offerSession->getOfferSessionId() == lclSessionId )
            {
                return offerSession;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiOfferListItem* GuiOfferListWidget::findListEntryWidgetBySessionId( VxGUID& sessionId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiOfferListItem*  offerItem = (GuiOfferListItem*)item( iRow );
        if( offerItem )
        {
            GuiOfferSession * offerSession = offerItem->getOfferSession();
            if( offerSession && ( offerSession->getOfferSessionId() == sessionId ) )
            {
                return offerItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiOfferListItem* GuiOfferListWidget::findListEntryWidgetByOnlineId( VxGUID& onlineId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ERROR GuiOfferListWidget::findListEntryWidgetByOnlineId: invalid online id" );
        return nullptr;
    }

    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiOfferListItem*  offerItem = (GuiOfferListItem*)item( iRow );
        if( offerItem )
        {
            GuiOfferSession * offerSession = offerItem->getOfferSession();
            if( offerSession && ( offerSession->getMyOnlineId() == onlineId ) )
            {
                return offerItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiOfferListItem* GuiOfferListWidget::findListEntryWidgetByUniqueId( VxGUID& uniqueId )
{
    if( !uniqueId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ERROR GuiOfferListWidget::findListEntryWidgetByOnlineId: invalid uniqueId" );
        return nullptr;
    }

    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiOfferListItem* offerItem = (GuiOfferListItem*)item( iRow );
        if( offerItem )
        {
            GuiOfferSession* offerSession = offerItem->getOfferSession();
            if( offerSession && (offerSession->getUniqueId() == uniqueId) )
            {
                return offerItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
void GuiOfferListWidget::slotItemClicked( QListWidgetItem* item )
{
	if( 300 < m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
	{
        return;
	}

    m_ClickEventTimer.startTimer();
    onOfferListItemClicked( dynamic_cast<GuiOfferListItem *>(item) );
}

//============================================================================
void GuiOfferListWidget::slotUserListItemClicked( GuiOfferListItem* offerItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onOfferListItemClicked( offerItem );
}

//============================================================================
void GuiOfferListWidget::slotAvatarButtonClicked( GuiOfferListItem* offerItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onAvatarButtonClicked(offerItem);
}

//============================================================================
void GuiOfferListWidget::slotFriendshipButtonClicked( GuiOfferListItem* offerItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onFriendshipButtonClicked( offerItem );
}

//============================================================================
void GuiOfferListWidget::slotOfferViewButtonClicked( GuiOfferListItem* offerItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onOfferViewButtonClicked( offerItem );
}

//============================================================================
void GuiOfferListWidget::slotOfferAcceptButtonClicked( GuiOfferListItem* offerItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onOfferAcceptButtonClicked( offerItem );
}

//============================================================================
void GuiOfferListWidget::slotOfferRejectButtonClicked( GuiOfferListItem* offerItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onOfferRejectButtonClicked( offerItem );
}

//============================================================================
void GuiOfferListWidget::slotPushToTalkButtonClicked( GuiOfferListItem* offerItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onPushToTalkButtonClicked( offerItem );
}

//============================================================================
void GuiOfferListWidget::slotMenuButtonClicked( GuiOfferListItem* offerItem )
{
	if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
	{
		return;
	}

	m_ClickEventTimer.startTimer();
    onMenuButtonClicked( offerItem );
}

//============================================================================
void GuiOfferListWidget::callbackUserAdded( GuiUser* guiUser )
{
    updateUser( guiUser );
}

//============================================================================
void GuiOfferListWidget::callbackUserRemoved( VxGUID& onlineId )
{

}

//============================================================================
void GuiOfferListWidget::callbackUserUpdated( GuiUser* guiUser )
{
    updateUser( guiUser );
}

//============================================================================
void GuiOfferListWidget::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
    updateUser( guiUser );
}

//============================================================================
void GuiOfferListWidget::callbackThumbAdded( GuiThumb* guiThumb )
{
    updateThumb( guiThumb );
}

//============================================================================
void GuiOfferListWidget::callbackThumbUpdated( GuiThumb* guiThumb )
{
    updateThumb( guiThumb );
}


//============================================================================
void GuiOfferListWidget::callbackThumbRemoved( VxGUID& thumbId )
{
    // TODO
}

//============================================================================
GuiOfferListItem* GuiOfferListWidget::addOrUpdateSession( GuiOfferSession* offerSession )
{
    GuiOfferListItem* offerItem = findListEntryWidgetBySessionId( offerSession->getOfferSessionId() );
    if( offerItem )
    {
        GuiOfferSession* hostOldSession = offerItem->getOfferSession();
        if( hostOldSession != offerSession )
        {
            offerItem->setOfferSession( offerSession );
        }

        offerItem->updateWidgetFromInfo();
    }
    else
    {
        offerItem = sessionToWidget( offerSession );
        connect( offerSession, SIGNAL(signalOfferUpdated()), this, SLOT(updateWidgetFromInfo()) );

        if( !count() )
        {
            GuiUser* guiUser = offerSession->getUserIdent();
            std::string userName = guiUser->getOnlineName();
            LogMsg( LOG_VERBOSE, "add user %s", offerSession->getUserIdent()->getOnlineName().c_str() );
            addItem( offerItem );
        }
        else
        {
            LogMsg( LOG_VERBOSE, "insert user %s", offerSession->getUserIdent()->getOnlineName().c_str() );
            insertItem( 0, (QListWidgetItem*)offerItem );
        }

        setItemWidget( (QListWidgetItem*)offerItem, (QWidget*)offerItem );
    }

    return offerItem;
}

//============================================================================
void GuiOfferListWidget::onOfferListItemClicked( GuiOfferListItem* offerItem )
{
    LogMsg( LOG_DEBUG, "onOfferListItemClicked" );
    if( offerItem )
    {
        GuiOfferSession* offerSession = offerItem->getOfferSession();
        if( offerSession )
        {
            emit signalOfferListItemClicked( offerSession, offerItem );
        }
    }
}

//============================================================================
void GuiOfferListWidget::onAvatarButtonClicked( GuiOfferListItem* offerItem )
{
    LogMsg( LOG_DEBUG, "onAvatarButtonClicked" );
    if( offerItem )
    {
        GuiOfferSession* offerSession = offerItem->getOfferSession();
        if( offerSession )
        {
            emit signalAvatarButtonClicked( offerSession, offerItem );
            if( offerSession->getUserIdent() )
            {
                emit signalUserAvatarClicked( offerSession->getUserIdent() );
            }
        }
    }
}

//============================================================================
void GuiOfferListWidget::onFriendshipButtonClicked( GuiOfferListItem* offerItem )
{
    LogMsg( LOG_DEBUG, "onAvatarButtonClicked" );
    if( offerItem )
    {
        GuiOfferSession* offerSession = offerItem->getOfferSession();
        if( offerSession )
        {
            emit signalFriendshipButtonClicked( offerSession, offerItem );
        }
    }
}

//============================================================================
void GuiOfferListWidget::onOfferViewButtonClicked( GuiOfferListItem* offerItem )
{
    LogMsg( LOG_VERBOSE, "onOfferViewButtonClicked" );
    if( offerItem )
    {
        GuiOfferSession* offerSession = offerItem->getOfferSession();
        if( offerSession )
        {
            emit signalOfferViewButtonClicked( offerSession, offerItem );
        }
    }
}

//============================================================================
void GuiOfferListWidget::onOfferAcceptButtonClicked( GuiOfferListItem* offerItem )
{
    LogMsg( LOG_VERBOSE, "onOfferAcceptButtonClicked" );
    if( offerItem )
    {
        GuiOfferSession* offerSession = offerItem->getOfferSession();
        if( offerSession )
        {
            emit signalOfferAcceptButtonClicked( offerSession, offerItem );
        }
    }
}

//============================================================================
void GuiOfferListWidget::onOfferRejectButtonClicked( GuiOfferListItem* offerItem )
{
    LogMsg( LOG_VERBOSE, "onOfferRejectButtonClicked" );
    if( offerItem )
    {
        GuiOfferSession* offerSession = offerItem->getOfferSession();
        if( offerSession )
        {
            emit signalOfferRejectButtonClicked( offerSession, offerItem );
        }
    }
}

//============================================================================
void GuiOfferListWidget::onPushToTalkButtonClicked( GuiOfferListItem* offerItem )
{
    LogMsg( LOG_DEBUG, "onPushToTalkButtonClicked" );
    if( offerItem )
    {
        GuiOfferSession* offerSession = offerItem->getOfferSession();
        if( offerSession )
        {
            emit signalPushToTalkButtonClicked( offerSession, offerItem );
        }
    }
}

//============================================================================
void GuiOfferListWidget::onMenuButtonClicked( GuiOfferListItem* offerItem )
{
    LogMsg( LOG_DEBUG, "UserListWidget::onMenuButtonClicked" );
    if( offerItem )
    {
        GuiOfferSession* offerSession = offerItem->getOfferSession();
        if( offerSession )
        {
            AppletPopupMenu* popupMenu = dynamic_cast< AppletPopupMenu* >( m_MyApp.launchApplet( eAppletPopupMenu, dynamic_cast< QWidget* >( this->parent() ) ) );
            if( popupMenu )
            {
                if( getAppletType() == eAppletMultiMessenger )
                {
                    popupMenu->showFriendMenu( offerSession->getUserIdent() );
                }
                else
                {
                    popupMenu->showUserSessionMenu( getAppletType(), offerSession );
                }
            }
        }
    }
}

//============================================================================
void GuiOfferListWidget::onListItemAdded( GuiOfferSession* offerSession, GuiOfferListItem* offerItem )
{
    onListItemUpdated( offerSession, offerItem );
}

//============================================================================
void GuiOfferListWidget::onListItemUpdated( GuiOfferSession* offerSession, GuiOfferListItem* offerItem )
{
    if( offerSession && offerItem && offerSession->getUserIdent() )
    {
        if( !offerItem->getIsThumbUpdated() )
        {
            VxPushButton* avatarButton = offerItem->getAvatarButton();
            GuiUser* guiUser = offerSession->getUserIdent();

            QImage	avatarImage;
            bool havAvatarImage = m_ThumbMgr.requestAvatarImage( guiUser, offerSession->getPluginType(), avatarImage, false );
            if( havAvatarImage && avatarButton )
            {
                avatarButton->setIconOverrideImage( avatarImage );
                offerItem->setIsThumbUpdated( true );
            }
        }

        offerItem->updateWidgetFromInfo();
    }
}

//============================================================================
void GuiOfferListWidget::updateThumb( GuiThumb* guiThumb )
{
    GuiOfferListItem* offerItem = findListEntryWidgetByOnlineId( guiThumb->getCreatorId() );
    if( offerItem )
    {
        offerItem->updateThumb( guiThumb );
    }
}
