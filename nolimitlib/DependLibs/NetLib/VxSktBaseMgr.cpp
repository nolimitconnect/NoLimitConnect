//============================================================================
// Copyright (C) 2008 Brett R. Jones 
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

#include "VxSktBase.h"
#include "VxSktBaseMgr.h"
#include "VxSktUtil.h"
#include "VxSktAccept.h"

#include <CoreLib/VxGlobals.h>

#include <stdio.h>
#include <memory.h>

namespace
{
	//============================================================================
	void VxSktBaseMgrReceiveFunction(  std::shared_ptr<VxSktBase>& sktBase, void * pvUserData )
	{
		vx_assert( sktBase );
		vx_assert( sktBase->m_SktMgr );
		sktBase->m_SktMgr->doReceiveCallback( sktBase );
	}
}

//============================================================================
VxSktBaseMgr::VxSktBaseMgr()
{
	m_pfnOurReceive = VxSktBaseMgrReceiveFunction;
	m_uiCreatorVxThreadId = VxGetCurrentThreadId();
}

//============================================================================
VxSktBaseMgr::~VxSktBaseMgr()
{
	closeAllSkts();
	deleteAllSockets();
}

//============================================================================
// find a socket.. assumes list has been locked
std::shared_ptr<VxSktBase> VxSktBaseMgr::findSktBase( const VxGUID& connectId, bool acceptSktsOnly )
{
    if( connectId.isVxGUIDValid() && ( !acceptSktsOnly || eSktMgrTypeTcpAccept == m_eSktMgrType ) )
    {
        for( auto sktBase : m_aoSkts )
        {
            if( sktBase && sktBase->getSocketId() == connectId )
            {
                return sktBase;
            }
        }
    }

    std::shared_ptr<VxSktBase> nullSktBase;
    return nullSktBase;
}

//============================================================================
void VxSktBaseMgr::sktMgrSetLocalIp( InetAddress& oLclIp )
{
	m_LclIp = oLclIp;
}


//============================================================================
void VxSktBaseMgr::sktMgrShutdown( void )
{
	closeAllSkts();
}

//============================================================================
void VxSktBaseMgr::deleteAllSockets()
{
	m_aoSktsToDelete.clear();
}

//============================================================================
void VxSktBaseMgr::setReceiveCallback( VX_SKT_CALLBACK pfnReceive, void* pvUserData )
{
	m_pfnUserReceive = pfnReceive;
	m_pvRxCallbackUserData = pvUserData;
}

//============================================================================
void VxSktBaseMgr::setSktMgrStatusCallback( VX_SKT_MGR_STATUS_CALLBACK pfnSktMgrStatus, void* pvUserData )
{
	m_pfnSktMgrStatus = pfnSktMgrStatus;
	m_pvSktMgrStatusCallbackUserData = pvUserData;
}

//============================================================================
//! lock access from other threads
void VxSktBaseMgr::sktBaseMgrLock( void )
{
    m_SktMgrMutex.lock(__FILE__, __LINE__);
}

//============================================================================
//! unlock access from other threads
void VxSktBaseMgr::sktBaseMgrUnlock( void )
{
    m_SktMgrMutex.unlock(__FILE__, __LINE__);
}

//============================================================================
//! add a new socket to manage
void VxSktBaseMgr::addSkt( std::shared_ptr<VxSktBase>& sktBase )
{
	//LogMsg( LOG_INFO, "Adding %s to VxSktBaseMgr skt list\n", sktBase->describeSktType().c_str() );
	sktBaseMgrLock();
	m_aoSkts.emplace_back( sktBase );
	sktBaseMgrUnlock();
}

