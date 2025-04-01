//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChooseHostAdminOrSearch.h"

#include "AppGlobals.h"
#include "AppCommon.h"
#include "MyIcons.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletChooseHostAdminOrSearch.h"

//============================================================================
AppletChooseHostAdminOrSearch::AppletChooseHostAdminOrSearch( AppCommon& app, QWidget* parent )
	: AppletBase( OBJNAME_APPLET_CHOOSE_HOST_ADMIN_OR_SEARCH, app, parent )
	, ui(*(new Ui::AppletChooseHostAdminOrSearchUi))
{
    setAppletType( eAppletChooseHostAdminOrSearch );
	ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	ui.m_OkButton->setIcon( eMyIconCheckMark );
	ui.m_OkButton->setFixedSize( eButtonSizeLarge );

	ui.m_CancelButton->setIcon( eMyIconRedX );
	ui.m_CancelButton->setFixedSize( eButtonSizeLarge );

	ui.m_ToAdminButton->setChecked( true );

	connect( ui.m_OkButton,			SIGNAL(clicked()), this, SLOT(onOkButClick()) );
	connect( ui.m_CancelButton,		SIGNAL(clicked()), this, SLOT(onCancelButClick()) );

	connect( ui.m_ToAdminButton,	SIGNAL(clicked()), this, SLOT(onChooseButClick()) );
	connect( ui.m_ToSearchButton,	SIGNAL(clicked()), this, SLOT(onChooseButClick()) );
}

//============================================================================
//! get current permission selection
void AppletChooseHostAdminOrSearch::updateChoiceSelection( void )
{
	EFriendState ePluginPermission = eFriendStateFriend;
	if( ui.m_ToAdminButton->isChecked() )
	{
		m_ToAdmin = true;
	}
	else if( ui.m_ToSearchButton->isChecked() )
	{
		m_ToAdmin = false;
	} 
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletChooseHostAdminOrSearch::onOkButClick( void )
{
	accept();
}

//============================================================================   
void AppletChooseHostAdminOrSearch::onCancelButClick( void )
{
	reject();
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletChooseHostAdminOrSearch::onChooseButClick( void )
{
	updateChoiceSelection();
}
