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

#include <GuiInterface/IDefs.h>
#include <QString>
#include <QDialog>
#include "ui_ActivityScanPeopleSearch.h"

class VxNetIdent;

class ActivityScanPeopleSearch : public ActivityBase
{
	Q_OBJECT
public:
	ActivityScanPeopleSearch(	AppCommon&		app, 
								EScanType			eSearchType,
								QWidget*			parent = nullptr );
	//=== destructor ===//
	virtual ~ActivityScanPeopleSearch() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

	EScanType					getScanType() { return m_eScanType; }
	void						searchResult( GuiUser* guiUser );
	void						setTitle( QString strTitle );
	void						setStatusLabel( QString strMsg );

    virtual void				toGuiScanResultSuccess( EScanType eScanType, GuiUser* guiUser ) override;
    virtual void				toGuiClientScanSearchComplete( EScanType eScanType ) override;

signals:
	void						signalSearchResult( GuiUser* guiUser );
	void						signalSearchComplete( void );

private slots:
	void						slotSearchResult( GuiUser* guiUser );
	void						slotSearchComplete( void );
    void						slotHomeButtonClicked( void ) override;
	void						slotStartSearchClicked();
	void						slotStopSearchClicked();
	//void						slotFriendClicked( VxNetIdent* netIdent );

protected:
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

	//=== vars ===//
	Ui::PeopleSearchDlg			ui;
	EScanType					m_eScanType;
};
