#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <CoreLib/MediaCallbackInterface.h>
#include <CoreLib/VxGUID.h>
#include <CoreLib/VxGUIDList.h>

#include <memory>

class P2PEngine;
class PktVoiceReq;
class PluginBase;
class PluginMgr;
class PluginSessionMgr;
class VxNetIdent;
class VxSktBase;
class VxPktHdr;

class VoiceFeedMgr
{
public:
	VoiceFeedMgr( P2PEngine& engine, PluginBase& plugin, PluginSessionMgr& sessionMgr );

	virtual void				fromGuiStartPluginSession( bool pluginIsLocked, EAppModule appModule, VxNetIdent* netIdent = nullptr, bool wantAudioCapture = true );
	virtual void				fromGuiStopPluginSession( bool pluginIsLocked, EAppModule appModule, VxNetIdent* netIdent = nullptr, bool wantAudioCapture = true );

	virtual void				onPktVoiceReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );
	virtual void				onPktVoiceReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );

	virtual void				callbackOpusPkt( void * userData, PktVoiceReq * pktOpusAudio );
	virtual void				callbackAudioOutSpaceAvail( int freeSpaceLen );

	void						stopAllSessions( EAppModule appModule, EPluginType pluginType );

protected:
	void						enableAudioCapture( bool enable, VxNetIdent* netIdent, EAppModule appModule, bool wantAudioCapture = true );

    P2PEngine&                  m_Engine;
	PluginBase&					m_Plugin;
	PluginMgr&					m_PluginMgr;
	PluginSessionMgr&			m_SessionMgr;
	bool						m_Enabled;
	VxGUIDList					m_GuidList;
	bool						m_CamServerEnabled;
	bool						m_AudioPktsRequested;
	bool						m_MixerInputRequesed;

};

