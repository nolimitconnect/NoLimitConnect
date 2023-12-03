//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "AppCommon.h"
#include "GuiUserJoinListItem.h"
#include "GuiUserJoinSession.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>

//============================================================================
GuiUserJoinListItem::GuiUserJoinListItem(QWidget* parent  )
: IdentWidget( parent )
, m_MyApp( GetAppInstance() )
, m_ConnectIdListMgr( m_MyApp.getConnectIdListMgr() )
, m_OfferMgr( m_MyApp.getOfferMgr() )
, m_UserMgr( m_MyApp.getUserMgr() )
, m_UserJoinMgr( m_MyApp.getUserJoinMgr() )
{
}

//============================================================================
GuiUserJoinListItem::~GuiUserJoinListItem()
{
    GuiUserJoinSession * hostSession = (GuiUserJoinSession *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
    if( hostSession && !hostSession->parent() )
    {
        delete hostSession;
    }
}

//============================================================================
QSize GuiUserJoinListItem::calculateSizeHint( void )
{
    return QSize( (int)( GuiParams::getGuiScale() * 200 ), (int)( GuiParams::getButtonSize().height() + 8 ) );
}

//============================================================================
MyIcons& GuiUserJoinListItem::getMyIcons( void )
{
    return m_MyApp.getMyIcons();
}

//============================================================================
void GuiUserJoinListItem::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateWidgetFromInfo();
}

//============================================================================
void GuiUserJoinListItem::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
    // emit signalUserJoinListItemClicked( this );
}

//============================================================================
void GuiUserJoinListItem::setUserJoinSession( GuiUserJoinSession* hostSession )
{
    QListWidgetItem::setData( Qt::UserRole + 1, QVariant((quint64)hostSession) );
}

//============================================================================
GuiUserJoinSession * GuiUserJoinListItem::getUserJoinSession( void )
{
    return (GuiUserJoinSession *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
void GuiUserJoinListItem::onIdentAvatarButtonClicked()
{
    LogMsg( LOG_DEBUG, "GuiUserJoinListItem::slotIconButtonClicked" );
	emit signalAvatarButtonClicked( this );
}

//============================================================================
void GuiUserJoinListItem::onIdentMenuButtonClicked( void )
{
	emit signalMenuButtonClicked( this );
}

//============================================================================
void GuiUserJoinListItem::updateWidgetFromInfo( void )
{
    GuiUserJoinSession* hostSession = getUserJoinSession();
    if( nullptr == hostSession )
    {
        LogMsg( LOG_DEBUG, "GuiUserJoinListItem::updateWidgetFromInfo null user session" );
        return;
    }

    GuiUser* guiUser = hostSession->getGuiUser();
    if( nullptr == guiUser )
    {
        LogMsg( LOG_DEBUG, "GuiUserJoinListItem::updateWidgetFromInfo null gui user" );
        return;
    }

    updateIdentity( guiUser );
}

//============================================================================
void GuiUserJoinListItem::updateThumb( GuiThumb* guiThumb )
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
void GuiUserJoinListItem::updateUser( GuiUser* guiUser )
{
    updateWidgetFromInfo();
}
