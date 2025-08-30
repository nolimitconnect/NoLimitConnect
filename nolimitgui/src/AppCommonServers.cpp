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
#include "MyIcons.h"
#include "AppletPopupMenu.h"
#include "AppGlobals.h"

#include "ActivityPermissions.h"

#include <CoreLib/VxGlobals.h>

//============================================================================
void AppCommon::onMenuServerSelected( int iMenuId, PopupMenu *, ActivityBase * )
{
	switch( iMenuId )
	{

	case 1: //Start/Stop Web Cam Server
		switch( getEngine().fromGuiGetPluginServerState(ePluginTypeCamServer))
		{
		case ePluginServerStateStarted:	// app has started
			getEngine().setHasSharedWebCam(false);
            getEngine().fromGuiStopPluginSession( ePluginTypeCamServer, m_UserMgr.getMyOnlineId(), 0 );
			break;
		case ePluginServerStateStopped:
			startActivity( ePluginTypeCamServer, m_UserMgr.getMyIdent() );
			getEngine().setHasSharedWebCam(true);
			break;
		case ePluginServerStateDisabled:	// disabled by permissions
			{
				ActivityPermissions * poDlg = new ActivityPermissions( *this, getCentralWidget() );
				poDlg->setPluginType(ePluginTypeCamServer);
				poDlg->exec();
			}
			break;
        default:
            break;
		}

		break;

	default: 
		LogMsg( LOG_ERROR, "onMenuServerSelected: ERROR unknown menu id %d", iMenuId );
		break;
	}
}
