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

#include <QString>
#include <QDialog>
#include <QTimer>
#include "ui_ActivityScanStoryBoards.h"
#include <GuiInterface/IDefs.h>

class VxNetIdent;

class ActivityScanStoryBoards :  public ActivityBase
{
	Q_OBJECT
public:
	ActivityScanStoryBoards(	AppCommon& app, 
								QWidget*		parent = nullptr );
	virtual ~ActivityScanStoryBoards() override;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

public:
	void						setTitle( QString strTitle );
	void						newStoryBoardSession( VxNetIdent* netIdent, const char* pStory );

signals:
	void						signalScanStoryBoard( VxNetIdent* netIdent, QString strStory );

public slots:
	void						slotScanStoryBoard( VxNetIdent* netIdent, QString strStory );

private slots:
    void						slotHomeButtonClicked( void ) override;
	void						onNextButtonClicked( void );

protected:
	void						setupIdentWidget( VxNetIdent* netIdent );

	//=== vars ===//
	Ui::ScanStoryBoardsDialog	ui;
	bool						m_bAutoScan;
	QTimer *					m_SessionTimer;
	EScanType					m_eScanType;
};

extern ActivityScanStoryBoards * g_poScanStoryBoardsActivity;
