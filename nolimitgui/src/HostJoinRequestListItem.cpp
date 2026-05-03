//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "HostJoinRequestListItem.h"
#include "GuiHostJoinSession.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

#include "ui_HostJoinRequestListItem.h"

VxPushButton*               HostJoinRequestListItem::getIdentAvatarButton( void )       { return ui.m_AvatarButton; }
VxPushButton*               HostJoinRequestListItem::getIdentFriendshipButton( void )   { return ui.m_FriendshipButton; }
VxPushButton*               HostJoinRequestListItem::getIdentMenuButton( void )         { return ui.m_MenuButton; }
VxPushButton*               HostJoinRequestListItem::getAcceptButton( void )            { return ui.m_AcceptButton; }
VxPushButton*               HostJoinRequestListItem::getRejectButton( void )            { return ui.m_RejectButton; }

QLabel*             HostJoinRequestListItem::getIdentLine1( void ) { return ui.m_TitlePart1; }
QLabel*             HostJoinRequestListItem::getIdentLine2( void ) { return ui.m_DescPart1; }

//============================================================================
HostJoinRequestListItem::HostJoinRequestListItem( QWidget* parent )
: IdentLogicInterface( parent )
, ui(*(new Ui::HostJoinRequestListItemUi))
, m_MyApp( GetAppInstance() )
{
	ui.setupUi( this );
    setupIdentLogic();
    connect( ui.m_AcceptButton,		  SIGNAL(clicked()), this, SLOT(slotAcceptButtonPressed()) );
    connect( ui.m_RejectButton,       SIGNAL(clicked()), this, SLOT(slotRejectButtonPressed()) );
   
    ui.m_AvatarButton->setFixedSize( eButtonSizeLarge );
    ui.m_AvatarButton->setIcon( eMyIconAvatarImage );
    ui.m_FriendshipButton->setFixedSize( eButtonSizeLarge );
    ui.m_FriendshipButton->setIcon( eMyIconAnonymous );
    ui.m_MenuButton->setFixedSize( eButtonSizeLarge );
    ui.m_MenuButton->setIcon( eMyIconMenu );

    ui.m_AcceptButton->setFixedSize( eButtonSizeTiny );
    ui.m_AcceptButton->setIcon( eMyIconAcceptNormal );
    ui.m_RejectButton->setFixedSize( eButtonSizeTiny );
    ui.m_RejectButton->setIcon( eMyIconCancelNormal );
    QSize sizeHint( 200, GuiParams::getButtonSize( eButtonSizeLarge ).height() + 4 );
    setSizeHint( sizeHint );
    setFixedHeight( sizeHint.height() );
}

//============================================================================
HostJoinRequestListItem::~HostJoinRequestListItem()
{
    GuiHostJoinSession * hostSession = (GuiHostJoinSession *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
    if( hostSession && !hostSession->parent() )
    {
        delete hostSession;
    }
}

//============================================================================
MyIcons& HostJoinRequestListItem::getMyIcons( void )
{
    return m_MyApp.getMyIcons();
}

//============================================================================
void HostJoinRequestListItem::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateWidgetFromInfo();
}

//============================================================================
void HostJoinRequestListItem::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
    emit signalHostJoinRequestListItemClicked( this );
}

//============================================================================
void HostJoinRequestListItem::setHostSession( GuiHostJoinSession* hostSession )
{
    setPluginType( HostTypeToHostPlugin( hostSession->getHostType() ) );
    QListWidgetItem::setData( Qt::UserRole + 1, QVariant((quint64)hostSession) );
}

//============================================================================
GuiHostJoinSession * HostJoinRequestListItem::getHostSession( void )
{
    return (GuiHostJoinSession *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
void HostJoinRequestListItem::onIdentAvatarButtonClicked()
{
    LogMsg( LOG_DEBUG, "HostJoinRequestListItem::slotAvatarButtonClicked" );
	emit signalAvatarButtonClicked( this );
}


//============================================================================
void HostJoinRequestListItem::onIdentMenuButtonClicked( void )
{
	emit signalMenuButtonClicked( this );
}

//============================================================================
void HostJoinRequestListItem::updateWidgetFromInfo( void )
{
    GuiHostJoinSession* hostSession = getHostSession();
    if( !hostSession || !hostSession->getGuiUser() )
    {
        LogMsg( LOG_ERROR, "HostJoinRequestListItem::updateWidgetFromInfo null ident" );
        return;
    }

    GuiHostJoin* hostIdent = hostSession->getHostJoin();
    if( hostIdent )
    {
        updateIdentity( hostIdent );
    }

    if( m_MyApp.getEngine().getMyOnlineId() == hostIdent->getUser()->getMyOnlineId() )
    {
        ui.m_TitlePart2->setText( QObject::tr( " Hosted By Me") );
        ui.m_FriendshipButton->setIcon( eMyIconAdministrator );
    }

    if( !ui.m_AvatarButton->hasImage() )
    {
        VxGUID thumbId = hostSession->getHostThumbId();
        if( !thumbId.isValid() )
        {
            thumbId = hostIdent->getUser()->getHostThumbId( hostSession->getHostType(), true );
        }
       
        if( thumbId.isValid() )
        {
            QImage thumbImage;
            if( m_MyApp.getThumbMgr().getThumbImage( thumbId, thumbImage ) )
            {
                ui.m_AvatarButton->setIconOverrideImage( thumbImage );
            }
        }
    }

    // set text of line 2
    std::string strDesc = hostSession->getHostDescription();
    if( strDesc.empty() )
    {
        strDesc = hostIdent->getUser()->getOnlineDescription();
    }

    if( !strDesc.empty() )
    {
        ui.m_DescPart2->setText( strDesc.c_str() );
    }
}

//============================================================================
void HostJoinRequestListItem::setJoinedState( EJoinState joinState )
{
    m_JoinedState = joinState;

    switch( joinState )
    {
    case eJoinStateJoinWasGranted:
    case eJoinStateJoinIsGranted:
        showAcceptButton( false );
        showRejectButton( true );
        break;
    case eJoinStateJoinRequested:
        showAcceptButton( true );
        showRejectButton( true );
        break;
    case eJoinStateJoinDenied:
        showAcceptButton( true );
        showRejectButton( false );
        break;

    case eJoinStateSending:
    case eJoinStateSendFail:
    case eJoinStateSendAcked:
    case eJoinStateNone:
    default:
        showAcceptButton( true );
        showRejectButton( true );
        break;
    }
}

//============================================================================
void HostJoinRequestListItem::showAcceptButton( bool makeVisible )
{
    ui.m_AcceptButton->setVisible( makeVisible );
    ui.m_AcceptLabel->setVisible( makeVisible );
}

//============================================================================
void HostJoinRequestListItem::showRejectButton( bool makeVisible )
{
    ui.m_RejectButton->setVisible( makeVisible );
    ui.m_RejectLabel->setVisible( makeVisible );
}

//============================================================================
void HostJoinRequestListItem::slotAcceptButtonPressed( void )
{
    emit signalAcceptButtonClicked( this );
}

//============================================================================
void HostJoinRequestListItem::slotRejectButtonPressed( void )
{
    emit signalRejectButtonClicked( this );
}
