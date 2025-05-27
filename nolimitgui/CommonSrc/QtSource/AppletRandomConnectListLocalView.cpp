//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletRandomConnectListLocalView.h"

#include "AppCommon.h"
#include "AppGlobals.h"
#include "AppletPopupMenu.h"
#include "ActivityMessageBox.h"
#include "GuiHostSession.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QTimer>

#include "ui_AppletRandomConnectListLocalView.h"

namespace
{
    const int MAX_INFO_MSG_SIZE = 2048;
}

//============================================================================
AppletRandomConnectListLocalView::AppletRandomConnectListLocalView(	AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_RANDOM_CONNECT_LIST_LOCAL_VIEW, app, parent )
, ui(*(new Ui::AppletRandomConnectListLocalViewUi))
, m_CloseAppletTimer( new QTimer( this ) )
{
    setAppletType( eAppletRandomConnectListLocalView );
    setHostType( eHostTypeRandomConnect );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
    setSearchType( eSearchRandomConnectHost );

    connectBarWidgets();

    connect( ui.m_RefreshButton,	    SIGNAL(clicked()),		                    this, SLOT(slotRefreshGroupList()) );
    connect( this,					    SIGNAL(signalSearchComplete()),				this, SLOT(slotSearchComplete()) );
    connect( this,					    SIGNAL(signalSearchResult(VxNetIdent*)),	this, SLOT(slotSearchResult(VxNetIdent*)) ); 

    connect( this, SIGNAL( signalLogMsg( const QString& ) ), this, SLOT( slotInfoMsg( const QString& ) ) );
    connect( this, SIGNAL( signalInfoMsg( const QString& ) ), this, SLOT( slotInfoMsg( const QString& ) ) );

    connect( &m_MyApp, SIGNAL(signalHostJoinStatus( EHostType, VxGUID, EHostJoinStatus, QString )),
        this, SLOT(slotHostJoinStatus( EHostType, VxGUID, EHostJoinStatus, QString )) );
    connect( &m_MyApp, SIGNAL(signalHostSearchStatus( EHostType, VxGUID, EHostSearchStatus, QString )),
        this, SLOT(slotHostSearchStatus( EHostType, VxGUID, EHostSearchStatus, QString )) );


    connect( ui.m_HostedListWidget,      SIGNAL(signalIconButtonClicked(GuiHostSession*,GuiHostedListItem*)),  this, SLOT(slotIconButtonClicked(GuiHostSession*,GuiHostedListItem*)) );
    connect( ui.m_HostedListWidget,      SIGNAL(signalMenuButtonClicked(GuiHostSession*,GuiHostedListItem*)),  this, SLOT(slotMenuButtonClicked(GuiHostSession*,GuiHostedListItem*)) );
    connect( ui.m_HostedListWidget,      SIGNAL(signalJoinButtonClicked(GuiHostSession*,GuiHostedListItem*)),  this, SLOT(slotJoinButtonClicked(GuiHostSession*,GuiHostedListItem*)) );

    ui.m_ListDescriptionLabel->setText( QObject::tr( "Random Connect Hosts Announced To Network Host" ) );
    setStatusLabel( QObject::tr( "Random Connect Hosts Announced To Network Host" ) );
    m_MyApp.getHostedListMgr().wantHostedListCallbacks( this, true );
    slotRefreshGroupList();

    if( eFriendStateIgnore == m_Engine.getPluginPermission( ePluginTypeHostNetwork ) )
    {
        okMessageBox( QObject::tr( "Service Unavailable" ), QObject::tr( "Network Host Service Is Not Enabled. You can not view random connect hosts announced to this device because the service is not enabled" ) );
        connect( m_CloseAppletTimer, SIGNAL(timeout()), this, SLOT(onCancelButClick()) );
        m_CloseAppletTimer->setSingleShot( true );
        m_CloseAppletTimer->start( 1000 );
    }
    else if( eFriendStateIgnore == m_Engine.isDirectConnectReady() )
    {
        okMessageBox( QObject::tr( "Service Disabled" ), QObject::tr( "Network Host Service is not enabled because it requires an open port." ) );
        connect( m_CloseAppletTimer, SIGNAL(timeout()), this, SLOT(onCancelButClick()) );
        m_CloseAppletTimer->setSingleShot( true );
        m_CloseAppletTimer->start( 1000 );
    }
}

