#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <CoreLib/VxDefs.h>
#include <CoreLib/AssetDefs.h>
#include <PktLib/VxCommon.h>

#include <GuiInterface/IDefs.h>

#include <QString>

class VxGUID;
class VxNetIdent;
class GuiFileXferSession;
class FileListReplySession;
class FileInfo;

class ToGuiFileXferInterface 
{
public:
	virtual void				callbackToGuiFileList( FileInfo& fileInfo ){};
	virtual void				callbackToGuiFileListCompleted( void ) {};

	virtual void				callbackBeginDownload( EApplet appletType, GuiFileXferSession* xferSession ) {};
	virtual void				callbackAcceptDownload( EApplet appletType, GuiFileXferSession* xferSession ) {};
	virtual void				callbackCancelDownload( EApplet appletType, GuiFileXferSession* xferSession ) {};

	virtual void				callbackBeginUpload( EApplet appletType, GuiFileXferSession* xferSession ) {};
	virtual void				callbackAcceptUpload( EApplet appletType, GuiFileXferSession* xferSession ) {};
	virtual void				callbackCancelUpload( EApplet appletType, GuiFileXferSession* xferSession ) {};

	virtual void				toGuiFileListReply(	FileListReplySession* xferSession ){};

	virtual void				toGuiFileUploadStart( GuiFileXferSession* xferSession ){};
	virtual void				toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, EXferError xferError ){};

	virtual void				toGuiFileDownloadStart( GuiFileXferSession* xferSession ){};
	virtual void				toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, QString newFileName, EXferError xferError ){};

	virtual void				toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 ){};

	virtual void				toGuiFileDeleted( QString& fileName ){};
};
