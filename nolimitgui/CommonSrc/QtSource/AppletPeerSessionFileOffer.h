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

#include "AppletPeerBase.h"

#include <PktLib/VxCommon.h>

#include "GuiUser.h"
#include "ActivityBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletPeerSessionFileOfferUi;
}
QT_END_NAMESPACE

class GuiOfferSession;

class AppletPeerSessionFileOffer : public AppletPeerBase
{
	Q_OBJECT

public:
	AppletPeerSessionFileOffer(	AppCommon& app, 
								QWidget* parent = nullptr );
	virtual ~AppletPeerSessionFileOffer() override = default;

public slots:
	void						onAcceptButClick();
	void						onCancelButClick(); 

protected:
	//! Set plugin icon based on permission level
	void						setPluginIcon( EPluginType pluginType, EFriendState ePluginPermission );

	//=== vars ===//
	Ui::AppletPeerSessionFileOfferUi&	ui;
	EPluginType					m_ePluginType;
};
