//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPermissionList.h"

#include "PermissionListItem.h"

#include "AppGlobals.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "AccountMgr.h"
#include "GuiHelpers.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <NetLib/NetHostSetting.h>
#include <NetLib/VxGetRandomPort.h>
#include <CoreLib/VxSktUtil.h>

#include "ui_AppletPermissionList.h"

//============================================================================
AppletPermissionList::AppletPermissionList( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_PERMISSION_LIST, app, parent )
, ui(*(new Ui::AppletPermissionListtUi))
{
	setAppletType( eAppletPermissionList );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    initializePermissionList();

    connectSignals();

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletPermissionList::~AppletPermissionList()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletPermissionList::connectSignals( void )
{
   // connect( ui.m_NetworkHostButton, SIGNAL(clicked()), this, SLOT(slotShowNetworkHostInformation() ) );
}

//============================================================================
void AppletPermissionList::initializePermissionList( void )
{
    for( int i = 1; i < eMaxPermissionPluginType; ++i )
    {
        EPluginType pluginType = ( EPluginType )i;
        if( GuiHelpers::isPluginAPrimaryService( pluginType ) )
        {
            m_PluginList.emplace_back( pluginType );
            createPermissionItem( pluginType );
        }
    }
}

//============================================================================
void AppletPermissionList::createPermissionItem( EPluginType pluginType )
{
    PermissionListItem * item = new PermissionListItem( ui.m_PermissionItemList, this );
    item->initPermissionListItem( pluginType );
    ui.m_PermissionItemList->addItem( ( QListWidgetItem* )item );
    ui.m_PermissionItemList->setItemWidget( ( QListWidgetItem* )item, ( QWidget* )item );
}
