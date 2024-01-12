//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityDownloadItemMenu.h"

#include "AppCommon.h"
#include "MyIcons.h"

#include "GuiFileXferSession.h"

#include <CoreLib/ObjectCommonDefs.h>

//============================================================================
ActivityDownloadItemMenu::ActivityDownloadItemMenu(	AppCommon& app, 
													GuiFileXferSession* poSession, 
													QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_DOWNLOAD_ITEM_MENU, app, parent, eAppletMessengerFrame, false, true )
, m_ePluginType( ePluginTypeFileShareServer )
, m_Session( poSession )
{
	ui.setupUi(this);
	ui.m_TitleBarWidget->setTitleBarText( QObject::tr("Download Item Menu") );

    connectBarWidgets();

	setupStyledDlg(	poSession->getIdent(), 
					ui.FriendIdentWidget,
					m_ePluginType,
					ui.PermissionButton, 
					ui.PermissionLabel );

	ui.PermissionButton->setIcon( getMyIcons().getFileIcon( poSession->getFileType() ) );
	ui.PermissionLabel->setText( poSession->describeFileType() );

	ui.FileNameEdit->setText( m_Session->getJustFileName() );

	connect( ui.CancelButton, SIGNAL(clicked()), this, SLOT(onCancelButClick()) );
}

//============================================================================
void ActivityDownloadItemMenu::onCancelButClick()
{
	accept();
}
