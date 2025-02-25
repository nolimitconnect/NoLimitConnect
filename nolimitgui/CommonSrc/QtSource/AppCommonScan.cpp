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
#include "ActivityScanProfiles.h"
#include "ActivityScanWebCams.h"
#include "ActivityScanPeopleSearch.h"
#include "ToGuiActivityInterface.h"

#include <Plugins/FileInfo.h>

#include <CoreLib/VxGlobals.h>

//============================================================================
void AppCommon::toGuiScanSearchComplete( EScanType eScanType )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiScanSearchComplete( eScanType );
}

//============================================================================
void AppCommon::slotInternalToGuiScanSearchComplete( EScanType eScanType )
{
	m_ToGuiActivityInterfaceBusy = true;
	for( auto& client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiClientScanSearchComplete( eScanType );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiScanResultSuccess( EScanType eScanType, VxNetIdent* netIdent )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiScanResultSuccess( eScanType, *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiScanResultSuccess( EScanType eScanType, VxNetIdent netIdent )
{
    GuiUser* guiUser = m_UserMgr.getUser( netIdent.getMyOnlineId() );

	m_ToGuiActivityInterfaceBusy = true;
	for( auto& client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiScanResultSuccess( eScanType, guiUser );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiSearchResultError( EScanType eScanType, VxNetIdent* netIdentIn, int errCode )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiSearchResultError( eScanType, *netIdentIn, errCode );
}

//============================================================================
void AppCommon::slotInternalToGuiSearchResultError( EScanType eScanType, VxNetIdent netIdentIn, int errCode )
{
    GuiUser* guiUser = m_UserMgr.getUser( netIdentIn.getMyOnlineId() );

	m_ToGuiActivityInterfaceBusy = true;
	for( auto& client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiSearchResultError( eScanType, guiUser, errCode );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiSearchResultProfilePic(	VxNetIdent*	netIdentIn, 
												uint8_t *		pu8JpgData, 
												uint32_t		u32JpgDataLen )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    GuiUser* guiUser = m_UserMgr.getUser( netIdentIn->getMyOnlineId() );

	m_ToGuiActivityInterfaceBusy = true;
	for( auto& client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiSearchResultProfilePic( guiUser, pu8JpgData, u32JpgDataLen );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiSearchResultFileSearch( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
	emit signalInternalToGuiSearchResultFileSearch( onlineId, pluginType, lclSessionId, fileInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiSearchResultFileSearch( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	GuiUser* guiUser = m_UserMgr.getUser( onlineId );

	m_ToGuiActivityInterfaceBusy = true;
	for( auto& client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiSearchResultFileSearch( guiUser, pluginType, lclSessionId, fileInfo );
	}

	m_ToGuiActivityInterfaceBusy = false;
}