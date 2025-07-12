#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBaseFiles.h"

#include "FileInfoPersonFileXferMgr.h"

class PluginPersonFileXfer : public PluginBaseFiles
{
public:

    PluginPersonFileXfer( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginPersonFileXfer() override = default;

    EMediaModule			    getMediaModule( void ) override { return eMediaModuleInvalid; }

    void				        onContactWentOnline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase ) override {};
    void				        onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase ) override {};
    void				        replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt ) override {};
    void				        onConnectionLost( std::shared_ptr<VxSktBase>& sktBase ) override {};

    void						onContactOnlineStatusChange( VxGUID& onlineId, bool isOnline ) override {};

    void                        onPktPluginOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

protected:
	FileInfoPersonFileXferMgr	m_FileInfoPersonFileXferMgr;
};
