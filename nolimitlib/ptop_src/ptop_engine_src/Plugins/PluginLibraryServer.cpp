//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginLibraryServer.h"
#include "PluginMgr.h"

#include <Plugins/FileInfo.h>
#include <P2PEngine/P2PEngine.h>
#include <Plugins/PluginFileShareServer.h>
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
PluginLibraryServer::PluginLibraryServer( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseFilesServer( engine, pluginMgr, myIdent, pluginType, "LibraryService.db3" )
{
	LogMsg( LOG_VERBOSE, "PluginLibraryServer::PluginLibraryServer" );
	setPluginType( ePluginTypeLibraryServer );
}

//============================================================================
void PluginLibraryServer::onNetworkConnectionReady( bool requiresRelay )
{
	if( isPluginEnabled() )
	{
		updateSharedFilesInfo();
	}
}

//============================================================================
void PluginLibraryServer::updateSharedFilesInfo( void )
{
	getFileInfoMgr().updateFileTypes();
}

//============================================================================
void PluginLibraryServer::onFilesChanged( int64_t lastFileUpdateTime, int64_t totalBytes, uint16_t fileTypes )
{

}

//============================================================================
bool PluginLibraryServer::fromGuiSetFileIsInLibrary( FileInfo& fileInfo, bool inLibrary )
{
	return fromGuiSetFileIsShared( fileInfo, inLibrary );
	fileInfo.setIsInLibrary( inLibrary );
	if( inLibrary )
	{
		return m_FileInfoMgr.addFileToDbAndList( fileInfo );
	}
	else
	{
		return m_FileInfoMgr.removeFromDbAndList( fileInfo.getFullFileName() );
	}
}

//============================================================================
bool PluginLibraryServer::fromGuiSetFileIsInLibrary( std::string& fileName, bool inLibrary )
{
	return fromGuiSetFileIsShared( fileName, inLibrary );
}

//============================================================================
void PluginLibraryServer::fromGuiGetFileLibraryList( uint8_t fileTypeFilter )
{
	m_FileInfoMgr.fromGuiGetSharedFiles( fileTypeFilter );
}

//============================================================================
bool PluginLibraryServer::fromGuiGetIsFileInLibrary( std::string& fileName )
{
	return isFileInLibrary( fileName );
}

//============================================================================
bool PluginLibraryServer::fromGuiRemoveFromLibrary( std::string& fileName )
{
	return m_FileInfoMgr.removeFromDbAndList( fileName );
}

//============================================================================
bool PluginLibraryServer::isFileInLibrary( std::string& fileName )
{
	return m_FileInfoMgr.isFileShared( fileName );
}

//============================================================================
void PluginLibraryServer::deleteFile( std::string fileName, bool shredFile )
{
	m_Engine.getPluginFileShareServer().fromGuiSetFileIsShared( fileName, false );
	PluginBaseFilesServer::deleteFile( fileName, shredFile );
}