//============================================================================
//! remove a socket from management
RCODE VxSktBaseMgr::removeSkt(  std::shared_ptr<VxSktBase>&	sktBase,		// skt to remove
								bool		bDelete )	// if true delete the skt
{
	RCODE rc = -1;
	if( sktBase.get() )
	{
		//LogMsg( LOG_INFO, "Removing Skt ID %d  type %s from VxSktBaseMgr skt list\n", sktBase->getSktNumber(), sktBase->describeSktType().c_str() );

		sktBaseMgrLock();
		for( auto iter = m_aoSkts.begin(); iter != m_aoSkts.end(); ++iter )
		{
			if( (iter->get()) == sktBase.get() )
			{
				// found it in our list
				LogMsg( LOG_VERBOSE, "%s Deleting Skt ID %d type %s from VxSktBaseMgr skt list", __func__, sktBase->getSktNumber(), sktBase->describeSktType().c_str() );
				m_aoSkts.erase( iter );
				rc = 0;
				break;
			}
		}

		sktBaseMgrUnlock();
	}
	else
	{
		 LogMsg( LOG_ERROR, "VxSktBaseMgr %s null sktBase", __func__ );
	}

	return rc;
}

//============================================================================
bool VxSktBaseMgr::isSktActive( std::shared_ptr<VxSktBase>& sktBase )
{
    if( !sktBase )
    {
        LogMsg( LOG_ERROR, "VxSktBaseMgr::isSktActive null sktBase" );
        return false;
    }

	bool isActive = false;
    vx_assert( sktBase );
    if( sktBase == m_SktLoopback )
    {
        return true;
    }

	sktBaseMgrLock();
	for( auto iter = m_aoSkts.begin(); iter != m_aoSkts.end(); ++iter )
	{
		if( (*iter) == sktBase )
		{
			// found it in our list
			isActive = true;
			break;
		}
	}

	sktBaseMgrUnlock();
	return isActive;
}

//============================================================================
//! Send to all connections
void VxSktBaseMgr::sendToAll(	char * pData,			// data to send
								int iDataLen )			// length of data
{
	sktBaseMgrLock();
	for( auto iter = m_aoSkts.begin(); iter != m_aoSkts.end(); ++iter )
	{
		std::shared_ptr<VxSktBase>& skt = (*iter);
		if( skt->isTxCryptoKeySet() )
		{
			skt->txEncrypted( pData, iDataLen );
		}
		else
		{
			RCODE rc = skt->sendData( pData, iDataLen );
			if( skt->isFatalSocketError( rc ) )
			{
				skt->closeSkt( eSktCloseTxFailed );
			}
		}
	}

	sktBaseMgrUnlock();
}

//============================================================================
//! get number of connected sockets
int VxSktBaseMgr::getConnectedCount( void )
{
	int iConnectedCnt = 0;

	sktBaseMgrLock();
	for( auto iter = m_aoSkts.begin(); iter != m_aoSkts.end(); ++iter )
	{
		if( (*iter)->isConnected() )
		{
			iConnectedCnt++;
		}
	}

	sktBaseMgrUnlock();
	return iConnectedCnt;
}

//============================================================================
//! close all sockets
void VxSktBaseMgr::closeAllSkts( void )
{
	sktBaseMgrLock();
	for( auto iter = m_aoSkts.begin(); iter != m_aoSkts.end(); ++iter )
	{
		if( (*iter)->isConnected() )
		{
			(*iter)->closeSkt(eSktCloseAll);
		}
	}

	sktBaseMgrUnlock();
}

//============================================================================
std::shared_ptr<VxSktBase>	VxSktBaseMgr::makeNewSkt( void )					
{ 
	return  std::shared_ptr<VxSktBase>();
}

//============================================================================
std::shared_ptr<VxSktBase> VxSktBaseMgr::makeNewAcceptSkt( void )				
{ 
    std::shared_ptr<VxSktBase> sharedSkt( new VxSktAccept() );
	sharedSkt->setThisSkt( sharedSkt ); // so skt can do callbacks without look up in manager
	return sharedSkt;
}

//============================================================================
//! handle transmit callbacks from sockets
void VxSktBaseMgr::doTransmitCallback( std::shared_ptr<VxSktBase>& sktBase )
{
	TxedPkt( sktBase->m_iLastTxLen );
}

