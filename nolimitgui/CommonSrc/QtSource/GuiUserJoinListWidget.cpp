//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "GuiUserJoinListItem.h"
#include "GuiUserJoinListWidget.h"
#include "GuiUserJoinMgr.h"
#include "GuiUserJoinSession.h"

#include "MyIcons.h"
#include "AppletPopupMenu.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>
 
//============================================================================
GuiUserJoinListWidget::GuiUserJoinListWidget( QWidget* parent )
: QListWidget( parent )
, m_MyApp( GetAppInstance() )
, m_Engine( m_MyApp.getEngine() )
, m_UserJoinMgr( m_MyApp.getUserJoinMgr() )
, m_ThumbMgr( m_MyApp.getThumbMgr() )
{
	QListWidget::setSortingEnabled( true );
	sortItems( Qt::DescendingOrder );

    connect( &m_UserJoinMgr, SIGNAL(signalMyIdentUpdated( GuiUserJoin*)),               this, SLOT(callbackMyIdentUpdated( GuiUserJoin*))) ;

    connect( &m_UserJoinMgr, SIGNAL(signalUserJoinAdded( GuiUserJoin*)),                this, SLOT(slotUserJoinAdded( GuiUserJoin*))) ;
    connect( &m_UserJoinMgr, SIGNAL(signalUserJoinRemoved( VxGUID)),                    this, SLOT(slotUserJoinRemoved( VxGUID))) ;
    connect( &m_UserJoinMgr, SIGNAL(signalUserJoinUpdated( GuiUserJoin*)),              this, SLOT(slotUserJoinUpdated( GuiUserJoin*))) ;
    connect( &m_UserJoinMgr, SIGNAL(signalUserJoinOnlineStatus( GuiUserJoin*,bool)),    this, SLOT(slotUserJoinOnlineStatus( GuiUserJoin*,bool))) ;

    connect( &m_ThumbMgr, SIGNAL( signalThumbAdded( GuiThumb* ) ),          this, SLOT( callbackThumbAdded( GuiThumb* ) ) );
    connect( &m_ThumbMgr, SIGNAL(signalThumbUpdated( GuiThumb*)),           this, SLOT(callbackThumbUpdated( GuiThumb*))) ;
    connect( &m_ThumbMgr, SIGNAL(signalThumbRemoved( VxGUID)),              this, SLOT(callbackThumbRemoved( VxGUID))) ;


    //connect( this, SIGNAL(itemClicked(QListWidgetItem*)),          this, SLOT(slotItemClicked(QListWidgetItem*))) ;
    //connect( this, SIGNAL(itemDoubleClicked(QListWidgetItem*)),    this, SLOT(slotItemClicked(QListWidgetItem*))) ;

    setUserJoinViewType( eUserJoinViewTypeEverybody );
    m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, true );
    m_ThumbMgr.wantGuiThumbCallbacks( this, true );
}

//============================================================================
GuiUserJoinListWidget::~GuiUserJoinListWidget()
{
    m_ThumbMgr.wantGuiThumbCallbacks( this, true );
    m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, false );
}

//============================================================================
GuiUserJoinListItem* GuiUserJoinListWidget::sessionToWidget( GuiUserJoinSession* userSession )
{
    GuiUserJoinListItem* userItem = new GuiUserJoinListItem(this);
    userItem->setUserJoinSession( userSession );
    userItem->setSizeHint( userItem->calculateSizeHint() );

    connect( userItem, SIGNAL(signalUserJoinListItemClicked(QListWidgetItem*)),	            this, SLOT(slotUserJoinListItemClicked(QListWidgetItem*)) );
    connect( userItem, SIGNAL(signalAvatarButtonClicked(GuiUserJoinListItem*)),	        this, SLOT(slotAvatarButtonClicked(GuiUserJoinListItem*)) );
    connect( userItem, SIGNAL(signalMenuButtonClicked(GuiUserJoinListItem*)),	            this, SLOT(slotMenuButtonClicked(GuiUserJoinListItem*)) );
    connect( userItem, SIGNAL(signalFriendshipButtonClicked(GuiUserJoinListItem*)),		this, SLOT(slotFriendshipButtonClicked(GuiUserJoinListItem*)) );

    userItem->updateWidgetFromInfo();

    return userItem;
}

