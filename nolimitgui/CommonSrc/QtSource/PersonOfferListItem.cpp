//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PersonOfferListItem.h"

#include "AppCommon.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>

#include "ui_PersonOfferListItem.h"

VxPushButton*               PersonOfferListItem::getIdentAvatarButton( void )       { return ui.m_AvatarButton; }
VxPushButton*               PersonOfferListItem::getIdentFriendshipButton( void )   { return ui.m_FriendshipButton; }
VxPushButton*               PersonOfferListItem::getIdentMenuButton( void )         { return ui.m_MenuButton; }
VxPushButton*               PersonOfferListItem::getAcceptButton( void )                     { return ui.m_AcceptButton; }
VxPushButton*               PersonOfferListItem::getRejectButton( void )                     { return ui.m_RejectButton; }

QLabel*                     PersonOfferListItem::getIdentLine1( void )              { return ui.m_TitlePart1; }
QLabel*                     PersonOfferListItem::getIdentLine2( void )              { return ui.m_Line2Label; }

//============================================================================
PersonOfferListItem::PersonOfferListItem(QWidget* parent  )
: IdentLogicInterface( parent )
, ui(*(new Ui::PersonOfferListItemUi))
, m_MyApp( GetAppInstance() )
{
	ui.setupUi( this );
    setupIdentLogic();
    ui.m_AvatarButton->setFixedSize( eButtonSizeLarge );
    ui.m_FriendshipButton->setFixedSize( eButtonSizeLarge );
    ui.m_MenuButton->setFixedSize( eButtonSizeLarge );
}

//============================================================================
QSize PersonOfferListItem::calculateSizeHint( void )
{
    return QSize( (int)( GuiParams::getGuiScale() * 200 ), (int)( GuiParams::getButtonSize().height() + 8 ) );
}

//============================================================================
MyIcons& PersonOfferListItem::getMyIcons( void )
{
    return m_MyApp.getMyIcons();
}

//============================================================================
void PersonOfferListItem::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateWidgetFromInfo();
}

//============================================================================
void PersonOfferListItem::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
    emit signalPersonOfferListItemClicked( this );
}

//============================================================================
void PersonOfferListItem::setOfferSession( GuiOfferSession* hostSession )
{
    QListWidgetItem::setData( Qt::UserRole + 1, QVariant((quint64)hostSession) );
}

//============================================================================
GuiOfferSession* PersonOfferListItem::getOfferSession( void )
{
    return (GuiOfferSession*)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
void PersonOfferListItem::onIdentAvatarButtonClicked()
{
    LogMsg( LOG_DEBUG, "PersonOfferListItem::slotIconButtonClicked" );
	emit signalAvatarButtonClicked( this );
}

//============================================================================
void PersonOfferListItem::onIdentMenuButtonClicked( void )
{
	emit signalMenuButtonClicked( this );
}

//============================================================================
void PersonOfferListItem::updateWidgetFromInfo( void )
{
    GuiOfferSession* hostSession = getOfferSession();
    if( nullptr == hostSession )
    {
        LogMsg( LOG_DEBUG, "PersonOfferListItem::updateWidgetFromInfo null user session" );
        return;
    }

    GuiUser* hostIdent = hostSession->getUserIdent();
    if( nullptr == hostIdent )
    {
        LogMsg( LOG_DEBUG, "PersonOfferListItem::updateWidgetFromInfo null gui user" );
        return;
    }

    updateIdentity( hostIdent );
}
