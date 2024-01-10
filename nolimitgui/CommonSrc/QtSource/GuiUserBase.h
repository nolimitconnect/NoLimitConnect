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

#include <QWidget>

#include <PktLib/VxCommon.h>

#include <set>

class AppCommon;
class VxNetIdent;
class VxSktBase;

class GuiUserBase : public QWidget
{
    Q_OBJECT
public:
    static const int NEARBY_TIMEOUT_MS = 10000;

    GuiUserBase() = delete;
    GuiUserBase( AppCommon& app );
    GuiUserBase( AppCommon& app, VxNetIdent* netIdent, VxGUID& sessionId );
    GuiUserBase( const GuiUserBase& rhs );
	virtual ~GuiUserBase() override = default;

    bool                        isValid( void )                         { return isValidNetIdent(); }
    bool                        isValidNetIdent( void )                 { return m_NetIdent.isValidNetIdent(); }

    void                        setNetIdent( VxNetIdent* netIdent );
    VxNetIdent&                 getNetIdent( void )                     { return m_NetIdent; }
   
    void                        setSessionId( VxGUID& sessionId )       { m_SessionId = sessionId; }
    VxGUID&                     getSessionId( void )                    { return m_SessionId; }

    bool                        updateIsNearby( void );
    virtual bool                setNearbyStatus( int64_t nearbyTimeOrZeroIfNotd ); // return false if nearbyTime is zero
    bool                        isNearby( void );

    virtual bool                isOnline( void );
    bool                        isDirectConnect( void );
    bool                        isRelayed( void );

    bool                        isAutomatedHost( void )                 { return m_NetIdent.isAutomatedHost(); }

    bool                        isAdmin( void )                         { return m_NetIdent.isAdministrator(); }
    bool                        isFriend( void )                        { return m_NetIdent.isFriend(); }
    bool                        isGuest( void )                         { return m_NetIdent.isGuest(); }
    bool                        isAnonymous( void )                     { return m_NetIdent.isAnonymous(); }
    bool                        isIgnored( void )                       { return m_NetIdent.isIgnored(); }
    bool                        isMyself( void );

    bool                        isHosted( void );
    bool                        isGroupHosted( void );
    bool                        isChatRoomHosted( void );
    bool                        isRandomConnectHosted( void );

    bool                        isInSession( void );

    VxGUID&                     getMyOnlineId( void )                   { return m_OnlineId; }
    std::string                 getOnlineName( void )                   { return std::string( m_NetIdent.getOnlineName() ); }
    std::string                 getOnlineDescription( void )            { return std::string( m_NetIdent.getOnlineDescription() ); }
    std::string                 getMyOnlineUrl( bool ipv6 = false )     { return m_NetIdent.getMyOnlineUrl( ipv6 ); }
    VxGUID                      getHostThumbId( EHostType hostType, bool defaultToAvatarThumbId ) { return m_NetIdent.getHostThumbId( hostType, defaultToAvatarThumbId ); } 
    VxGUID                      getAvatarThumbId( void )                { return m_NetIdent.getAvatarThumbGuid(); }

    void                        setMyFriendshipToHim( EFriendState friendState ) { m_NetIdent.setMyFriendshipToHim( friendState ); }
    EFriendState                getMyFriendshipToHim( void )            { return m_NetIdent.getMyFriendshipToHim(); }
    EFriendState                getHisFriendshipToMe( void )            { return m_NetIdent.getHisFriendshipToMe(); }
    EPluginAccess               getMyAccessPermissionFromHim( EPluginType pluginType, bool inGroup = false );
    EFriendState                getPluginPermission( EPluginType pluginType ) { return m_NetIdent.getPluginPermission( pluginType ); }
    int64_t					    getLastSessionTimeMs( void )            { return m_NetIdent.getLastSessionTimeMs(); }

    QString				        describeMyFriendshipToHim( bool inGroup  );
    QString				        describeHisFriendshipToMe( bool inGroup );

    uint32_t                    getTruthCount( void )                   { return m_NetIdent.getTruthCount(); }
    uint32_t                    getDareCount( void )                    { return m_NetIdent.getDareCount(); }
    void		                setHasTextOffers( bool hasOffers )	    { m_NetIdent.setHasTextOffers( hasOffers ); }
    bool		                getHasTextOffers( void )				{ return m_NetIdent.getHasTextOffers(); }

    bool                        isMyAccessAllowedFromHim( enum EPluginType pluginType, bool inGroup = false );
    bool						isHisAccessAllowedFromMe( enum EPluginType pluginType, bool inGroup = false ) { return m_NetIdent.isHisAccessAllowedFromMe( pluginType, inGroup ); }

    void                        setLastUpdateTime( uint64_t updateTimeMs ) { m_LastUpdateTime = updateTimeMs; }
    uint64_t                    getLastUpdateTime( void )               { return m_LastUpdateTime; }

    void						emitUserUpdated( void )                 { emit signalUserUpdated(); };

    virtual QString             describeUser( bool verbose = false );

signals:
    void						signalUserUpdated( void );

protected:
    AppCommon&                  m_MyApp;
    VxNetIdent                  m_NetIdent;
    VxGUID                      m_OnlineId;
    VxGUID                      m_SessionId;
    uint64_t                    m_LastUpdateTime{ 0 };
    int64_t                     m_NearbyTimeOrZero{ 0 };
};
