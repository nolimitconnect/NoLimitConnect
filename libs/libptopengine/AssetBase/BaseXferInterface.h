#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <CoreLib/VxMutex.h>

#include <string>
#include <memory>

class VxGUID;
class VxNetIdent;
class VxPktHdr;
class VxSktBase;

class BaseXferInterface
{
public:
    virtual VxMutex&            getAssetXferMutex( void ) = 0;
    virtual EPluginType         getPluginType( void ) = 0;
    virtual EPluginType         getAssetOverridePluginType( void ) { return ePluginTypeInvalid; }
    virtual std::string         getAssetXferDbName( void ) { std::string dbName = GetPluginName( getPluginType() ); dbName += "Db.db3"; return dbName; }
    virtual std::string         getAssetXferThreadName( void ) { std::string thrdName = GetPluginName( getPluginType() ); thrdName += "Thrd"; return thrdName; }

    virtual bool                txPacket( const VxGUID sendToId, std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, EPluginType overridePluginType = ePluginTypeInvalid ) = 0;

    //! returns the expiry time ( occurrence end + event retention ) to tag a just-received
    //! asset with if hostOnlineId currently has an active calendar-event occurrence, else 0 --
    //! "every member and host independently decides" ( no wire signaling ): every peer already
    //! has the host's calendar cached ( CalendarMgr host-side / CalendarClientMgr client-side ),
    //! so each computes the same answer from data it already has. Default no-op here since most
    //! plugin types ( FileShare, Messenger, PersonalRecorder, ... ) aren't host/session
    //! contexts and have no calendar at all -- overridden in PluginBaseHostService/
    //! PluginBaseHostClient. Called from AssetBaseXferMgr::onPktAssetBaseSendReq, which is the
    //! single shared receive path for every plugin/host type, host and client alike -- see
    //! event-calendar design notes on the asset purge mechanism.
    virtual int64_t             getActiveCalendarEventExpiresTime( VxGUID& hostOnlineId, int64_t nowMs ) { return 0; }
};

class AutoXferLock
{
public:
    AutoXferLock( VxMutex& mutex ) 
        : m_Mutex(mutex)	
    { 
        m_Mutex.lock(); 
    }

    ~AutoXferLock()
    { 
        m_Mutex.unlock(); 
    }

    VxMutex&						m_Mutex;
};