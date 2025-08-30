#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//



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

    virtual void				toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int percentProgress );

    virtual void				toGuiFileDeleted( QString& fileName );

    virtual void				toGuiFileDownloadComplete( EPluginType pluginType, VxGUID&	lclSessionId, QString& fileName, EXferError xferError );
    virtual void				toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, QString& fileName, EXferError xferError );

    virtual void				toGuiFileList( VxGUID appInstId, FileInfo& fileInfo );
    virtual void				toGuiFileListCompleted( VxGUID appInstId );

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
    void                        updateSession( std::vector<GuiFileXferSession*>& sessionList, VxGUID& lclSessionId, EXferState xferState, EXferError xferErr, int percentProgress );
    void                        removeSession( std::vector<GuiFileXferSession*>& sessionList, GuiFileXferSession* xferSession );
    GuiFileXferSession*         findSession( std::vector<GuiFileXferSession*>& sessionList, VxGUID& lclSessionId );

    void                        clearFileXferClientList( void );

    AppCommon&                  m_MyApp;
    std::vector<ToGuiFileXferInterface*> m_ToGuiFileXferInterfaceList;
    std::vector<GuiFileXferSession*>  m_DownloadSessions;
    std::vector<GuiFileXferSession*>  m_UploadSessions;
    bool                        m_ToGuiFileXferInterfaceBusy{ false };
};