//============================================================================
void VxSktBaseMgr::doReceiveCallback( std::shared_ptr<VxSktBase>& sktBase )
{
	ESktCallbackReason eCallbackReason = sktBase->getCallbackReason();
	m_pfnUserReceive( sktBase, m_pvRxCallbackUserData );
	if( eSktCallbackReasonClosed == eCallbackReason )
	{
		LogModule( eLogSkt, LOG_VERBOSE, "VxSktBaseMgr::doReceiveCallback: closed %s num %d", sktBase->describeSktType().c_str(), sktBase->getSktNumber() );
	}
}

//============================================================================
void VxSktBaseMgr::handleSktCloseEvent( std::shared_ptr<VxSktBase>& sktBase )
{
	//LogMsg( LOG_INFO, "VxSktBaseMgr::handleSktCloseEvent: for skt %d 0x%x \n", sktBase->m_SktNumber, sktBase );
    sktBaseMgrLock();
    uint64_t timeNow = GetTimeStampMs();
    for( auto& sktBase : m_aoSkts )
    {
        if( sktBase )
        {
            if( timeNow - sktBase->getLastActiveTimeMs() > SKT_ALIVE_TIMEOUT )
            {
                LogModule( eLogSkt, LOG_DEBUG, "Closing due to alive timeout %s skt %d handle %d", DescribeSktType( sktBase->getSktType() ), sktBase->getSktNumber(), sktBase->getSktHandle() );
                sktBase->dumpSocketStats();

                sktBase->closeSkt( eSktCloseImAliveTimeout );
            }
        }
    }

	sktBaseMgrUnlock();

    // put this skt in delete list to be deleted later
    moveToEraseList( sktBase );
    doSktDeleteCleanup();
}

//============================================================================
//! delete sockets that have expired
void VxSktBaseMgr::doSktDeleteCleanup()
{
    int64_t timeNowMs = GetGmtTimeMs();
    std::vector<std::shared_ptr<VxSktBase>> deleteSktList;

    bool deletedSkt = true;
    while( deletedSkt )
    {
        std::shared_ptr<VxSktBase> sktToDelete;
        deletedSkt = false;
        sktBaseMgrLock();
        auto iter = m_aoSktsToDelete.begin();
        // to be deleted sockets delete after 10 seconds
        while( iter != m_aoSktsToDelete.end() )
        {
            sktToDelete = ( *iter );
            if( timeNowMs > sktToDelete->getToDeleteTimeMs() )
            {
                if( sktToDelete->getInUseByRxThread() )
                {
                    LogMsg( LOG_ERROR, "socket id %d handle %d type %s still in use by thread after delete timeout %s", sktToDelete->getSktNumber(), 
                        sktToDelete->getSktHandle(),
                        DescribeSktType( sktToDelete->getSktType() ),
                        sktToDelete->describeSktConnection().c_str() );

                    if( INVALID_SOCKET != sktToDelete->getSktHandle() )
                    {
                        sktToDelete->doCloseThisSocketHandle( false );
                    }

                    ++iter;
                    break;
                }
                else
                {
                    deleteSktList.emplace_back( sktToDelete );
                    iter = m_aoSktsToDelete.erase( iter );
                    deletedSkt = true;
                    break;
                }
            }
            else
            {
                ++iter;
            }
        }

        sktBaseMgrUnlock();
    }   

    for( auto sktBase : deleteSktList )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "%s deleting skt %s", __func__, sktBase->describeSktConnection().c_str() );
		sktBase->getThisSkt().reset(); // clear self reference so when the last shared pointer is deleted the socket will be deleted.
		if( sktBase->getThisSkt().use_count() > 1 )
		{
			LogMsg( LOG_ERROR, "*** %s skt %d handle %d still has %d references %s ", __func__, sktBase->getSktNumber(), 
					sktBase->getSktHandle(), sktBase.use_count(), sktBase->describeSktConnection().c_str() );
		}
    }

	deleteSktList.clear();
}

