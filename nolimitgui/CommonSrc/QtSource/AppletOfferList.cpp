//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletOfferList.h"
#include "ActivityInformation.h"

#include "AppCommon.h"
#include "AppGlobals.h"

#include "ActivityMessageBox.h"
#include "GuiHostSession.h"
#include "GuiParams.h"
#include "GuiHelpers.h"

#include <CoreLib/VxGlobals.h>

namespace
{
    const int MAX_INFO_MSG_SIZE = 2048;
}

//============================================================================
AppletOfferList::AppletOfferList(	AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_OFFER_LIST, app, parent )
{
    setAppletType( eAppletOfferList );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_ActiveOffersButton->setFixedSize( eButtonSizeSmall );
    ui.m_ActiveOffersButton->setIcon( eMyIconOfferList );
    ui.m_OfferHistoryButton->setFixedSize( eButtonSizeSmall );
    ui.m_OfferHistoryButton->setIcon( eMyIconOfferHistory );

    connectBarWidgets();

    connect( this,					    SIGNAL(finished(int)),						this, SLOT(slotHomeButtonClicked()) );

    connect( ui.m_ActiveOffersButton, SIGNAL( clicked() ), this, SLOT( slotActiveOffersButtonClicked() ) );
    connect( ui.m_OfferHistoryButton, SIGNAL( clicked() ), this, SLOT( slotOfferHistoryButtonClicked() ) );

    connect( ui.m_OfferListWidget, SIGNAL( signalOfferListItemClicked(GuiOfferSession*,GuiOfferListItem*) ), this, SLOT( slotOfferListItemClicked(GuiOfferSession*,GuiOfferListItem*) ) );
    connect( ui.m_OfferListWidget, SIGNAL( signalAvatarButtonClicked(GuiOfferSession*,GuiOfferListItem*) ), this, SLOT( slotAvatarButtonClicked(GuiOfferSession*,GuiOfferListItem*) ) );
    connect( ui.m_OfferListWidget, SIGNAL( signalOfferViewButtonClicked(GuiOfferSession*,GuiOfferListItem*) ), this, SLOT( slotOfferViewButtonClicked(GuiOfferSession*,GuiOfferListItem*) ) );
    connect( ui.m_OfferListWidget, SIGNAL( signalOfferAcceptButtonClicked(GuiOfferSession*,GuiOfferListItem*) ), this, SLOT( slotOfferAcceptButtonClicked(GuiOfferSession*,GuiOfferListItem*) ) );
    connect( ui.m_OfferListWidget, SIGNAL( signalOfferRejectButtonClicked(GuiOfferSession*,GuiOfferListItem*) ), this, SLOT( slotOfferRejectButtonClicked(GuiOfferSession*,GuiOfferListItem*) ) );
    connect( ui.m_OfferListWidget, SIGNAL( signalPushToTalkButtonClicked(GuiOfferSession*,GuiOfferListItem*) ), this, SLOT( slotPushToTalkButtonClicked(GuiOfferSession*,GuiOfferListItem*) ) );

    m_MyApp.activityStateChange( this, true );
    m_UserMgr.wantGuiUserUpdateCallbacks( this, true );
    updateOfferList( eOfferViewTypeActive );
}

//============================================================================
AppletOfferList::~AppletOfferList()
{
    m_Engine.fromGuiNearbyBroadcastEnable( false );
    m_UserMgr.wantGuiUserUpdateCallbacks( this, false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletOfferList::setStatusLabel( QString strMsg )
{
    ui.m_StatusLabel->setText( strMsg );
}

//============================================================================
void AppletOfferList::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
    m_MyApp.wantToGuiActivityCallbacks( this, true );
}

//============================================================================
void AppletOfferList::hideEvent( QHideEvent* ev )
{
    m_MyApp.wantToGuiActivityCallbacks( this, false );
    ActivityBase::hideEvent( ev );
}

//============================================================================
void AppletOfferList::toGuiInfoMsg( char * infoMsg )
{
    QString infoStr( infoMsg );
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
    infoStr.remove(QRegularExpression("[\\n\\r]"));
#else
    infoStr.remove(QRegExp("[\\n\\r]"));
#endif // QT_VERSION > QT_VERSION_CHECK(6,0,0)
    emit signalInfoMsg( infoStr );
}

//============================================================================
void AppletOfferList::infoMsg( const char* errMsg, ... )
{
    char as8Buf[ MAX_INFO_MSG_SIZE ];
    va_list argList;
    va_start( argList, errMsg );
    vsnprintf( as8Buf, sizeof( as8Buf ), errMsg, argList );
    as8Buf[ sizeof( as8Buf ) - 1 ] = 0;
    va_end( argList );

    toGuiInfoMsg( as8Buf );
}

//============================================================================
void AppletOfferList::clearList( void )
{
    ui.m_OfferListWidget->clear();
    setStatusLabel( GuiParams::describeOfferViewType( m_ListViewType ) + QObject::tr( "List" ) );
}

//============================================================================
void AppletOfferList::clearStatus( void )
{
    setStatusLabel( "" );
}

//============================================================================
void AppletOfferList::callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp )
{
}

