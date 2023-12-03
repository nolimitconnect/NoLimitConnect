#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"

#include <CoreLib/VxDefs.h>

#include <QString>
#include <QDialog>
#include "ui_ActivityFileSearch.h"

class P2PEngine;
class VxNetIdent;

class ActivityFileSearch : public ActivityBase
{
	Q_OBJECT
public:
	ActivityFileSearch(	AppCommon& app, QWidget* parent = nullptr );
	virtual ~ActivityFileSearch() override;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

	void						setTitle( QString strTitle );
	void						addFile(	VxNetIdent*	netIdent, 
											uint8_t				u8FileType, 
											uint64_t				u64FileLen, 
											const char*	pFileName );

private slots:
	//! user clicked the upper right x button
    void						slotHomeButtonClicked( void ) override;
	//! user selected menu item
	void						slotItemClicked( QListWidgetItem* item );

protected:
	//=== vars ===//
	Ui::FileSearchDialog		ui;
};

extern ActivityFileSearch * g_poFileSearchActivity;
