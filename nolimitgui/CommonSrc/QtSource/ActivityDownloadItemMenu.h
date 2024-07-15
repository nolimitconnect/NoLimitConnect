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

#include <PktLib/VxCommon.h>

#include "GuiUser.h"
#include "AppletPeerBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class CancelDownloadDialog;
}
QT_END_NAMESPACE

class GuiFileXferSession;
class P2PEngine;

class ActivityDownloadItemMenu : public ActivityBase
{
	Q_OBJECT

public:
	ActivityDownloadItemMenu( AppCommon& app, GuiFileXferSession* xferSession, QWidget* parent = nullptr );
	virtual ~ActivityDownloadItemMenu() override = default;

public slots:
	void						onCancelButClick(); 

protected:
	//! Set plugin icon based on permission level
	void						setPluginIcon( EPluginType pluginType, EFriendState ePluginPermission );

	//=== vars ===//
	Ui::CancelDownloadDialog&	ui;
	EPluginType					m_ePluginType{ ePluginTypeInvalid };
	GuiFileXferSession*			m_Session{ nullptr };
	VxNetIdent*					m_Ident{ nullptr };
};

