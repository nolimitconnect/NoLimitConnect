//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHackerList.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <NetLib/VxPeerMgr.h>

#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QScrollBar>
#include <QApplication>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QDesktopWidget>
#endif // QT_VERSION < QT_VERSION_CHECK(6,0,0)

#include "ui_AppletHackerList.h"

//============================================================================
AppletHackerList::AppletHackerList( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_LOG, app, parent )
, ui(*(new Ui::AppletHackerListUi))
{
    setAppletType( eAppletHackerList );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_CopyToClipboardButton->setVisible( false );

    QStandardItemModel* model = new QStandardItemModel( 0, 5, parent );
    ui.m_TreeView->setRootIsDecorated( false );
    ui.m_TreeView->setAlternatingRowColors( true );
    ui.m_TreeView->setModel( model );
    ui.m_TreeView->setSortingEnabled( true );

    model->setHeaderData( 0, Qt::Horizontal, QObject::tr( "Count" ) );
    model->setHeaderData( 1, Qt::Horizontal, QObject::tr( "IP Address" ) );
    model->setHeaderData( 2, Qt::Horizontal, QObject::tr( "Level" ) );
    model->setHeaderData( 3, Qt::Horizontal, QObject::tr( "Offense" ) );
    model->setHeaderData( 4, Qt::Horizontal, QObject::tr( "Signature" ) );

    connect( ui.m_CopyToClipboardButton, SIGNAL(clicked()), this, SLOT( slotCopyToClipboardClicked() ) );

	m_MyApp.activityStateChange( this, true );

    refreshHackerList();
}

//============================================================================
AppletHackerList::~AppletHackerList()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHackerList::slotCopyToClipboardClicked( void )
{
    //QClipboard * clipboard = QApplication::clipboard();
    //clipboard->setText( getHackerListEdit()->toPlainText() );
}

//============================================================================
void AppletHackerList::refreshHackerList( void )
{
    VxPeerMgr& peerMgr = m_MyApp.getEngine().getPeerMgr();
    std::vector<VxHackerRecord> hackerList;

    peerMgr.getHackerList( hackerList );
    for( auto& rec : hackerList )
    {
        addHacker( rec );
    }
}

//============================================================================
void AppletHackerList::addHacker( VxHackerRecord& hackerRec )
{
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(ui.m_TreeView->model());
    if( model )
    {
        int rowNum = model->rowCount();
        model->insertRow( rowNum );
        model->setData( model->index( rowNum, 0 ), QString::number( hackerRec.getOffenseCount() ) );
        model->setData( model->index( rowNum, 1 ), QString( hackerRec.getIpAddr().c_str() ) );
        model->setData( model->index( rowNum, 2 ), QString::number( hackerRec.getHackerLevel() ) );
        model->setData( model->index( rowNum, 3 ), QString( DescribeHackerReason( hackerRec.getHackerReason() ) ) );
        model->setData( model->index( rowNum, 4 ), QString( hackerRec.getSignature().toHexString().c_str() ) );
    }
}
