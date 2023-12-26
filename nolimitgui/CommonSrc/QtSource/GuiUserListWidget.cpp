//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "GuiUserListItem.h"
#include "GuiUser.h"
#include "GuiUserListWidget.h"
#include "GuiUserSessionBase.h"

#include "MyIcons.h"
#include "AppletPopupMenu.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>

#include <vector>
 
//============================================================================
GuiUserListWidget::GuiUserListWidget( QWidget* parent )
: ListWidgetBase( parent )
{
	QListWidget::setSortingEnabled( true );
	sortItems( Qt::DescendingOrder );

    GetAppInstance().getUserMgr().wantGuiUserUpdateCallbacks( this, true );
    GetAppInstance().getThumbMgr().wantGuiThumbCallbacks( this, true );
    GetAppInstance().getMemberActiveMgr().wantMemberActiveCallback( this, true );
}

//============================================================================
GuiUserListWidget::~GuiUserListWidget()
{
    disconnectUserUpdates();
}

//============================================================================
void GuiUserListWidget::disconnectUserUpdates( void )
{
    GetAppInstance().getMemberActiveMgr().wantMemberActiveCallback( this, false );
    GetAppInstance().getThumbMgr().wantGuiThumbCallbacks( this, false );
    GetAppInstance().getUserMgr().wantGuiUserUpdateCallbacks( this, false );
}

//============================================================================
void GuiUserListWidget::callbackMyIdentUpdated( GuiUser* guiUser )
{
    if( guiUser->getMyOnlineId() != getHostAdminId().getHostOnlineId() )
    {
        callbackUserUpdated( guiUser );
    }
}

//============================================================================
void GuiUserListWidget::setUserViewType( EUserViewType viewType )
{
    m_ViewType = viewType;
    refreshUserList();
}

//============================================================================
void GuiUserListWidget::refreshUserList( void )
{
    clearUserList();
    std::vector<GuiUser*> updateUserList;
    std::map<VxGUID, GuiUser*>& userList = m_UserMgr.getUserList();
    for( auto iter = userList.begin(); iter != userList.end(); ++iter )
    {
        if( isListViewMatch( iter->second ) )
        {
            updateUserList.push_back( iter->second );
        }  
    }

    for( auto user : updateUserList )
    {
        updateUser( user );
    }

    update();
}

//============================================================================
void GuiUserListWidget::clearUserList( void )
{
    m_UserCache.clear();
    for( int i = 0; i < count(); ++i )
    {
        QListWidgetItem* listItem = item( i );
        GuiUserListItem* userItem = dynamic_cast<GuiUserListItem*>(listItem);
        if( userItem )
        {
            GuiUserSessionBase* userSession = userItem->getUserSession();
            if( userSession )
            {
                userSession->deleteLater();
            }

            userItem->deleteLater();
        }
    }

    clear();
}

//============================================================================
GuiUserListItem* GuiUserListWidget::sessionToWidget( GuiUserSessionBase* userSession )
{
    GuiUserListItem* userItem = new GuiUserListItem(this);
    userItem->setSizeHint( userItem->calculateSizeHint() );

    userItem->setUserSession( userSession );

    //connect( userItem, SIGNAL( signalGuiUserListItemClicked(GuiUserListItem*) ), this, SLOT( slotUserListItemClicked(GuiUserListItem*) ) );
    connect( userItem, SIGNAL(signalAvatarButtonClicked(GuiUserListItem*)),     this, SLOT(slotAvatarButtonClicked(GuiUserListItem*)) );
    connect( userItem, SIGNAL(signalFriendshipButtonClicked(GuiUserListItem*)), this, SLOT(slotFriendshipButtonClicked(GuiUserListItem*)) );
    connect( userItem, SIGNAL(signalOfferViewButtonClicked(GuiUserListItem*)),  this, SLOT(slotOfferViewButtonClicked(GuiUserListItem*)) );
    connect( userItem, SIGNAL(signalOfferAcceptButtonClicked(GuiUserListItem*)), this, SLOT(slotOfferAcceptButtonClicked(GuiUserListItem*)) );
    connect( userItem, SIGNAL(signalOfferRejectButtonClicked(GuiUserListItem*)), this, SLOT(slotOfferRejectButtonClicked(GuiUserListItem*)) );
    connect( userItem, SIGNAL(signalPushToTalkButtonClicked(GuiUserListItem*)), this, SLOT(slotPushToTalkButtonClicked(GuiUserListItem*)) );
    connect( userItem, SIGNAL(signalMenuButtonClicked(GuiUserListItem*)),       this, SLOT(slotMenuButtonClicked(GuiUserListItem*)) );

    userItem->updateWidgetFromInfo();

    return userItem;
}

