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

#include "GuiFileXferMgr.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "FileListReplySession.h"
#include "GuiFileXferSession.h"
#include "GuiParams.h"
#include "ToGuiFileXferInterface.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
GuiFileXferMgr::GuiFileXferMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
{
}

//============================================================================
void GuiFileXferMgr::onAppCommonCreated( void )
{
}

//============================================================================
bool GuiFileXferMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//============================================================================
void GuiFileXferMgr::clearFileXferClientList( void )
{
    if( m_ToGuiFileXferInterfaceList.size() )
    {
        m_ToGuiFileXferInterfaceList.clear();
    }
}

//============================================================================
void GuiFileXferMgr::wantToGuiFileXferCallbacks( ToGuiFileXferInterface* callback, bool	wantCallback )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( m_ToGuiFileXferInterfaceBusy )
    {
        LogMsg( LOG_ERROR, "AppCommon::wantToGuiFileXferCallbacks do NOT call while busy" );
    }

    if( wantCallback )
    {
        for( ToGuiFileXferInterface* client : m_ToGuiFileXferInterfaceList )
        {
            if( client == callback )
            {
                LogMsg( LOG_VERBOSE, "WARNING. Ignoring New m_ToGuiActivityInterface.h because already in list" );
                return;
            }
        }

        m_ToGuiFileXferInterfaceList.push_back( callback );
        return;
    }

    for( auto iter = m_ToGuiFileXferInterfaceList.begin(); iter != m_ToGuiFileXferInterfaceList.end(); ++iter )
    {
        ToGuiFileXferInterface* client = *iter;
        if( client == callback )
        {
            m_ToGuiFileXferInterfaceList.erase( iter );
            return;
        }
    }

    LogMsg( LOG_INFO, "WARNING. ToGuiFileXferInterface remove not found in list" );
    return;
}

//============================================================================
void GuiFileXferMgr::toGuiFileListReply( VxGUID& onlineId, EPluginType pluginType, FileInfo& fileInfo )
{
    FileListReplySession* replySession = new FileListReplySession( pluginType, m_MyApp.getUserMgr().getUser( onlineId ), fileInfo );

    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->toGuiFileListReply( replySession );
    }

    m_ToGuiFileXferInterfaceBusy = false;
    replySession->deleteLater();
}

//============================================================================
void GuiFileXferMgr::toGuiFileUploadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    GuiFileXferSession* fileXferSession = new GuiFileXferSession( eXferDirectionTx, pluginType, m_MyApp.getUserMgr().getUser( onlineId ), lclSessionId, fileInfo );

    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->toGuiFileUploadStart( fileXferSession );
    }

    m_ToGuiFileXferInterfaceBusy = false;
    fileXferSession->deleteLater();
}

//============================================================================
void GuiFileXferMgr::toGuiFileDownloadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    GuiFileXferSession* fileXferSession = new GuiFileXferSession( eXferDirectionRx, pluginType, m_MyApp.getUserMgr().getUser( onlineId ), lclSessionId, fileInfo );
    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->toGuiFileDownloadStart( fileXferSession );
    }

    m_ToGuiFileXferInterfaceBusy = false;
    fileXferSession->deleteLater();
}

//============================================================================
void GuiFileXferMgr::toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int percentProgress )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( eXferDirectionRx == xferDir )
    {
        updateSession( m_DownloadSessions, lclSessionId, xferState, xferErr, percentProgress );
    }
    else
    {
        updateSession( m_UploadSessions, lclSessionId, xferState, xferErr, percentProgress );
    }

    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->toGuiFileXferState( pluginType, lclSessionId, xferDir, xferState, xferErr, percentProgress );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}


