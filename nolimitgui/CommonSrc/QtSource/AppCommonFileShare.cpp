//============================================================================
// Copyright (C) 2013 Brett R. Jones 
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
    getFileXferMgr().toGuiFileXferState(pluginType, lclSessionId, xferDir, xferState, xferErr, param1 );
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
void AppCommon::toGuiFileList( FileInfo& fileInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiFileList( fileInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiFileList( FileInfo fileInfo )
{
    getFileXferMgr().toGuiFileList( fileInfo );
}

//============================================================================
void AppCommon::toGuiFileListCompleted( void )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiFileListCompleted();
}

//============================================================================
void AppCommon::slotInternalToGuiFileListCompleted( void )
{
    getFileXferMgr().toGuiFileListCompleted();
}