//============================================================================
GuiUserJoinListItem* GuiUserJoinListWidget::findListEntryWidgetBySessionId( VxGUID& sessionId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiUserJoinListItem*  userItem = (GuiUserJoinListItem*)item( iRow );
        if( userItem )
        {
            GuiUserJoinSession * userSession = userItem->getUserJoinSession();
            if( userSession && ( userSession->getSessionId() == sessionId ) )
            {
                return userItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
GuiUserJoinListItem* GuiUserJoinListWidget::findListEntryWidgetByGroupieId( GroupieId& groupieId )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiUserJoinListItem*  userItem = (GuiUserJoinListItem*)item( iRow );
        if( userItem )
        {
            GuiUserJoinSession * userSession = userItem->getUserJoinSession();
            if( userSession && ( userSession->getGroupieId() == groupieId ) )
            {
                return userItem;
            }
        }
    }

    return nullptr;
}

//============================================================================
void GuiUserJoinListWidget::slotAvatarButtonClicked( GuiUserJoinListItem* userItem )
{
    if( 300 > m_ClickEventTimer.elapsedMs() ) // avoid duplicate clicks
    {
        return;
    }

    m_ClickEventTimer.startTimer();
    onAvatarButtonClicked(userItem);
}

//============================================================================
void GuiUserJoinListWidget::slotMenuButtonClicked( GuiUserJoinListItem* userItem )
{
	if( 300 > m_ClickEventTimer.elapsedMs()  ) // avoid duplicate clicks
	{
		return;
	}

	m_ClickEventTimer.startTimer();
    onMenuButtonClicked( userItem );
}

//============================================================================
GuiUserJoinListItem* GuiUserJoinListWidget::addOrUpdateUserJoinSession( GuiUserJoinSession* userSession )
{
    GuiUserJoinListItem* userItem = findListEntryWidgetBySessionId( userSession->getSessionId() );
    if( userItem )
    {
        GuiUserJoinSession* userOldSession = userItem->getUserJoinSession();
        if( userOldSession != userSession )
        {
            userItem->setUserJoinSession( userSession );
            if( !userOldSession->parent() )
            {
                delete userOldSession;
            }
        }

        userItem->updateWidgetFromInfo();
    }
    else
    {
        userItem = sessionToWidget( userSession );
        if( 0 == count() )
        {
            LogMsg( LOG_INFO, "add user %s", userSession->getOnlineName().c_str() );
            addItem( userItem );
        }
        else
        {
            LogMsg( LOG_INFO, "insert user %s", userSession->getOnlineName().c_str() );
            insertItem( 0, (QListWidgetItem*)userItem );
        }

        setItemWidget( (QListWidgetItem*)userItem, (QWidget*)userItem );
        onListItemAdded( userSession, userItem );
    }

    return userItem;
}

//============================================================================
void GuiUserJoinListWidget::onAvatarButtonClicked( GuiUserJoinListItem* userItem )
{
    LogMsg( LOG_DEBUG, "onAvatarButtonClicked" );
    if( userItem )
    {
        GuiUserJoinSession* userSession = userItem->getUserJoinSession();
        if( userSession )
        {
            emit signalAvatarButtonClicked( userSession, userItem );
        }
    }
}

//============================================================================
void GuiUserJoinListWidget::onMenuButtonClicked( GuiUserJoinListItem* userItem )
{
    LogMsg( LOG_DEBUG, "onMenuButtonClicked" );
    if( userItem )
    {
        GuiUserJoinSession* userSession = userItem->getUserJoinSession();
        if( userSession )
        {
            emit signalMenuButtonClicked( userSession, userItem );

            if( userSession->getGuiUser() )
            {
                AppletPopupMenu* popupMenu = dynamic_cast<AppletPopupMenu*>(m_MyApp.launchApplet( eAppletPopupMenu, dynamic_cast<QWidget*>(this->parent()) ));
                if( popupMenu )
                {
                    popupMenu->showFriendMenu( userSession->getGuiUser() );
                }
            }
        }
    }
}

//============================================================================
void GuiUserJoinListWidget::slotUserJoinAdded( GuiUserJoin* guiUserJoin )
{
    updateUserJoin( guiUserJoin );
}

//============================================================================
void GuiUserJoinListWidget::slotUserJoinRemoved( GroupieId& groupieId )
{
    removeUserJoin( groupieId );
}

//============================================================================
void GuiUserJoinListWidget::slotUserJoinUpdated( GuiUserJoin* guiUserJoin )
{
    updateUserJoin( guiUserJoin );
}

//============================================================================
void GuiUserJoinListWidget::slotUserJoinOnlineStatus( GuiUserJoin* user, bool isOnline )
{
    updateUserJoin( user );
}

//============================================================================
void GuiUserJoinListWidget::callbackThumbAdded( GuiThumb* guiThumb )
{
    updateThumb( guiThumb );
}

//============================================================================
void GuiUserJoinListWidget::callbackThumbUpdated( GuiThumb* guiThumb )
{
    updateThumb( guiThumb );
}

//============================================================================
void GuiUserJoinListWidget::callbackThumbRemoved( VxGUID& thumbId )
{
    // TODO
}

//============================================================================
void GuiUserJoinListWidget::setUserJoinViewType( EUserJoinViewType viewType )
{
    if( viewType != m_UserJoinViewType )
    {
        m_UserJoinViewType = viewType;
        refreshList();
    }
}

//============================================================================
void GuiUserJoinListWidget::refreshList( void )
{
    clearUserJoinList();
    std::vector<GuiUserJoin *> userList;

    if( isListViewMatch( m_MyApp.getUserMgr().getMyIdent() ) )
    {
        // temporary for dev.. add ourself
        GuiUserJoin* myself = m_UserJoinMgr.updateUserJoin( &m_MyApp.getUserMgr().getMyIdent()->getNetIdent() );
        userList.push_back( myself );
    }

    std::map<GroupieId, GuiUserJoin*>& mgrList = m_UserJoinMgr.getUserJoinList();
    for( auto iter = mgrList.begin(); iter != mgrList.end(); ++iter )
    {
        if( isListViewMatch( iter->second->getUser() ) )
        {
            userList.push_back( iter->second );
        }
    }

    for( auto user : userList )
    {
        updateUserJoin( user );
    }
}


//============================================================================
void GuiUserJoinListWidget::clearUserJoinList( void )
{
    m_UserJoinCache.clear();
    for(int i = 0; i < count(); ++i)
    {
        QListWidgetItem* userItem = item(i);
        delete ((GuiUserJoinListItem*)userItem);
    }

    clear();
}

//============================================================================
bool GuiUserJoinListWidget::isListViewMatch( GuiUser* guiUser )
{
    if( guiUser && !guiUser->isIgnored())
    {
        if( guiUser->isMyself() )
        { 
            return true;
        }
        else if( eUserJoinViewTypeEverybody == getUserJoinViewType() )
        {
            return true;
        }
        else if( eUserJoinViewTypeFriends == getUserJoinViewType() )
        {
            return guiUser->isFriend() || guiUser->isAdmin();
        }
        else if( eUserJoinViewTypeGroup == getUserJoinViewType() )
        {
            return guiUser->isGroupHosted() && !guiUser->isAnonymous();
        }
        else if( eUserJoinViewTypeChatRoom == getUserJoinViewType() )
        {
            return guiUser->isChatRoomHosted() && !guiUser->isAnonymous();
        }
        else if( eUserJoinViewTypeRandomConnect == getUserJoinViewType() )
        {
            return guiUser->isRandomConnectHosted() && !guiUser->isAnonymous();
        }

        if( guiUser->isDirectConnect() && guiUser->isFriend() )
        {
            return true;
        }
    }
    else if( guiUser && guiUser->isIgnored() && eUserJoinViewTypeIgnored == getUserJoinViewType() )
    {
        return true;
    }

    return false;
}

//============================================================================
void GuiUserJoinListWidget::updateUserJoin( GuiUserJoin * guiUserJoin )
{
    if( isListViewMatch( guiUserJoin->getUser() ) )
    {
        auto iter = m_UserJoinCache.find( guiUserJoin->getGroupieId() );
        if( iter == m_UserJoinCache.end() )
        {
            GuiUserJoinSession * userSession = new GuiUserJoinSession( guiUserJoin, this );
            if( userSession )
            {
                GuiUserJoinListItem* userItem = sessionToWidget( userSession );
                if( 0 == count() )
                {
                    LogMsg( LOG_INFO, "add user %s", userSession->getOnlineName().c_str() );
                    addItem( userItem );
                }
                else
                {
                    LogMsg( LOG_INFO, "insert user %s", userSession->getOnlineName().c_str() );
                    insertItem( 0, (QListWidgetItem*)userItem );
                }

                setItemWidget( (QListWidgetItem*)userItem, (QWidget*)userItem );
                m_UserJoinCache[guiUserJoin->getGroupieId()] = userSession;
                onListItemAdded( userSession, userItem );
            }
        }
        else
        {
            GuiUserJoinListItem* userItem = findListEntryWidgetByGroupieId( guiUserJoin->getGroupieId() );
            if( userItem )
            {
                GuiUserJoinSession * userSession = userItem->getUserJoinSession();
                if( userSession )
                {
                    onListItemUpdated( userItem->getUserJoinSession(), userItem );
                }
               
                userItem->update();
            }
        }
    }
}

//============================================================================
void GuiUserJoinListWidget::removeUserJoin( GroupieId& groupieId )
{
    auto iter = m_UserJoinCache.find( groupieId );
    if( iter != m_UserJoinCache.end() )
    {
        m_UserJoinCache.erase( iter );
        GuiUserJoinListItem* userItem = findListEntryWidgetByGroupieId( groupieId );
        if( userItem )
        {
            GuiUserJoinSession * userSession = userItem->getUserJoinSession();
            delete userItem;
            delete userSession;
        }
    }
}

//============================================================================
void GuiUserJoinListWidget::updateThumb( GuiThumb* guiThumb )
{
    int iCnt = count();
    for( int iRow = 0; iRow < iCnt; iRow++ )
    {
        GuiUserJoinListItem* userItem = ( GuiUserJoinListItem* )item( iRow );
        if( userItem )
        {
            GuiUserJoinSession* userSession = userItem->getUserJoinSession();
            if( userSession && userSession->getGuiUser() && userSession->getGuiUser()->getAvatarThumbId() == guiThumb->getThumbId() )
            {
                userItem->updateThumb( guiThumb );
            }
        }
    }
}

//============================================================================
void GuiUserJoinListWidget::onListItemAdded( GuiUserJoinSession* userSession, GuiUserJoinListItem* userItem )
{
    onListItemUpdated( userSession, userItem );
}

//============================================================================
void GuiUserJoinListWidget::onListItemUpdated( GuiUserJoinSession* userSession, GuiUserJoinListItem* userItem )
{
    if( userSession && userItem && userSession->getGuiUser() )
    {
        EHostType hostType = eHostTypeUnknown;
        switch( m_UserJoinViewType )
        {
        case eUserJoinViewTypeGroup:
            hostType = eHostTypeGroup;
            break;
        case eUserJoinViewTypeChatRoom:
            hostType = eHostTypeChatRoom;
            break;
        case eUserJoinViewTypeRandomConnect:
            hostType = eHostTypeRandomConnect;
            break;
        default:
            hostType = eHostTypePeerUserDirect;
        }
        
        if( !userItem->getIsThumbUpdated() )
        {
            VxPushButton* avatarButton = userItem->getIdentAvatarButton();
            GuiUser* guiUser = userSession->getGuiUser();
            if( avatarButton && guiUser )
            {
                QImage	avatarImage;
                bool havAvatarImage = m_ThumbMgr.requestAvatarImage( guiUser, hostType, avatarImage, true );
                if( havAvatarImage && avatarButton )
                {
                    avatarButton->setIconOverrideImage( avatarImage );
                    userItem->setIsThumbUpdated( true );
                }
            }
        }
    }
}
