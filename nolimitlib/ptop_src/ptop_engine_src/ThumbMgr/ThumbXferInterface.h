#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <AssetBase/BaseXferInterface.h>

class PluginBase;

class ThumbXferInterface : public BaseXferInterface
{
public:
    ThumbXferInterface( PluginBase& pluginBase );

    virtual VxMutex&            getAssetXferMutex( void ) override;
    virtual EPluginType         getPluginType( void ) override;
    virtual std::string         getAssetXferDbName( void ) override;
    virtual std::string         getAssetXferThreadName( void ) override;

    virtual bool                txPacket( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, bool bDisconnectAfterSend = false ) override;

    PluginBase&                 m_PluginBase;
    AutoXferLock                m_AutoXferLock;
};