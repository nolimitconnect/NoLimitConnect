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

#include "AppCommon.h"
#include "GuiFriendRequest.h"
#include "GuiParams.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

#include "ui_GuiFriendRequestWidget.h"

//============================================================================
GuiFriendRequestListItem::GuiFriendRequestListItem( QWidget* parent )
: QWidget( parent )
, ui(*(new Ui::GuiFriendRequestWidgetUi))
, m_MyApp( GetAppInstance() )
{
	ui.setupUi( this );
    setFocusPolicy( Qt::NoFocus );

	ui.m_AvatarButton->setVisible( false );
	ui.m_MenuButton->setVisible( false );

    ui.m_AvatarButton->setFixedSize( eButtonSizeLarge );
    ui.m_AvatarButton->setIcon( eMyIconAvatarImage );
    ui.m_FriendshipButton->setFixedSize( eButtonSizeLarge );
    ui.m_FriendshipButton->setIcon( eMyIconAnonymous );
    ui.m_MenuButton->setFixedSize( eButtonSizeLarge );
    ui.m_MenuButton->setIcon( eMyIconMenu );

	ui.m_AcceptButton->setFixedSize( eButtonSizeTiny );
	ui.m_AcceptButton->setIcon( eMyIconAcceptCheckMark );
	ui.m_AcceptButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );

	ui.m_RejectButton->setFixedSize( eButtonSizeTiny );
	ui.m_RejectButton->setIcon( eMyIconRejectRedX );
	ui.m_RejectButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );

	ui.m_DetailsButton->setFixedSize( eButtonSizeTiny );
	ui.m_DetailsButton->setIcon( eMyIconAnonymous );

	connect( ui.m_AcceptButton, SIGNAL(clicked()), this, SLOT(slotAcceptButtonClicked()) );
    connect( ui.m_AcceptLabel, SIGNAL(clicked()), this, SLOT(slotAcceptButtonClicked()) );
	connect( ui.m_DetailsButton, SIGNAL(clicked()), this, SLOT(slotDetailsButtonClicked()) );
    connect( ui.m_DetailsLabel, SIGNAL(clicked()), this, SLOT(slotDetailsButtonClicked()) );
	connect( ui.m_FriendshipButton, SIGNAL(clicked()), this, SLOT(slotFriendshipButtonClicked()) );
	connect( ui.m_RejectButton, SIGNAL(clicked()), this, SLOT(slotRejectButtonClicked()) );
    connect( ui.m_RejectLabel, SIGNAL(clicked()), this, SLOT(slotRejectButtonClicked()) );
}

//============================================================================
GuiFriendRequestListItem::~GuiFriendRequestListItem()
{
    GuiFriendRequest * friendRequest = (GuiFriendRequest *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
    if( friendRequest && !friendRequest->parent() )
    {
        delete friendRequest;
    }
}

//============================================================================
QSize GuiFriendRequestListItem::sizeHint( void ) const
{
    return QSize( 200, GuiParams::getButtonSize( eButtonSizeLarge ).height() + 4 );
}

//============================================================================
void GuiFriendRequestListItem::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateWidgetFromInfo();
}

//============================================================================
void GuiFriendRequestListItem::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
    emit signalFriendRequestListItemClicked( this );
}

//============================================================================
void GuiFriendRequestListItem::setFriendRequest( GuiFriendRequest* friendRequest )
{
    QListWidgetItem::setData( Qt::UserRole + 1, QVariant((quint64)friendRequest) );
}

//============================================================================
GuiFriendRequest * GuiFriendRequestListItem::getFriendRequest( void )
{
    return (GuiFriendRequest *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
void GuiFriendRequestListItem::slotAcceptButtonClicked()
{
    LogMsg( LOG_DEBUG, "GuiFriendRequestListItem::%s", __func__ );
	emit signalAcceptButtonClicked( this );
}

//============================================================================
void GuiFriendRequestListItem::slotDetailsButtonClicked( void )
{
    LogMsg( LOG_DEBUG, "GuiFriendRequestListItem::%s", __func__ );
	emit signalDetailsButtonClicked( this );
}

//============================================================================
void GuiFriendRequestListItem::slotFriendshipButtonClicked( void )
{
    LogMsg( LOG_DEBUG, "GuiFriendRequestListItem::%s", __func__ );
	emit signalFriendshipButtonClicked( this );
}

//============================================================================
void GuiFriendRequestListItem::slotRejectButtonClicked( void )
{
    LogMsg( LOG_DEBUG, "GuiFriendRequestListItem::%s", __func__ );
	emit signalRejectButtonClicked( this );
}

//============================================================================
void GuiFriendRequestListItem::updateWidgetFromInfo( void )
{
    GuiFriendRequest* friendRequest = getFriendRequest();
    if( nullptr == friendRequest )
    {
        return;
    }

    updateFriendRequest( friendRequest );
    update();
}

//============================================================================
void GuiFriendRequestListItem::updateFriendRequest( GuiFriendRequest* friendRequest )
{
    GuiUser* guiUser = GetAppInstance().getUserMgr().getUser( friendRequest->getMyOnlineId() );
    if( guiUser )
    {
        updateUser( guiUser );
    }
}

//============================================================================
void GuiFriendRequestListItem::updateUser( GuiUser* guiUser )
{
	ui.m_TitlePart1->setText( guiUser->getOnlineName().c_str() );
	ui.m_DescPart1->setText( guiUser->getOnlineDescription().c_str() );
	ui.m_FriendshipButton->setIcon( m_MyApp.getMyIcons().getFriendshipIcon( guiUser->getMyFriendshipToHim() ) );
}