//============================================================================
GuiUserSessionBase* GuiUserListWidget::widgetToSession( GuiUserListItem * item )
{
    return item->getUserSession();
}

//============================================================================
void GuiUserListWidget::updateUser( VxGUID& onlineId )
{
    GuiUser* guiUser = m_MyApp.getUserMgr().getUser(onlineId);
    updateUser( guiUser );
}

//============================================================================
void GuiUserListWidget::updateUser( GuiUser* guiUser )
{
    if( guiUser )
    {   
        if( isListViewMatch( guiUser ) )
        {
            LogMsg( LOG_DEBUG, "GuiUserListWidget::updateUser user %s %s", 
                    guiUser->getOnlineName().c_str(), guiUser->getMyOnlineId().toOnlineIdString().c_str());
            if( guiUser->getNetIdent().isValidNetIdent() )
            {
                GuiUserSessionBase* userSession = nullptr;
                auto iter = m_UserCache.find( guiUser->getMyOnlineId() );
                if( iter == m_UserCache.end() )
                {
                    GuiUserListItem* userItem = findListEntryWidgetByOnlineId( guiUser->getMyOnlineId() );

                    if( !userItem )
                    {
                        if( guiUser->isOnline() || (!guiUser->isOnline() && m_ViewType == eUserViewTypeOffline) )
                        {
                            LogMsg( LOG_DEBUG, "GuiUserListWidget::updateUser new user %s", guiUser->getOnlineName().c_str() );
                            GuiUserSessionBase* userSession = makeSession( guiUser );
                            if( userSession )
                            {
                                m_UserCache[guiUser->getMyOnlineId()] = userSession;
                                GuiUserListItem* entryWidget = sessionToWidget( userSession );
                                if( 0 == count() )
                                {
                                    LogMsg( LOG_INFO, "add user %s", guiUser->getOnlineName().c_str() );
                                    addItem( entryWidget );
                                }
                                else
                                {
                                    LogMsg( LOG_INFO, "insert user %s", guiUser->getOnlineName().c_str() );
                                    insertItem( 0, (QListWidgetItem*)entryWidget );
                                }

                                setItemWidget( (QListWidgetItem*)entryWidget, (QWidget*)entryWidget );
                                onListItemAdded( userSession, entryWidget );
                            }
                            else
                            {
                                LogMsg( LOG_ERROR, "GuiUserListWidget::updateUser failed create session for user %s", guiUser->getOnlineName().c_str() );
                            }
                        }
                        else
                        {
                            LogMsg( LOG_ERROR, "GuiUserListWidget::updateUser user %s is no longer online", guiUser->getOnlineName().c_str() );
                        }
                    }
                    else
                    {
                        LogMsg( LOG_ERROR, "GuiUserListWidget::updateUser entryWidget exists but is not in cache %s", guiUser->getOnlineName().c_str() );
                        if( userItem )
                        {
                            GuiUserSessionBase* userSession = userItem->getUserSession();
                            if( userSession )
                            {
                                onListItemUpdated( userItem->getUserSession(), userItem );
                            }

                            userItem->update();
                        }
                    } 
                }
                else
                {
                    GuiUserListItem* userItem = findListEntryWidgetByOnlineId( guiUser->getMyOnlineId() );
                    if( userItem )
                    {
                        GuiUserSessionBase* userSession = userItem->getUserSession();
                        if( userSession )
                        {
                            onListItemUpdated( userItem->getUserSession(), userItem );
                        }

                        userItem->update();
                    }
                }
            }
            else
            {
                LogMsg( LOG_ERROR, "GuiUserListWidget::updateUser invalid net ident" );
            }
        } // no need for else.. is just not a match
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiUserListWidget::updateUser null gui user" );
    }
}

