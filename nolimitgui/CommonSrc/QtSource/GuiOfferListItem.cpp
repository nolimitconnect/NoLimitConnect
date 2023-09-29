//============================================================================
// Copyright (C) 2022 Brett R. Jones 
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
#include "GuiOfferListItem.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>

//============================================================================
GuiOfferListItem::GuiOfferListItem(QWidget* parent  )
: IdentWidget( parent )
, m_MyApp( GetAppInstance() )
{
    setIdentWidgetSize( eButtonSizeMedium );
}

//============================================================================
GuiOfferListItem::~GuiOfferListItem()
{
    GuiOfferSession * hostSession = getOfferSession();
    if( hostSession && !hostSession->parent() )
    {
        hostSession->deleteLater();
    }
}

//============================================================================
QSize GuiOfferListItem::calculateSizeHint( void )
{
    return QSize( (int)( GuiParams::getGuiScale() * 200 ), (int)( GuiParams::getButtonSize( eButtonSizeMedium ).height() + 8 ) );
}

//============================================================================
MyIcons& GuiOfferListItem::getMyIcons( void )
{
    return m_MyApp.getMyIcons();
}

//============================================================================
void GuiOfferListItem::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateWidgetFromInfo();
}

//============================================================================
void GuiOfferListItem::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
    emit signalGuiOfferListItemClicked( this );
}

//============================================================================
void GuiOfferListItem::setOfferSession( GuiOfferSession* offerSession )
{
    QListWidgetItem::setData( Qt::UserRole + 1, QVariant((qulonglong)offerSession) );
    if( !m_UserUpdateConnected && offerSession && offerSession->getUserIdent() )
    {
        connect( offerSession->getUserIdent(), SIGNAL(signalUserUpdated()), this, SLOT(updateWidgetFromInfo()) );
        connect( offerSession, SIGNAL(signalOfferUpdated()), this, SLOT(updateWidgetFromInfo()) );
        m_UserUpdateConnected = true;
    }
}

//============================================================================
GuiOfferSession * GuiOfferListItem::getOfferSession( void )
{
    return (GuiOfferSession *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
void GuiOfferListItem::onIdentAvatarButtonClicked()
{
    LogMsg( LOG_DEBUG, "GuiOfferListItem::onIdentAvatarButtonClicked" );
    emit signalAvatarButtonClicked( this );
}

// Allow default behavior
//============================================================================
//void GuiOfferListItem::onIdentFriendshipButtonClicked()
//{
//    LogMsg( LOG_DEBUG, "GuiOfferListItem::onIdentFrienshipButtonClicked" );
//    emit signalFriendshipButtonClicked( this );
//}

//============================================================================
void GuiOfferListItem::onIdentOfferViewButtonClicked()
{
    LogMsg( LOG_DEBUG, "GuiOfferListItem::onIdentOfferViewButtonClicked" );
    emit signalOfferViewButtonClicked( this );
}

//============================================================================
void GuiOfferListItem::onIdentOfferAcceptButtonClicked()
{
    LogMsg( LOG_DEBUG, "GuiOfferListItem::onIdentOfferAcceptButtonClicked" );
    emit signalOfferAcceptButtonClicked( this );
}

//============================================================================
void GuiOfferListItem::onIdentOfferRejectButtonClicked()
{
    LogMsg( LOG_DEBUG, "GuiOfferListItem::onIdentOfferRejectButtonClicked" );
    emit signalOfferRejectButtonClicked( this );
}

//============================================================================
void GuiOfferListItem::onIdentPushToTalkButtonClicked()
{
    LogMsg( LOG_DEBUG, "GuiOfferListItem::onIdentPushToTalkButtonClicked" );
    emit signalPushToTalkButtonClicked( this );
}

//============================================================================
void GuiOfferListItem::onIdentMenuButtonClicked( void )
{
	emit signalMenuButtonClicked( this );
}

//============================================================================
void GuiOfferListItem::updateWidgetFromInfo( void )
{
    GuiOfferSession* offerSession = getOfferSession();
    if( nullptr == offerSession )
    {
        LogMsg( LOG_DEBUG, "GuiOfferListItem::updateWidgetFromInfo null user session" );
        return;
    }

    GuiUser* guiUser = offerSession->getUserIdent();
    if( nullptr == guiUser )
    {
        LogMsg( LOG_DEBUG, "GuiOfferListItem::updateWidgetFromInfo null gui user" );
        return;
    }

    updateIdentity( guiUser );
    updateOffer( offerSession );
}

//============================================================================
void GuiOfferListItem::updateThumb( GuiThumb* guiThumb )
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
void GuiOfferListItem::callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
    if( getIdentPushToTalkButton() )
    {
        getIdentPushToTalkButton()->setPushToTalkStatus( pushToTalkStatus );
    }
}