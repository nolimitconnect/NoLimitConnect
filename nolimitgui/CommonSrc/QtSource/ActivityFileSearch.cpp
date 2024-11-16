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

#include <CoreLib/ObjectCommonDefs.h>

#include "ui_ActivityFileSearch.h"

ActivityFileSearch * g_poFileSearchActivity = NULL;

TitleBarWidget *  ActivityFileSearch::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget * ActivityFileSearch::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }

//============================================================================
ActivityFileSearch::ActivityFileSearch(	AppCommon& app,  QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_FILE_SEARCH, app, parent, eAppletScanSharedFiles, true, false )
, ui(*(new Ui::FileSearchDialog))
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


