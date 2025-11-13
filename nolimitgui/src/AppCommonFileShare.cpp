//============================================================================
// Copyright (C) 2013 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"

#include "AppletFileShareClientView.h"
#include "FileListReplySession.h"
#include "GuiFileXferSession.h"
#include "ToGuiFileXferInterface.h"

#include <CoreLib/VxGlobals.h>

//============================================================================
void AppCommon::toGuiFileListReply( VxGUID& onlineId, EPluginType pluginType, FileInfo& fileInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    emit signalInternalToGuiFileListReply( onlineId, pluginType, fileInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiFileListReply( VxGUID onlineId, EPluginType pluginType, FileInfo fileInfo )
{
    getFileXferMgr().toGuiFileListReply(onlineId, pluginType, fileInfo );
}

//============================================================================
void AppCommon::toGuiFileUploadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    emit signalInternalToGuiFileUploadStart( onlineId, pluginType, lclSessionId, fileInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiFileUploadStart( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo )
{
    getFileXferMgr().toGuiFileUploadStart(onlineId, pluginType, lclSessionId, fileInfo );
}

//============================================================================
void AppCommon::toGuiFileDownloadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    emit signalInternalToGuiFileDownloadStart( onlineId, pluginType, lclSessionId, fileInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiFileDownloadStart( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo )
{
    getFileXferMgr().toGuiFileDownloadStart(onlineId, pluginType, lclSessionId, fileInfo );
}

//============================================================================
void AppCommon::toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    emit signalInternalToGuiFileXferState( pluginType, lclSessionId, xferDir, xferState, xferErr, param1 );
}

//============================================================================
void AppCommon::slotInternalToGuiFileXferState( EPluginType pluginType, VxGUID lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 )
{
    getFileXferMgr().toGuiFileXferState( pluginType, lclSessionId, xferDir, xferState, xferErr, param1 );
}

//============================================================================
void AppCommon::toGuiFileDeleted( std::string& fileName )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    emit signalInternalToGuiFileDeleted( fileName.c_str() );
}

//============================================================================
void AppCommon::slotInternalToGuiFileDeleted( QString fileName )
{
    getFileXferMgr().toGuiFileDeleted( fileName );
}

//============================================================================
void AppCommon::toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, std::string& fileName, EXferError xferError )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiFileDownloadComplete( pluginType, lclSessionId, fileName.c_str(), xferError );
}

//============================================================================
void AppCommon::slotInternalToGuiFileDownloadComplete( EPluginType pluginType, VxGUID lclSessionId, QString fileName, EXferError xferError )
{
    getFileXferMgr().toGuiFileDownloadComplete(pluginType, lclSessionId, fileName, xferError );
}

//============================================================================
void AppCommon::toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, std::string& fileName, EXferError xferError )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiFileUploadComplete( pluginType, lclSessionId, fileName.c_str(), xferError );
}

//============================================================================
void AppCommon::slotInternalToGuiFileUploadComplete( EPluginType pluginType, VxGUID lclSessionId, QString fileName, EXferError xferError )
{
    getFileXferMgr().toGuiFileUploadComplete(pluginType, lclSessionId, fileName, xferError );
}

//============================================================================
void AppCommon::toGuiFileList( VxGUID& appInstId, FileInfo& fileInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiFileList( appInstId, fileInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiFileList( VxGUID appInstId, FileInfo fileInfo )
{
    getFileXferMgr().toGuiFileList( appInstId, fileInfo );
}

//============================================================================
void AppCommon::toGuiFileListCompleted( VxGUID& appInstId )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiFileListCompleted(appInstId);
}

//============================================================================
void AppCommon::slotInternalToGuiFileListCompleted( VxGUID appInstId )
{
    getFileXferMgr().toGuiFileListCompleted(appInstId);
}

//============================================================================
void AppCommon::toGuiFolderScan( VxGUID& appInstId, FileInfo& fileInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiFolderScan( appInstId, fileInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiFolderScan( VxGUID appInstId, FileInfo fileInfo )
{
	getFileXferMgr().toGuiFolderScan( appInstId, fileInfo );
}

//============================================================================
void AppCommon::toGuiFolderScanCompleted( VxGUID& appInstId, bool wasCanceled )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiFolderScanCompleted( appInstId, wasCanceled );
}

//============================================================================
void AppCommon::slotInternalToGuiFolderScanCompleted( VxGUID appInstId, bool wasCanceled )
{
	getFileXferMgr().toGuiFolderScanCompleted( appInstId, wasCanceled );
}