//============================================================================
void GuiUserListWidget::updateEntryWidget( VxGUID& onlineId )
{
    GuiUserListItem* entryWidget = findListEntryWidgetByOnlineId( onlineId );
    if( entryWidget )
    {
        entryWidget->update();
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiUserListWidget::updateEntryWidget failed to find session for user" );
    }
}

//============================================================================
GuiUserSessionBase* GuiUserListWidget::makeSession( GuiUser* guiUser )
{
    return new GuiUserSessionBase( guiUser, this );
}

//============================================================================
void GuiUserListWidget::removeUser( VxGUID& onlineId )
{
    auto iter = m_UserCache.find( onlineId );
    if( iter != m_UserCache.end() )
    {
        LogMsg( LOG_DEBUG, "GuiUserListWidget::removeUser %s", iter->second->getOnlineName().c_str() );
        m_UserCache.erase( iter );

        GuiUserListItem* userItem = findListEntryWidgetByOnlineId( onlineId );
        if( userItem )
        {
            QListWidgetItem* listItem = dynamic_cast<QListWidgetItem*>( userItem );
            removeItemWidget(listItem);
            delete listItem; // Qt documentation warnings you to destroy item to effectively remove it from QListWidget.
        }  
    }
}

//============================================================================
GuiUserSessionBase* GuiUserListWidget::findSession( VxGUID& lclSessionId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiUserListItem* listItem =  (GuiUserListItem*)item( iRow );
        if( listItem )
        {
            GuiUserSessionBase * userSession = listItem->getUserSession();
            if( userSession && userSession->getOfferSessionId() == lclSessionId )
            {
                return userSession;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiUserListItem* GuiUserListWidget::findListEntryWidgetBySessionId( VxGUID& sessionId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiUserListItem*  userItem = (GuiUserListItem*)item( iRow );
        if( userItem )
        {
            GuiUserSessionBase * userSession = userItem->getUserSession();
            if( userSession && ( userSession->getOfferSessionId() == sessionId ) )
            {
                return userItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiUserListItem* GuiUserListWidget::findListEntryWidgetByOnlineId( VxGUID& onlineId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ERROR GuiUserListWidget::findListEntryWidgetByOnlineId: invalid online id" );
        return nullptr;
    }

    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiUserListItem*  userItem = (GuiUserListItem*)item( iRow );
        if( userItem )
        {
            GuiUserSessionBase * userSession = userItem->getUserSession();
            if( userSession && ( userSession->getMyOnlineId() == onlineId ) )
            {
                return userItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
void GuiUserListWidget::slotItemClicked( QListWidgetItem* item )
{
	if( 300 < m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
	{
        return;
	}

    m_ClickEventTimer.startTimer();
    onUserListItemClicked( dynamic_cast<GuiUserListItem *>(item) );
}

//============================================================================
void GuiUserListWidget::slotUserListItemClicked( GuiUserListItem* userItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onUserListItemClicked( userItem );
}

//============================================================================
void GuiUserListWidget::slotAvatarButtonClicked( GuiUserListItem* userItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onAvatarButtonClicked(userItem);
}

//============================================================================
void GuiUserListWidget::slotFriendshipButtonClicked( GuiUserListItem* userItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onFriendshipButtonClicked( userItem );
}

//============================================================================
void GuiUserListWidget::slotOfferViewButtonClicked( GuiUserListItem* userItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onOfferViewButtonClicked( userItem );
}

//============================================================================
void GuiUserListWidget::slotOfferAcceptButtonClicked( GuiUserListItem* userItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onOfferAcceptButtonClicked( userItem );
}

//============================================================================
void GuiUserListWidget::slotOfferRejectButtonClicked( GuiUserListItem* userItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onOfferRejectButtonClicked( userItem );
}

//============================================================================
void GuiUserListWidget::slotPushToTalkButtonClicked( GuiUserListItem* userItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onPushToTalkButtonClicked( userItem );
}

//============================================================================
void GuiUserListWidget::slotMenuButtonClicked( GuiUserListItem* userItem )
{
	if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
	{
		return;
	}

	m_ClickEventTimer.startTimer();
    onMenuButtonClicked( userItem );
}

//============================================================================
void GuiUserListWidget::callbackUserAdded( GuiUser* guiUser )
{
    updateUser( guiUser );
}

//============================================================================
void GuiUserListWidget::callbackUserRemoved( VxGUID& onlineId )
{
    removeUser( onlineId );
}

//============================================================================
void GuiUserListWidget::callbackUserUpdated( GuiUser* guiUser )
{
    updateUser( guiUser );
}

//============================================================================
void GuiUserListWidget::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
    updateUser( guiUser );
}

//============================================================================
void GuiUserListWidget::callbackThumbAdded( GuiThumb* guiThumb )
{
    updateThumb( guiThumb );
}

//============================================================================
void GuiUserListWidget::callbackThumbUpdated( GuiThumb* guiThumb )
{
    updateThumb( guiThumb );
}


//============================================================================
void GuiUserListWidget::callbackThumbRemoved( VxGUID& thumbId )
{
    // TODO
}

//============================================================================
void GuiUserListWidget::addSessionToList( EHostType hostType, VxGUID& sessionId, GuiUser* hostIdent )
{
    GuiUserSessionBase* userSession = new GuiUserSessionBase( hostType, sessionId, hostIdent, this );

    addOrUpdateSession( userSession );
}

//============================================================================
GuiUserListItem* GuiUserListWidget::addOrUpdateSession( GuiUserSessionBase* userSession )
{
    GuiUserListItem* userItem = findListEntryWidgetBySessionId( userSession->getOfferSessionId() );
    if( userItem )
    {
        GuiUserSessionBase* hostOldSession = userItem->getUserSession();
        if( hostOldSession != userSession )
        {
            userItem->setUserSession( userSession );
            if( !hostOldSession->parent() )
            {
                delete hostOldSession;
            }
        }

        userItem->updateWidgetFromInfo();
    }
    else
    {
        userItem = sessionToWidget( userSession );
        if( 0 == count() )
        {
            LogMsg( LOG_VERBOSE, "add user %s", userSession->getUserIdent()->getOnlineName().c_str() );
            addItem( userItem );
        }
        else
        {
            LogMsg( LOG_VERBOSE, "insert user %s", userSession->getUserIdent()->getOnlineName().c_str() );
            insertItem( 0, (QListWidgetItem*)userItem );
        }

        setItemWidget( (QListWidgetItem*)userItem, (QWidget*)userItem );
    }

    return userItem;
}

//============================================================================
void GuiUserListWidget::onUserListItemClicked( GuiUserListItem* userItem )
{
    LogMsg( LOG_DEBUG, "onHostListItemClicked" );
    if( userItem )
    {
        GuiUserSessionBase* userSession = userItem->getUserSession();
        if( userSession )
        {
            emit signalUserListItemClicked( userSession, userItem );
        }
    }
}

//============================================================================
void GuiUserListWidget::onAvatarButtonClicked( GuiUserListItem* userItem )
{
    LogMsg( LOG_DEBUG, "onAvatarButtonClicked" );
    if( userItem )
    {
        GuiUserSessionBase* userSession = userItem->getUserSession();
        if( userSession )
        {
            emit signalAvatarButtonClicked( userSession, userItem );
            if( userSession->getUserIdent() )
            {
                emit signalUserAvatarClicked( userSession->getUserIdent() );
            }
        }
    }
}

//============================================================================
void GuiUserListWidget::onFriendshipButtonClicked( GuiUserListItem* userItem )
{
    LogMsg( LOG_DEBUG, "onAvatarButtonClicked" );
    if( userItem )
    {
        GuiUserSessionBase* userSession = userItem->getUserSession();
        if( userSession )
        {
            emit signalFriendshipButtonClicked( userSession, userItem );
        }
    }
}

//============================================================================
void GuiUserListWidget::onOfferViewButtonClicked( GuiUserListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "onOfferViewButtonClicked" );
    if( userItem )
    {
        GuiUserSessionBase* userSession = userItem->getUserSession();
        if( userSession )
        {
            emit signalOfferViewButtonClicked( userSession, userItem );
        }
    }
}

//============================================================================
void GuiUserListWidget::onOfferAcceptButtonClicked( GuiUserListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "onOfferAcceptButtonClicked" );
    if( userItem )
    {
        GuiUserSessionBase* userSession = userItem->getUserSession();
        if( userSession )
        {
            emit signalOfferAcceptButtonClicked( userSession, userItem );
        }
    }
}

//============================================================================
void GuiUserListWidget::onOfferRejectButtonClicked( GuiUserListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "onOfferRejectButtonClicked" );
    if( userItem )
    {
        GuiUserSessionBase* userSession = userItem->getUserSession();
        if( userSession )
        {
            emit signalOfferRejectButtonClicked( userSession, userItem );
        }
    }
}

//============================================================================
void GuiUserListWidget::onPushToTalkButtonClicked( GuiUserListItem* userItem )
{
    LogMsg( LOG_DEBUG, "onPushToTalkButtonClicked" );
    if( userItem )
    {
        GuiUserSessionBase* userSession = userItem->getUserSession();
        if( userSession )
        {
            emit signalPushToTalkButtonClicked( userSession, userItem );
        }
    }
}

//============================================================================
void GuiUserListWidget::onMenuButtonClicked( GuiUserListItem* userItem )
{
    LogMsg( LOG_DEBUG, "UserListWidget::onMenuButtonClicked" );
    if( userItem )
    {
        GuiUserSessionBase* userSession = userItem->getUserSession();
        if( userSession )
        {
            AppletPopupMenu* popupMenu = dynamic_cast< AppletPopupMenu* >( m_MyApp.launchApplet( eAppletPopupMenu, dynamic_cast< QWidget* >( this->parent() ) ) );
            if( popupMenu )
            {
                if( getAppletType() == eAppletMultiMessenger )
                {
                    popupMenu->showFriendMenu( userSession->getUserIdent() );
                }
                else
                {
                    popupMenu->showUserSessionMenu( getAppletType(), userSession );
                }
            }
        }
    }
}

//============================================================================
bool GuiUserListWidget::isMemberView( void )
{
    return IsHostedMembersViewType( getUserViewType() );
}

//============================================================================
bool GuiUserListWidget::isListViewMatch( GuiUser* guiUser )
{
    EUserViewType viewType = getUserViewType();
    if( guiUser && !guiUser->isIgnored() )
    {
        if( guiUser->isMyself() )
        {
            return getAllowMyselfInList() &&
                   (!getHostAdminId().isValid() || guiUser->getMyOnlineId() != getHostAdminId().getHostOnlineId());
        }
        else if( eUserViewTypeEverybody == viewType )
        {
            return true;
        }
        else if( eUserViewTypeOnline == viewType )
        {
            return guiUser->isOnline();
        }
        else if( eUserViewTypeDirectConnect == viewType )
        {
            return guiUser->isOnline() && guiUser->isDirectConnect();
        }
        else if( eUserViewTypeFriends == viewType )
        {
            return guiUser->isFriend() || guiUser->isAdmin();
        }
        else if( eUserViewTypeGroup == viewType )
        {
            if( getHostAdminId().isValid() )
            {
                GroupieId memberId( guiUser->getMyOnlineId(), getHostAdminId().getHostedId() );
                return !guiUser->isAnonymous() && m_MyApp.getMemberActiveMgr().isMemberActive( memberId );
            }
            else
            {
                 return !guiUser->isAnonymous() && m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeGroup, guiUser->getMyOnlineId() );
            }
        }
        else if( eUserViewTypeChatRoom == viewType )
        {
            if( getHostAdminId().isValid() )
            {
                GroupieId memberId( guiUser->getMyOnlineId(), getHostAdminId().getHostedId() );
                return !guiUser->isAnonymous() && m_MyApp.getMemberActiveMgr().isMemberActive( memberId );
            }
            else
            {
                 return !guiUser->isAnonymous() && m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeChatRoom, guiUser->getMyOnlineId() );
            }
        }
        else if( eUserViewTypeRandomConnect == viewType )
        {
            if( getHostAdminId().isValid() )
            {
                GroupieId memberId( guiUser->getMyOnlineId(), getHostAdminId().getHostedId() );
                return !guiUser->isAnonymous() && m_MyApp.getMemberActiveMgr().isMemberActive( memberId );
            }
            else
            {
                 return !guiUser->isAnonymous() && m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeRandomConnect, guiUser->getMyOnlineId() );
            }
        }
        else if( eUserViewTypeNearby == viewType )
        {
            return guiUser->isNearby();
        }
        else if( eUserViewTypeOffline == viewType )
        {
            return !guiUser->isOnline() && guiUser->isFriend() || guiUser->isAdmin();
        }

        if( guiUser->isDirectConnect() && guiUser->isFriend() )
        {
            return true;
        }
    }
    else if( guiUser && guiUser->isIgnored() && eUserViewTypeIgnored == viewType )
    {
        return true;
    }

    return false;
}

//============================================================================
void GuiUserListWidget::onListItemAdded( GuiUserSessionBase* userSession, GuiUserListItem* userItem )
{
    onListItemUpdated( userSession, userItem );
}

//============================================================================
void GuiUserListWidget::onListItemUpdated( GuiUserSessionBase* userSession, GuiUserListItem* userItem )
{
    if( userSession && userItem && userSession->getUserIdent() )
    {
        EPluginType pluginType = ePluginTypeInvalid;
        switch( getUserViewType() )
        {
        case eUserViewTypeGroup:
            pluginType = ePluginTypeClientGroup;
            break;
        case eUserViewTypeChatRoom:
            pluginType = ePluginTypeClientChatRoom;
            break;
        case eUserViewTypeRandomConnect:
            pluginType = ePluginTypeClientRandomConnect;
            break;
        default:
            pluginType = ePluginTypeHostPeerUser;
        }

        if( !userItem->getIsThumbUpdated() )
        {
            VxPushButton* avatarButton = userItem->getAvatarButton();
            GuiUser* guiUser = userSession->getUserIdent();

            QImage	avatarImage;
            bool havAvatarImage = m_ThumbMgr.requestAvatarImage( guiUser, pluginType, avatarImage, true );
            if( havAvatarImage && avatarButton )
            {
                avatarButton->setIconOverrideImage( avatarImage );
                userItem->setIsThumbUpdated( true );
            }
        }

        userItem->updateWidgetFromInfo();
        if( isSortingEnabled() )
        {
            //sortItems();
        }
    }
}

//============================================================================
void GuiUserListWidget::updateThumb( GuiThumb* guiThumb )
{
    // commented out.. bad idea without verifing is correct user.. thumb could have been creaed by anyone
    /*
    GuiUserListItem* userItem = findListEntryWidgetByOnlineId( guiThumb->getCreatorId() );
    if( userItem )
    {
        userItem->updateThumb( guiThumb );
    }
    */
}

//============================================================================
void GuiUserListWidget::callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
    GuiUserListItem* userItem = findListEntryWidgetByOnlineId( onlineId );
    if( userItem )
    {
        userItem->callbackPushToTalkStatus( onlineId, pushToTalkStatus );
    }
}

//============================================================================
void GuiUserListWidget::callbackGuiMemberActive( GroupieId& groupieId, bool isActive )
{
    if( isActive && isMemberView() && ( groupieId.getHostedId() == m_HostAdminId.getHostedId() || !m_HostAdminId.isValid() ) )
    {
        VxGUID onlineId = groupieId.getUserOnlineId();
        if( onlineId != groupieId.getHostOnlineId() )
        {
            updateUser( onlineId );
        }
    }
}

//============================================================================
void GuiUserListWidget::callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    updateUser( onlineId );
}

//============================================================================
void GuiUserListWidget::searchTextChanged( QString& searchText )
{
    int rowCnt = count();
    for( int rowIdx = 0; rowIdx < rowCnt; rowIdx++ )
    {
        GuiUserListItem* userItem = (GuiUserListItem*)item( rowIdx );
        if( userItem )
        {
            if( searchText.isEmpty() )
            {
                userItem->setVisible( true );
            }
            else
            {
                GuiUser* guiUser = userItem->getGuiUser();
                if( guiUser && guiUser->onlineNameMatch( searchText ) )
                {
                    userItem->setVisible( true );
                }
                else
                {
                    userItem->setVisible( false );
                }
            }
        }
    }
}
