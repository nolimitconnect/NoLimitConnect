//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginFileShareServer.h"
#include "PluginMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <GuiInterface/IToGui.h>

#include <PktLib/PktsFileShare.h>
#include <PktLib/PktsPluginOffer.h>
#include <PktLib/VxSearchDefs.h>

#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxFileShredder.h>
#include <CoreLib/VxGlobals.h>

#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif

//============================================================================
PluginFileShareServer::PluginFileShareServer( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseFilesServer( engine, pluginMgr, myIdent, pluginType, "FileShareService.db3" )
{
	setPluginType( ePluginTypeFileShareServer );
}

//============================================================================
void PluginFileShareServer::onNetworkConnectionReady( bool requiresRelay )
{
	if( isPluginEnabled() )
	{
		updateSharedFilesInfo();
	}
}

//============================================================================
void PluginFileShareServer::updateSharedFilesInfo( void )
{
	getFileInfoMgr().updateFileTypes();
}

//============================================================================
void PluginFileShareServer::onFilesChanged( int64_t lastFileUpdateTime, int64_t totalBytes, uint16_t fileTypes )
{
	m_Engine.lockAnnouncePktAccess();
	PktAnnounce& pktAnn = m_Engine.getMyPktAnnounce();
	if( pktAnn.getSharedFileTypes() != fileTypes )
	{
		pktAnn.setSharedFileTypes( fileTypes );
		m_Engine.unlockAnnouncePktAccess();
		if( isPluginEnabled() )
		{
			m_Engine.doPktAnnHasChanged( false );
		}
	}
	else
	{
		m_Engine.unlockAnnouncePktAccess();
	}
}

//============================================================================
void PluginFileShareServer::deleteFile( std::string fileName, bool shredFile )
{
	m_Engine.getPluginLibraryServer().fromGuiSetFileIsInLibrary( fileName, false );
	PluginBaseFilesServer::deleteFile( fileName, shredFile );
}