//============================================================================
void GuiFileXferMgr::toGuiFileDeleted( QString& fileName )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->toGuiFileDeleted( fileName );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, QString& fileName, EXferError xferError )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->toGuiFileDownloadComplete( pluginType, lclSessionId, fileName, xferError );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, QString& fileName, EXferError xferError )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->toGuiFileUploadComplete( pluginType, lclSessionId, xferError );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::toGuiFileList( VxGUID appInstId, FileInfo& fileInfo )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->callbackToGuiFileList( appInstId, fileInfo );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::toGuiFileListCompleted( VxGUID appInstId )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->callbackToGuiFileListCompleted(appInstId);
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::beginDownload( EApplet appletType, GuiFileXferSession* xferSession )
{
    addDownload( xferSession );
    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->callbackBeginDownload( appletType, xferSession );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::acceptDownload( EApplet appletType, GuiFileXferSession* xferSession )
{
    removeDownload( xferSession );
    if( eXferStateCompletedDownload == xferSession->getXferState() )
    {
        m_MyApp.getEngine().fromGuiSetFileIsInLibrary( xferSession->getFileInfo(), true );
        m_ToGuiFileXferInterfaceBusy = true;
        for( auto& client : m_ToGuiFileXferInterfaceList )
        {
            client->callbackAcceptDownload( appletType, xferSession );
        }

        m_ToGuiFileXferInterfaceBusy = false;
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiFileXferMgr::acceptDownload wrong state %s", GuiParams::describeEXferState( xferSession->getXferState() ).toUtf8().constData() );
    }
}

//============================================================================
void GuiFileXferMgr::cancelDownload( EApplet appletType, GuiFileXferSession* xferSession )
{
    removeDownload( xferSession );
    m_MyApp.getEngine().fromGuiSetFileIsInLibrary( xferSession->getFileInfo(), false );
    m_MyApp.getEngine().fromGuiCancelDownload( xferSession->getLclSessionId() );
    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->callbackCancelDownload( appletType, xferSession );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::beginUpload( EApplet appletType, GuiFileXferSession* xferSession )
{
    addUpload( xferSession );
    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->callbackBeginUpload( appletType, xferSession );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::acceptUpload( EApplet appletType, GuiFileXferSession* xferSession )
{
    removeUpload( xferSession );
    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->callbackAcceptUpload( appletType, xferSession );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::cancelUpload( EApplet appletType, GuiFileXferSession* xferSession )
{
    removeUpload( xferSession );
    m_MyApp.getEngine().fromGuiCancelUpload( xferSession->getLclSessionId() );
    m_ToGuiFileXferInterfaceBusy = true;
    for( auto& client : m_ToGuiFileXferInterfaceList )
    {
        client->callbackCancelUpload( appletType, xferSession );
    }

    m_ToGuiFileXferInterfaceBusy = false;
}

//============================================================================
void GuiFileXferMgr::addDownload( GuiFileXferSession* xferSession )
{
    addSession( m_DownloadSessions, xferSession );
}

//============================================================================
void GuiFileXferMgr::removeDownload( GuiFileXferSession* xferSession )
{
    removeSession( m_DownloadSessions, xferSession );
}

//============================================================================
void GuiFileXferMgr::addUpload( GuiFileXferSession* xferSession )
{
    addSession( m_UploadSessions, xferSession );
}

//============================================================================
void GuiFileXferMgr::removeUpload( GuiFileXferSession* xferSession )
{
    removeSession( m_UploadSessions, xferSession );
}

//============================================================================
void GuiFileXferMgr::addSession( std::vector<GuiFileXferSession*>& sessionList, GuiFileXferSession* xferSessionIn )
{
    GuiFileXferSession* xferSession = findSession( sessionList, xferSessionIn->getLclSessionId() );
    if( xferSession )
    {
        xferSession->updateSession( xferSessionIn );
    }
    else
    {
        GuiFileXferSession* newSession = new GuiFileXferSession( *xferSessionIn );
        sessionList.push_back( newSession );
    }
}

//============================================================================
void GuiFileXferMgr::updateSession( std::vector<GuiFileXferSession*>& sessionList, VxGUID& lclSessionId, EXferState xferState, EXferError xferErr, int percentProgress )
{
    GuiFileXferSession* xferSession = findSession( sessionList, lclSessionId );
    if( xferSession )
    {
        xferSession->setXferState( xferState, xferErr, percentProgress );
    }
}

//============================================================================
void GuiFileXferMgr::removeSession( std::vector<GuiFileXferSession*>& sessionList, GuiFileXferSession* xferSession )
{
    for( auto iter = sessionList.begin(); iter != sessionList.end(); ++iter )
    {
        if( (*iter)->getLclSessionId() == xferSession->getLclSessionId() )
        {
            GuiFileXferSession* xferSession = (*iter);
            sessionList.erase( iter );
            xferSession->deleteLater();
            break;
        }
    }
}

//============================================================================
GuiFileXferSession* GuiFileXferMgr::findSession( std::vector<GuiFileXferSession*>& sessionList, VxGUID& lclSessionId )
{
    for( auto iter = sessionList.begin(); iter != sessionList.end(); ++iter )
    {
        if( (*iter)->getLclSessionId() == lclSessionId )
        {
            return (*iter);
        }
    }

    return nullptr;
}