//============================================================================
void AppletOfferList::callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId )
{
    removeUser( onlineId );
}

//============================================================================
void AppletOfferList::callbackUserAdded( GuiUser* guiUser )
{
    updateUser( guiUser );
}

//============================================================================
void AppletOfferList::callbackUserUpdated( GuiUser* guiUser )
{
    updateUser( guiUser );
}

//============================================================================
void AppletOfferList::callbackUserRemoved( VxGUID& onlineId )
{
    removeUser( onlineId );
}

//============================================================================
void AppletOfferList::callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
    ui.m_OfferListWidget->callbackPushToTalkStatus( onlineId, pushToTalkStatus );
}

//============================================================================
void AppletOfferList::slotActiveOffersButtonClicked( void )
{
    if( m_ListViewType != eOfferViewTypeActive )
    {
        updateOfferList( eOfferViewTypeActive );
    }
}

//============================================================================
void AppletOfferList::slotOfferHistoryButtonClicked( void )
{
    if( m_ListViewType != eOfferViewTypeHistory )
    {
        updateOfferList( eOfferViewTypeHistory );
    }
}

//============================================================================
void AppletOfferList::updateUser( EUserViewType listType, VxGUID& onlineId )
{
    GuiUser* guiUser = m_MyApp.getUserMgr().getOrQueryUser( onlineId );
    if( guiUser )
    {
        updateUser( guiUser );
    }  
}

//============================================================================
void AppletOfferList::updateUser( GuiUser* guiUser )
{
    if( guiUser )
    {
        ui.m_OfferListWidget->updateUser( guiUser );
    }
}

//============================================================================
void AppletOfferList::removeUser( VxGUID& onlineId )
{
}

//============================================================================
void AppletOfferList::updateOfferList( EOfferViewType offerViewType )
{
    m_ListViewType = offerViewType;
    switch( m_ListViewType )
    {
    case eOfferViewTypeActive:
        ui.m_ActiveOffersButton->setNotifyType( eNotifyOnline );
        ui.m_OfferHistoryButton->setNotifyType( eNotifyNone );
        break;
    case eOfferViewTypeHistory:
        ui.m_ActiveOffersButton->setNotifyType( eNotifyNone );
        ui.m_OfferHistoryButton->setNotifyType( eNotifyOnline );
        break;
    default:
        return;
    }

    ui.m_OfferListWidget->clearOfferList();
    for( auto offerSession : m_MyApp.getOfferMgr().getOfferList() )
    {
        bool isActive = offerSession->isAvailableAndActiveOffer();
        if( isActive && eOfferViewTypeActive == m_ListViewType )
        {
            ui.m_OfferListWidget->addOrUpdateSession( offerSession );
        }
        else if( !isActive && eOfferViewTypeHistory == m_ListViewType )
        {
            ui.m_OfferListWidget->addOrUpdateSession( offerSession );
        }
    }
}

//============================================================================
void AppletOfferList::slotOfferListItemClicked( GuiOfferSession* offerSession, GuiOfferListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "AppletOfferList::slotOfferListItemClicked" );
}

//============================================================================
void AppletOfferList::slotAvatarButtonClicked( GuiOfferSession* offerSession, GuiOfferListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "AppletOfferList::slotAvatarButtonClicked" );
}

//============================================================================
void AppletOfferList::slotOfferViewButtonClicked( GuiOfferSession* offerSession, GuiOfferListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "AppletOfferList::slotOfferViewButtonClicked" );
    m_MyApp.getOfferMgr().viewOffer( offerSession, getParentPageFrame() );
}

//============================================================================
void AppletOfferList::slotOfferAcceptButtonClicked( GuiOfferSession* offerSession, GuiOfferListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "AppletOfferList::slotOfferAcceptButtonClicked" );
    m_MyApp.getOfferMgr().acceptOffer( offerSession, getParentPageFrame() );
}

//============================================================================
void AppletOfferList::slotOfferRejectButtonClicked( GuiOfferSession* offerSession, GuiOfferListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "AppletOfferList::slotOfferRejectButtonClicked" );
    m_MyApp.getOfferMgr().rejectOffer( offerSession, getParentPageFrame() );
}

//============================================================================
void AppletOfferList::slotPushToTalkButtonClicked( GuiOfferSession* offerSession, GuiOfferListItem* userItem )
{
    LogMsg( LOG_VERBOSE, "AppletOfferList::slotPushToTalkButtonClicked" );
}
