#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
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

#include "AppDefs.h"
#include <GuiInterface/IDefs.h>

#include <QObject>
#include <vector>

class AppCommon;
class FileInfo;
class GuiFileXferSession;
class ToGuiFileXferInterface;
class VxGUID;
class VxNetIdent;

class GuiFileXferMgr : public QObject
{
    Q_OBJECT
public:
    GuiFileXferMgr() = delete;
    GuiFileXferMgr( AppCommon& app );
    GuiFileXferMgr( const GuiFileXferMgr& rhs ) = delete;
	virtual ~GuiFileXferMgr() = default;

    void                        onAppCommonCreated( void );
    void                        onMessengerReady( bool ready ) { }
    bool                        isMessengerReady( void );
    virtual void                onSystemReady( bool ready ) { }

    void                        wantToGuiFileXferCallbacks( ToGuiFileXferInterface* callback, bool	wantCallback );

    std::vector<GuiFileXferSession*>& getDownloadSessions( void )   { return  m_DownloadSessions; }
    std::vector<GuiFileXferSession*>& getUploadSessions( void )     { return  m_UploadSessions; }

    virtual void				toGuiFileListReply( VxGUID& onlineId, EPluginType pluginType, FileInfo& fileInfo );

    virtual void				toGuiFileUploadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo );
    virtual void				toGuiFileDownloadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo );

    virtual void				toGuiFileXferState( EPluginType pluginType, VxGUID& lclSession, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int percentProgress );

    virtual void				toGuiFileDownloadComplete( EPluginType pluginType, VxGUID&	lclSessionId, QString& fileName, EXferError xferError );
    virtual void				toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, QString& fileName, EXferError xferError );

    virtual void				toGuiFileList( FileInfo& fileInfo );
    virtual void				toGuiFileListCompleted( void );

    virtual void				beginDownload( EApplet appletType, GuiFileXferSession* xferSession );
    virtual void				acceptDownload( EApplet appletType, GuiFileXferSession* xferSession );
    virtual void				cancelDownload( EApplet appletType, GuiFileXferSession* xferSession );

    virtual void				beginUpload( EApplet appletType, GuiFileXferSession* xferSession );
    virtual void				acceptUpload( EApplet appletType, GuiFileXferSession* xferSession );
    virtual void				cancelUpload( EApplet appletType, GuiFileXferSession* xferSession );

    
protected:
    void                        addDownload( GuiFileXferSession* xferSession );
    void                        removeDownload( GuiFileXferSession* xferSession );
    void                        addUpload( GuiFileXferSession* xferSession );
    void                        removeUpload( GuiFileXferSession* xferSession );

    void                        addSession( std::vector<GuiFileXferSession*>& sessionList, GuiFileXferSession* xferSession );
    void                        updateSession( std::vector<GuiFileXferSession*>& sessionList, VxGUID& lclSession, EXferState xferState, EXferError xferErr, int percentProgress );
    void                        removeSession( std::vector<GuiFileXferSession*>& sessionList, GuiFileXferSession* xferSession );
    GuiFileXferSession*         findSession( std::vector<GuiFileXferSession*>& sessionList, VxGUID& lclSessionId );

    void                        clearFileXferClientList( void );

    AppCommon&                  m_MyApp;
    std::vector<ToGuiFileXferInterface*> m_ToGuiFileXferInterfaceList;
    std::vector<GuiFileXferSession*>  m_DownloadSessions;
    std::vector<GuiFileXferSession*>  m_UploadSessions;
    bool                        m_ToGuiFileXferInterfaceBusy{ false };
};
