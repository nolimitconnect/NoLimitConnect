//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSocketList.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "MyIcons.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <NetLib/VxPeerMgr.h>

#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QScrollBar>
#include <QApplication>

#include <QStandardItemModel>

namespace
{
    const int MAX_INFO_MSG_SIZE = 2048;
}


//============================================================================
AppletSocketList::AppletSocketList( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_SOCKET_LIST, app, parent )
{
    setAppletType( eAppletSocketList );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_CopyToClipboardButton->setVisible( false );

    connect( ui.m_CopyToClipboardButton, SIGNAL( clicked() ), this, SLOT( slotCopyToClipboardClicked() ) );

	m_MyApp.activityStateChange( this, true );

    QStandardItemModel* model = new QStandardItemModel( 0, 3, parent );
    ui.m_TreeView->setRootIsDecorated( false );
    ui.m_TreeView->setAlternatingRowColors( true );
    ui.m_TreeView->setModel( model );
    ui.m_TreeView->setSortingEnabled( true );

    model->setHeaderData( 0, Qt::Horizontal, QObject::tr( "Socket" ) );
    model->setHeaderData( 1, Qt::Horizontal, QObject::tr( "Socket Type" ) );
    model->setHeaderData( 2, Qt::Horizontal, QObject::tr( "IP Address" ) );

    refreshSktList();
}

//============================================================================
AppletSocketList::~AppletSocketList()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletSocketList::slotCopyToClipboardClicked( void )
{
    //QClipboard * clipboard = QApplication::clipboard();
    //clipboard->setText( getSocketListEdit()->toPlainText() );
}

//============================================================================
void AppletSocketList::logMsg( const char* logMsg, ... )
{
    char as8Buf[ MAX_INFO_MSG_SIZE ];
    va_list argList;
    va_start( argList, logMsg );
    vsnprintf( as8Buf, sizeof( as8Buf ), logMsg, argList );
    as8Buf[ sizeof( as8Buf ) - 1 ] = 0;
    va_end( argList );   
}

//============================================================================
void AppletSocketList::refreshSktList( void )
{
    VxPeerMgr& peerMgr = m_MyApp.getEngine().getPeerMgr();
    std::vector<VxSktStatRecord> sktStatList;
    peerMgr.getSktStatRecords( sktStatList );
    for( auto& rec : sktStatList )
    {
        addSocketStat( rec );
    }
}

//============================================================================
void AppletSocketList::addSocketStat( VxSktStatRecord& sktStat )
{
    QStandardItemModel* model = dynamic_cast<QStandardItemModel *>( ui.m_TreeView->model() );
    if( model )
    {
        int rowNum = model->rowCount();
        model->insertRow( rowNum );
        model->setData( model->index( rowNum, 0 ), QString::number( sktStat.getSktHandle() ) );
        model->setData( model->index( rowNum, 1 ), QString( DescribeSktType( sktStat.getSktType() ) ) );
        model->setData( model->index( rowNum, 2 ), QString( sktStat.getIpAddr().c_str() ) );
    }
}
