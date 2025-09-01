//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"

#include "ActivityMessageBox.h"
#include "AppletFileOfferSelect.h"
#include "AppletMultiMessenger.h"
#include "AppletOfferSend.h"
#include "AppletMgr.h"
#include "GuiHelpers.h"
#include "GuiOfferMgr.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"
#include "GuiUserMgr.h"

#include <CoreLib/VxDebug.h>

//============================================================================
bool AppCommon::offerToFriendPluginSession( GuiUser* guiUser, EPluginType pluginType, QWidget* parent )
{
	bool showedApplet = false;
	if( guiUser->isMyAccessAllowedFromHim( pluginType ) )
	{
		showedApplet = launchOfferSendApplet( pluginType, guiUser, parent );
	}
	else
	{
		EPluginAccess ePluginAccess = guiUser->getMyAccessPermissionFromHim( pluginType );
        ActivityMessageBox errMsgBox( GetAppInstance(), parent, LOG_INFO, "%s's %s", guiUser->getOnlineName().c_str(), 
			GuiParams::describePluginAction( guiUser, pluginType, ePluginAccess ).toStdString().c_str() );
		errMsgBox.exec();
	}

	return showedApplet;
}

//============================================================================
// offer to send a file
void AppCommon::offerToFriendSendFile( GuiUser* guiUser, QWidget* parent )
{
	if( guiUser->isMyAccessAllowedFromHim( ePluginTypePersonFileXfer ) )
	{
		AppletFileOfferSelect* applet = dynamic_cast<AppletFileOfferSelect*>(getAppletMgr().launchApplet( eAppletFileOfferSelect, parent ));
		if( applet )
		{
			applet->setUser( guiUser );
			applet->setPluginType( ePluginTypePersonFileXfer );
		}
	}
	else
	{
		EPluginAccess ePluginAccess = guiUser->getMyAccessPermissionFromHim( ePluginTypePersonFileXfer );
		ActivityMessageBox errMsgBox( GetAppInstance(), parent, LOG_INFO, "%s's %s", guiUser->getOnlineName().c_str(), 
			GuiParams::describePluginAction( guiUser, ePluginTypePersonFileXfer, ePluginAccess ).toStdString().c_str() );
		errMsgBox.exec();
	}
}

//============================================================================
bool AppCommon::launchOfferSendApplet( EPluginType pluginType, GuiUser* guiUser, QWidget* parent )
{
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "AppCommon::launchOfferApplet null guiUser" );
		return false;
	}

	std::shared_ptr<GuiOfferSession> existingOffer = getOfferMgr().findActiveAndAvailableOffer( guiUser, pluginType );
	return launchOfferSendSession( pluginType, guiUser, existingOffer, parent );
}

//============================================================================
bool AppCommon::launchOfferSendSession( EPluginType pluginType, GuiUser* guiUser, std::shared_ptr<GuiOfferSession> existingOffer, QWidget* parent )
{
	bool result{ false };
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "AppCommon::launchOfferApplet null guiUser" );
		return false;
	}

	AppletOfferSend* applet = dynamic_cast<AppletOfferSend*>(getAppletMgr().launchApplet( eAppletOfferSend, parent ));
	if( applet )
	{
		result = applet->setOffer( pluginType, guiUser, existingOffer );
	}

	return result;
}

//============================================================================
bool AppCommon::iAmHostAdmin( EPluginType pluginType, bool showErrMsg )
{
	GuiUser* myIdent = getUserMgr().getMyIdent();
	if( !myIdent )
	{
		LogMsg( LOG_ERROR, "AppletBase::%s null myIdent", __func__ );
		return false;
	}

	bool isHosting = myIdent->getNetIdent().userIsHosting( PluginTypeToHostType( pluginType ) );

	if( !isHosting )
	{
		if( showErrMsg )
		{
			GuiHelpers::showHostIsDisabledError( PluginTypeToHostType( pluginType ) );
		}

		return false;
	}

	return isHosting;
}