//============================================================================
//! move to erase/delete when safe to do so
void VxSktBaseMgr::moveToEraseList( std::shared_ptr<VxSktBase>& sktBase )
{
	bool found{ false };
	if( sktBase )
	{
		m_SktMgrMutex.lock( __FILE__, __LINE__ ); // dont let other threads mess with array while we remove the socket

		// make sure not allready in the lists to be deleted or will get deleted twice
		for( auto& toDeleteSkt : m_aoSktsToDelete  )
		{
			if( toDeleteSkt.get() == sktBase.get() )
			{
				LogMsg( LOG_ERROR, "%s socket %d handle %d alredy in to delete list", __func__, sktBase->getSktNumber(), sktBase->getSktHandle() );
				m_SktMgrMutex.unlock( __FILE__, __LINE__ );
				return;
			}
		}

		for( auto iter = m_aoSkts.begin(); iter != m_aoSkts.end(); ++iter )
		{
			if( *iter == sktBase )
			{
				found = true;
				std::shared_ptr<VxSktBase> sktCopy = *iter; // make copy first so destructor is not called before cleanup can be done
				sktCopy->setToDeleteTimeMs( GetGmtTimeMs() + 30000 );
				m_aoSktsToDelete.emplace_back( sktCopy );
				sktCopy->shutdownSkt(); // shutdown threads etc.. do not handle packets after this point
				m_aoSkts.erase( iter );
				break;
			}
		}

		if( !found )
		{
			LogMsg( LOG_ERROR, "%s socket %d %p was not found in mgr list", __func__, sktBase->getSktNumber(), sktBase.get() );
		    sktBase->setToDeleteTimeMs( GetGmtTimeMs() + 30000 );
			m_aoSktsToDelete.emplace_back( sktBase );
		}

		m_SktMgrMutex.unlock( __FILE__, __LINE__ );
	}
	else
	{
		LogMsg( LOG_ERROR, "%s called with null socket", __func__ );
	}
}

//============================================================================
void VxSktBaseMgr::dumpSocketStats( const char* reason, bool fullDump )
{
    std::string reasonMsg = reason ? reason : "";
    LogModule( eLogSkt, LOG_DEBUG, "%s skt active %d to delete %d total in system %d", reasonMsg.c_str(), m_aoSkts.size(), m_aoSktsToDelete.size(), VxSktBase::getCurrentSktCount() );
    if( fullDump )
    {
        int sktCnt = 0;
        uint64_t timeNow = GetTimeStampMs();
        m_SktMgrMutex.lock( __FILE__, __LINE__ ); 
        for( auto& sktBase : m_aoSkts )
        {
            sktCnt++;
            if( sktBase )
            {
                std::string dmpReason = std::to_string( sktCnt ) + " - ";
                if( timeNow - sktBase->getLastActiveTimeMs() > SKT_ALIVE_TIMEOUT )
                {
                    sktBase->dumpSocketStats( dmpReason.c_str(), fullDump );
                }
            }
        }

        m_SktMgrMutex.unlock( __FILE__, __LINE__ );
    }
}

//============================================================================
bool VxSktBaseMgr::closeConnection( VxGUID& connectId, ESktCloseReason closeReason )
{
	bool wasClosed{ false };
	if( connectId.isVxGUIDValid() )
	{
		std::shared_ptr<VxSktBase> sktBase( nullptr );
		sktBaseMgrLock();
		for( auto iter = m_aoSkts.begin(); iter != m_aoSkts.end(); ++iter )
		{
			if( ( *iter )->getSocketId() == connectId )
			{
				sktBase = (*iter);
				break;
			}
		}

		sktBaseMgrUnlock();
		if( sktBase )
		{
			sktBase->closeSkt( closeReason, true );
			wasClosed = true;
		}
	}

	return wasClosed;
}
