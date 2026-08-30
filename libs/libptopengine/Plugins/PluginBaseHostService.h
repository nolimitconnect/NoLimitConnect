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

#include "PluginBaseMultimedia.h"
#include "HostServerMgr.h"

#include <Calendar/CalendarMgr.h>

#include <PktLib/PktsHostInvite.h>

class PluginBaseHostService : public PluginBaseMultimedia
{
public:
    PluginBaseHostService( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
    virtual ~PluginBaseHostService() override = default;

    EPluginType                 getClientPluginType( void ) override;
    virtual bool				getHostedInfo( HostedInfo& hostedInfo ) override;

    virtual void				onMyOnlineUrlIsValid( bool iValid ) override;

    virtual void				onPktHostJoinReq                ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostLeaveReq               ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUnJoinReq              ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktHostOfferReq               ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostOfferReply             ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktHostInfoReq                ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostInfoReply              ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostSearchReq              ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktHostUserInfoReq            ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserInfoReply          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktGroupieInfoReq             ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktGroupieInfoReply           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktGroupieAnnReq              ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktGroupieAnnReply            ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktGroupieSearchReq           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktGroupieSearchReply         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktGroupieMoreReq             ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktGroupieMoreReply           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktHostUserListReq            ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserListReply          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserListMoreReq        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserListMoreReply      ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktCalendarEventListReq       ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktCalendarEventUpdateReq     ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktCalendarEventCancelReq     ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    //! this host's own admin managing their own calendar directly -- no packet round-trip.
    //! see event-calendar design notes and CalendarMgr::localEventUpdate et al.
    virtual void				fromGuiCalendarEventUpdate      ( HostedId& adminId, CalendarEventInfo& eventInfo ) override;
    virtual void				fromGuiCalendarEventCancel      ( HostedId& adminId, VxGUID& eventId ) override;
    virtual void				fromGuiCalendarRefresh          ( HostedId& adminId ) override;

    virtual void				onAfterUserLogOnThreaded        ( void ) override;

    virtual bool				fromGuiRequestPluginThumb       ( VxNetIdent* netIdent, VxGUID& thumbId ) override;
    virtual bool                ptopEngineRequestPluginThumb    ( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, VxGUID& thumbId, bool tmpThumb = false ) override;

    virtual bool                setPluginSetting( PluginSetting& pluginSetting, int64_t lastModifiedTime = 0 ) override;
    virtual	void				onPluginSettingChange( PluginSetting& pluginSetting, int64_t lastModifiedTime = 0 ) override;
    virtual void				onThreadOncePer15Minutes( void ) override;
    virtual void				onThreadOncePerMinute( void ) override;

    virtual int64_t             getActiveCalendarEventExpiresTime( VxGUID& hostOnlineId, int64_t nowMs ) override;

    virtual void                onUserJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent ) override;
    virtual void                onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent ) override;
    virtual void                onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent ) override;
    virtual void                onGroupDirectUserAnnounce( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent ) override;

protected:
    virtual void				onContactWentOffline            ( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase ) override;
    virtual void				onConnectionLost                ( std::shared_ptr<VxSktBase>& sktBase ) override;
    virtual void				replaceConnection               ( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt ) override;

    void						onContactOnlineStatusChange( ConnectId& connectId, bool isOnline ) override;

    virtual void                buildHostAnnounce( PluginSetting& pluginSetting );
    virtual void				sendHostAnnounce( void );
    virtual bool				getHostTitleAndDescription( std::string& hostTitle, std::string& hostDesc, int64_t& lastModifiedTime );
    virtual void				onPluginSettingsChanged( int64_t modifiedTimeMs );

    virtual void				updateHostInvite( PluginSetting& pluginSetting );
    virtual void				updateHostInviteUrl( void );

    ECommErr                    getCommAccessState( VxNetIdent* netIdent );

public:
    //! true while the admin currently has this host's admin screen open ( set/cleared from
    //! fromGuiAdminViewHost() in PluginChatRoomHost/PluginGroupHost/PluginRandomConnectHost,
    //! which already fires exactly on admin-applet open/close -- this just persists what used
    //! to be a fire-and-forget broadcast ). consulted by CalendarMgr::extendActiveEventExpiryTimes
    //! to decide whether a session running past its scheduled end may still be extended -- see
    //! event-calendar design notes.
    void                        setAdminIsViewing( bool isViewing )    { m_AdminIsViewing = isViewing; }
    bool                        isAdminViewing( void )                  { return m_AdminIsViewing; }

protected:
    //=== vars ===//
    HostServerMgr               m_HostServerMgr;
    CalendarMgr                 m_CalendarMgr;
    bool                        m_AdminIsViewing{ false };

    std::string                 m_HostInviteUrl;
    std::string                 m_HostTitle;
    std::string                 m_HostDescription;
    int64_t                     m_HostInfoModifiedTime{ 0 };
    bool                        m_PktHostInviteIsValid{ false };
    PktHostInviteAnnounceReq    m_PktHostInviteAnnounceReq;

    VxMutex                     m_AnnMutex;
    VxGUID                      m_AnnounceSessionId;
    int64_t                     m_PktAnnLastModTime{ 0 };
};

