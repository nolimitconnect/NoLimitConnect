//============================================================================
// Copyright (C) 2021 Brett R. Jones 
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

#include "AppCommon.h"
#include "GuiUserListItem.h"
#include "GuiUserSessionBase.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>

//============================================================================
GuiUserListItem::GuiUserListItem( QWidget* parent )
: IdentWidget( parent )
, m_MyApp( GetAppInstance() )
{
    setIdentWidgetSize( eButtonSizeSmall );
}

//============================================================================
GuiUserListItem::~GuiUserListItem()
{
    GuiUserSessionBase * hostSession = getUserSession();
    if( hostSession && !hostSession->parent() )
    {
        hostSession->deleteLater();
    }
}

//============================================================================
QSize GuiUserListItem::calculateSizeHint( void )
{
    return QSize( (int)( GuiParams::getGuiScale() * 200 ), (int)( GuiParams::getButtonSize( eButtonSizeSmall ).height() + 8 ) );
}

//============================================================================
MyIcons& GuiUserListItem::getMyIcons( void )
{
    return m_MyApp.getMyIcons();
}

//============================================================================
void GuiUserListItem::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateWidgetFromInfo();
}

//============================================================================
void GuiUserListItem::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
    emit signalGuiUserListItemClicked( this );
}

//============================================================================
void GuiUserListItem::setUserSession( GuiUserSessionBase* userSession )
{
    QListWidgetItem::setData( Qt::UserRole + 1, QVariant((qulonglong)userSession ) );
    if( !m_UserUpdateConnected && userSession && userSession->getUserIdent() )
    {
        connect( userSession->getUserIdent(), SIGNAL(signalUserUpdated()), this, SLOT(updateWidgetFromInfo()) );
        m_UserUpdateConnected = true;
    }
}

//============================================================================
GuiUserSessionBase * GuiUserListItem::getUserSession( void )
{
    return (GuiUserSessionBase *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
void GuiUserListItem::onIdentAvatarButtonClicked()
{
    LogMsg( LOG_VERBOSE, "GuiUserListItem::onIdentAvatarButtonClicked" );
    emit signalAvatarButtonClicked( this );
}

// Allow default behavior
//============================================================================
//void GuiUserListItem::onIdentFriendshipButtonClicked()
//{
//    LogMsg( LOG_VERBOSE, "GuiUserListItem::onIdentFrienshipButtonClicked" );
//    emit signalFriendshipButtonClicked( this );
//}

//============================================================================
void GuiUserListItem::onIdentOfferViewButtonClicked()
{
    LogMsg( LOG_VERBOSE, "GuiUserListItem::onIdentOfferViewButtonClicked" );
    emit signalOfferViewButtonClicked( this );
}

//============================================================================
void GuiUserListItem::onIdentOfferAcceptButtonClicked()
{
    LogMsg( LOG_VERBOSE, "GuiUserListItem::onIdentOfferAcceptButtonClicked" );
    emit signalOfferAcceptButtonClicked( this );
}

//============================================================================
void GuiUserListItem::onIdentOfferRejectButtonClicked()
{
    LogMsg( LOG_VERBOSE, "GuiUserListItem::onIdentOfferRejectButtonClicked" );
    emit signalOfferRejectButtonClicked( this );
}

//============================================================================
void GuiUserListItem::onIdentPushToTalkButtonClicked()
{
    LogMsg( LOG_VERBOSE, "GuiUserListItem::onIdentPushToTalkButtonClicked" );
    emit signalPushToTalkButtonClicked( this );
}

//============================================================================
void GuiUserListItem::onIdentMenuButtonClicked( void )
{
	emit signalMenuButtonClicked( this );
}

//============================================================================
void GuiUserListItem::updateWidgetFromInfo( void )
{
    GuiUserSessionBase* hostSession = getUserSession();
    if( nullptr == hostSession )
    {
        LogMsg( LOG_ERROR, "GuiUserListItem::updateWidgetFromInfo null user session" );
        return;
    }

    GuiUser* hostIdent = hostSession->getUserIdent();
    if( nullptr == hostIdent )
    {
        LogMsg( LOG_ERROR, "GuiUserListItem::updateWidgetFromInfo null gui user" );
        return;
    }

    updateIdentity( hostIdent );
}

//============================================================================
void GuiUserListItem::updateThumb( GuiThumb* guiThumb )
{
    if( guiThumb )
    {
        QImage thumbImage;
        if( guiThumb->createImage( thumbImage ) && !thumbImage.isNull() )
        {
            ui.m_AvatarButton->setIconOverrideImage( thumbImage );
            update();
        }
    }
}

//============================================================================
void GuiUserListItem::callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
    if( getIdentPushToTalkButton() )
    {
        getIdentPushToTalkButton()->setPushToTalkStatus( pushToTalkStatus );
    }
}

//============================================================================
bool GuiUserListItem::operator<( const QListWidgetItem& other ) const
{
    const GuiUserListItem* rhs = dynamic_cast<const GuiUserListItem*>(&other);
    if( rhs )
    {
        return const_cast<GuiUserListItem*>(this)->compareLessThanItem( (GuiUserListItem*)rhs );
    }

    return false;
}

//============================================================================
bool GuiUserListItem::compareLessThanItem( GuiUserListItem* rhs )
{
    if( rhs )
    {
        GuiUser* guiUser = getGuiUser();
        GuiUser* guiOther = rhs->getGuiUser();
        if( guiUser && guiOther )
        {
            return guiUser->compareLessThan( guiOther );
        }
    }

    return false;
}