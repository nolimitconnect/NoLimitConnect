//============================================================================
// Copyright (C) 2009 Brett R. Jones 
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "AppGlobals.h"
#include "AppletMgr.h"
#include "GuiOfferMgr.h"

#include "ActivityMessageBox.h"
#include "AppletPeerChangeFriendship.h"
#include "AppletFileShareClientView.h"
#include "AppletFileOfferSelect.h"
#include "AppletMultiMessenger.h"
#include "AppletOfferSend.h"
#include "AppletPeerTodGame.h"
#include "AppletPeerVideoPhone.h"
#include "AppletPeerVoicePhone.h"

#include "MyIcons.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <QDesktopServices>
#include <QUrl>

//============================================================================
bool AppCommon::offerToFriendPluginSession( GuiUser* guiUser, EPluginType pluginType, bool inGroup, QWidget* parent )
{
	bool showedApplet = false;
	if( guiUser->isMyAccessAllowedFromHim( pluginType, inGroup ) )
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
void AppCommon::offerToFriendSendFile( GuiUser* guiUser, bool inGroup, QWidget* parent )
{
	if( guiUser->isMyAccessAllowedFromHim( ePluginTypePersonFileXfer, inGroup ) )
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
		EPluginAccess ePluginAccess = guiUser->getMyAccessPermissionFromHim( ePluginTypePersonFileXfer, inGroup );
		ActivityMessageBox errMsgBox( GetAppInstance(), parent, LOG_INFO, "%s's %s", guiUser->getOnlineName().c_str(), 
			GuiParams::describePluginAction( guiUser, ePluginTypePersonFileXfer, ePluginAccess ).toStdString().c_str() );
		errMsgBox.exec();
	}
}

//============================================================================
void AppCommon::removePluginSessionOffer( EPluginType pluginType, GuiUser* guiUser )
{
    getOfferMgr().removePluginSessionOffer( pluginType, guiUser );
}

//============================================================================
bool AppCommon::launchOfferSendApplet( EPluginType pluginType, GuiUser* guiUser, QWidget* parent )
{
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "AppCommon::launchOfferApplet null guiUser" );
		return false;
	}

	GuiOfferSession* existingOffer = getOfferMgr().findActiveAndAvailableOffer( guiUser, pluginType );
	return launchOfferSendSession( pluginType, guiUser, existingOffer, parent );
}

//============================================================================
bool AppCommon::launchOfferSendSession( EPluginType pluginType, GuiUser* guiUser, GuiOfferSession* existingOffer, QWidget* parent )
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
bool AppCommon::launchOfferResponseAccept( GuiOfferSession* offerSession, QWidget* contentFrame )
{
	bool result{ false };
	GuiUser* guiUser = offerSession->getUser();
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "AppCommon::launchOfferApplet null guiUser" );
		return false;
	}

	switch( offerSession->getPluginType() )
	{

	case ePluginTypeMessenger:
	{
		AppletMultiMessenger* appletMessenger = getAppletMultiMessenger();
		if( appletMessenger )
		{
			appletMessenger->setSelectedUser( guiUser );
			result = true;
		}
	}

	break;

	case ePluginTypeVoicePhone:
	{
		if( !getAppletMgr().findAppletDialog( eAppletPeerVoicePhone ) || guiUser->isMyself() ) // allow multiple sessions if is myself for testing
		{
			AppletPeerVoicePhone* applet = dynamic_cast<AppletPeerVoicePhone*>( getAppletMgr().launchApplet( eAppletPeerVoicePhone, contentFrame ) );
			if( applet )
			{
				result = applet->offerSession( guiUser, offerSession );
			}
		}
		else
		{
			GuiHelpers::errorMsgBox( eErrMsgAlreadyInSession, contentFrame );
		}
	}

	break;

	case ePluginTypeVideoPhone:
	{
		if( !getAppletMgr().findAppletDialog( eAppletPeerVideoPhone ) || guiUser->isMyself() ) // allow multiple sessions if is myself for testing
		{
			AppletPeerVideoPhone* applet = dynamic_cast<AppletPeerVideoPhone*>(getAppletMgr().launchApplet( eAppletPeerVideoPhone, contentFrame ));
			if( applet )
			{
				result = applet->offerSession( guiUser, offerSession );
			}
		}
		else
		{
			GuiHelpers::errorMsgBox( eErrMsgAlreadyInSession, contentFrame );
		}
	}

	break;

	case ePluginTypeTruthOrDare:
	{
		if( !getAppletMgr().findAppletDialog( eAppletPeerTruthOrDare ) || guiUser->isMyself() ) // allow multiple sessions if is myself for testing
		{
			AppletPeerTodGame* applet = dynamic_cast<AppletPeerTodGame*>(getAppletMgr().launchApplet( eAppletPeerTruthOrDare, contentFrame ));
			if( applet )
			{
				result = applet->offerSession( guiUser, offerSession );
			}
		}
		else
		{
			GuiHelpers::errorMsgBox( eErrMsgAlreadyInSession, contentFrame );
		}
	}

	break;

	default:
		ActivityMessageBox errMsgBox( *this, this, LOG_ERROR, "AppCommon::launchOfferApplet UNKNOWN plugin type %d", offerSession->getPluginType() );
		errMsgBox.exec();
	}

	return result;
}