//============================================================================
AppletRandomConnectListLocalView::~AppletRandomConnectListLocalView()
{
    m_MyApp.getHostedListMgr().wantHostedListCallbacks( this, false );
}

//============================================================================
void AppletRandomConnectListLocalView::setStatusLabel( QString strMsg )
{
    ui.m_StatusLabel->setText( strMsg );
}

//============================================================================
void AppletRandomConnectListLocalView::setInfoLabel( QString strMsg )
{
    ui.m_InfoLabel->setText( strMsg );
    ui.m_InfoLabel->update();
}

//============================================================================
void AppletRandomConnectListLocalView::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
    wantActivityCallbacks( true );
}

//============================================================================
void AppletRandomConnectListLocalView::hideEvent( QHideEvent* ev )
{
    wantActivityCallbacks( false );
    ActivityBase::hideEvent( ev );
}

//============================================================================
void AppletRandomConnectListLocalView::slotRefreshGroupList( void )
{
    clearPluginSettingToList();
    clearStatus();
    m_SearchSessionId.initializeWithNewVxGUID();
    m_MyApp.getFromGuiInterface().fromGuiSendAnnouncedList( getHostType(), m_SearchSessionId );
}

//============================================================================
void AppletRandomConnectListLocalView::slotSearchComplete( void )
{
    //ui.m_SearchsParamWidget->slotSearchComplete();
}

//============================================================================
void AppletRandomConnectListLocalView::slotInfoMsg( const QString& text )
{
    setStatusLabel( text ); // Adds the message to the widget                                                                                              //m_LogFile.write( text ); // Logs to file
}

//============================================================================
void AppletRandomConnectListLocalView::slotHostAnnounceStatus( EHostType hostType, VxGUID sessionId, EHostAnnounceStatus hostStatus, QString text )
{
    setInfoLabel( GuiParams::describeStatus(hostStatus) + text);
}

//============================================================================
void AppletRandomConnectListLocalView::slotHostJoinStatus( EHostType hostType, VxGUID sessionId, EHostJoinStatus hostStatus, QString text )
{
    setInfoLabel( GuiParams::describeStatus(hostStatus) + text);
}

//============================================================================
void AppletRandomConnectListLocalView::slotHostSearchStatus( EHostType hostType, VxGUID sessionId, EHostSearchStatus hostStatus, QString strMsg )
{
    if( hostStatus == eHostSearchCompleted )
    {
        m_SearchStarted = false;
        setStatusLabel( strMsg );
        //ui.m_SearchsParamWidget->slotSearchComplete();
    }
    else
    {
        setInfoLabel( strMsg );
    }
}

//============================================================================
void AppletRandomConnectListLocalView::toGuiInfoMsg( char * infoMsg )
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
void AppletRandomConnectListLocalView::infoMsg( const char* errMsg, ... )
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
void AppletRandomConnectListLocalView::clearPluginSettingToList( void )
{
    ui.m_HostedListWidget->clearHostList();
}

//============================================================================
void AppletRandomConnectListLocalView::clearStatus( void )
{
    setInfoLabel( "" );
    setStatusLabel( "" );
}

//============================================================================
void AppletRandomConnectListLocalView::slotIconButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem )
{

}

//============================================================================
void AppletRandomConnectListLocalView::slotMenuButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem )
{

}

//============================================================================
void AppletRandomConnectListLocalView::slotJoinButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem )
{
    onJointButtonClicked( hostSession );
}


//============================================================================
void AppletRandomConnectListLocalView::callbackGuiHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId )
{
    LogMsg( LOG_DEBUG, "AppletRandomConnectListLocalView::callbackGuiHostedListSearchResult host %s title %s", DescribeHostType( hostedId.getHostType() ), guiHosted->getHostTitle().c_str() );

    QString strMsg = QObject::tr( "Match found: " );
    strMsg += guiHosted->getHostTitle().c_str();
    setInfoLabel( strMsg );
    updateHostedList( hostedId, guiHosted, sessionId );
}

//============================================================================
void AppletRandomConnectListLocalView::updateHostedList( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId )
{
    ui.m_HostedListWidget->updateHostedList( hostedId, guiHosted, sessionId );
}

//============================================================================   
void AppletRandomConnectListLocalView::onCancelButClick( void )
{
    onBackButtonClicked();
}
