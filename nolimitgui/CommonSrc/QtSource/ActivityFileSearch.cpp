//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityFileSearch.h"
#include "AppCommon.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

ActivityFileSearch * g_poFileSearchActivity = NULL;

//============================================================================
ActivityFileSearch::ActivityFileSearch(	AppCommon& app,  QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_FILE_SEARCH, app, parent, eAppletScanSharedFiles, true, false )
{
	ui.setupUi(this);
	ui.m_TitleBarWidget->setTitleBarText( QObject::tr("File Search" ) );

    connectBarWidgets();

    connect(ui.m_TitleBarWidget, SIGNAL(signalBackButtonClicked()), this, SLOT(slotHomeButtonClicked()));
    connect(ui.FileItemList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
    connect(ui.FileItemList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));

    g_poFileSearchActivity = this;
	setTitle("FileSearch" );
}

//============================================================================
ActivityFileSearch::~ActivityFileSearch()
{
	g_poFileSearchActivity = NULL;
}

//============================================================================
void ActivityFileSearch::setTitle( QString strTitle )
{
	ui.m_TitleBarWidget->setTitleBarText(strTitle);
}

//============================================================================
//=== signals and slots ===//
//============================================================================

//============================================================================
//! clicked the upper right x button
void ActivityFileSearch::slotHomeButtonClicked( void )
{
	close();
}

//============================================================================
//! user selected menu item
void ActivityFileSearch::slotItemClicked(QListWidgetItem* item)
{
}


