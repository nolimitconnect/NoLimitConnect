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

#include <config_appcorelibs.h>

#include <CoreLib/VxMutex.h>

#include <memory>

class VxNetIdent;
class VxPktHdr;
class VxSktBase;

class BaseXferInterface
{
public:
    virtual VxMutex&            getAssetXferMutex( void ) = 0;
    virtual EPluginType         getPluginType( void ) = 0;
    virtual bool                txPacket( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, bool bDisconnectAfterSend = false ) = 0;

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