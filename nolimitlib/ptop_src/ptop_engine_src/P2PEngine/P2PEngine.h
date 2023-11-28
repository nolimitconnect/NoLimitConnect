#pragma once
//============================================================================
// Copyright (C) 2009 Brett R. Jones 
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

#include "config_appcorelibs.h"

#include "P2PConnectList.h"
#include "EngineSettings.h"
#include "EngineParams.h"

#include <ptop_src/ptop_engine_src/AssetMgr/AssetCallbackInterface.h>
#include <ptop_src/ptop_engine_src/BlobXferMgr/BlobCallbackInterface.h>
#include <ptop_src/ptop_engine_src/Connections/ConnectionMgr.h>
#include <ptop_src/ptop_engine_src/ConnectMgr/ConnectMgr.h>
#include <ptop_src/ptop_engine_src/GroupieListMgr/GroupieListMgr.h>
#include <ptop_src/ptop_engine_src/HostListMgr/HostedListMgr.h>
#include <ptop_src/ptop_engine_src/HostListMgr/HostUrlListMgr.h>

#include <ptop_src/ptop_engine_src/ConnectIdListMgr/ConnectIdListMgr.h>
#include <ptop_src/ptop_engine_src/IdentListMgrs/FriendListMgr.h>
#include <ptop_src/ptop_engine_src/IdentListMgrs/IgnoreListMgr.h>
#include <ptop_src/ptop_engine_src/IdentListMgrs/NearbyListMgr.h>

#include <ptop_src/ptop_engine_src/Membership/MembershipAvailableMgr.h>
#include <ptop_src/ptop_engine_src/Membership/MembershipHostedMgr.h>
#include <ptop_src/ptop_engine_src/NetworkMonitor/NetStatusAccum.h>
#include <ptop_src/ptop_engine_src/OfferBase/OfferMgr.h>
#include <ptop_src/ptop_engine_src/PluginSettings/PluginSettingMgr.h>
#include <ptop_src/ptop_engine_src/ThumbMgr/ThumbCallbackInterface.h>
#include <ptop_src/ptop_engine_src/ThumbMgr/ThumbMgr.h>
#include <ptop_src/ptop_engine_src/WebPageMgr/WebPageMgr.h>

#include <GuiInterface/IDefs.h>
#include <GuiInterface/IFromGui.h>
#include <GuiInterface/IAudioInterface.h>

#include <ptop_src/ptop_engine_src/Relay/RelayMgr.h>
#include <ptop_src/ptop_engine_src/Search/RcScan.h>

#include <ptop_src/ptop_engine_src/BigListLib/BigListMgr.h>
#include <ptop_src/ptop_engine_src/Plugins/PluginLibraryServer.h>

#include <NetLib/VxSktDefs.h>
#include <PktLib/PktAnnounce.h>
#include <PktLib/PktSysHandlerBase.h>
#include <PktLib/PktsImAlive.h>

#include <memory.h>

class AssetMgr;
class BlobMgr;
class ConnectMgr;
class ConnectRequest;
class FileShareSettings;
class HostJoinMgr;
class IToGui;
class INlc;
class IsPortOpenTest;
class PluginMgr;
class RcConnectInfo;
class MediaProcessor;
class NetworkMgr;
class NetworkStateMachine;
class NetConnector;
class NetworkMonitor;
class NetServicesMgr;
class PluginNetServices;
class PluginFileShareServer;
class PluginSetting;
class PluginSettingMgr;
class RunUrlAction;
class UrlMgr;
class UserJoinMgr;
class UserOnlineMgr;
class VxPeerMgr;

class P2PEngine :	public IFromGui,
					public PktHandlerBase,
                    public AssetCallbackInterface,
                    public BlobCallbackInterface,
					public MediaCallbackInterface,
                    public IAudioCallbacks
{
public:
    P2PEngine() = delete; // don't allow default constructor
    P2PEngine( const P2PEngine& ) = delete; // don't allow copy constructor

	P2PEngine( VxPeerMgr& peerMgr );
	virtual ~P2PEngine() override;

	void						startupEngine( void );
	void						shutdownEngine( void );
    bool                        isEngineCreated( void )                         { return m_IsEngineCreated; }
    bool                        isEngineReady( void )                           { return m_IsEngineReady; }

    IToGui&						getToGui( void );
	IFromGui&					getFromGuiInterface( void )						{ return *this; }
    IAudioRequests&			    getAudioRequest( void );
    AssetMgr&					getAssetMgr( void )								{ return m_AssetMgr; }
    BigListMgr&					getBigListMgr( void )							{ return m_BigListMgr; }
    ConnectionMgr&              getConnectionMgr( void )                        { return m_ConnectionMgr; }
    ConnectMgr&                 getConnectMgr( void )                           { return m_ConnectMgr; }
    BlobMgr&				    getBlobMgr( void )							    { return m_BlobMgr; }
    EngineSettings&				getEngineSettings( void )						{ return m_EngineSettings; }
	EngineParams&				getEngineParams( void )							{ return m_EngineParams; }
    ConnectIdListMgr&           getConnectIdListMgr( void )                     { return m_ConnectIdListMgr; }
    FriendListMgr&              getFriendListMgr( void )                        { return m_FriendListMgr; }
    GroupieListMgr&             getGroupieListMgr( void )                       { return m_GroupieListMgr; }
    HostUrlListMgr&             getHostUrlListMgr( void )                       { return m_HostUrlListMgr; }
    HostedListMgr&              getHostedListMgr( void )                        { return m_HostedListMgr; }
    IgnoreListMgr&              getIgnoreListMgr( void )                        { return m_IgnoreListMgr; }
    MembershipAvailableMgr&     getMembershipAvailableMgr( void )               { return m_MembershipAvailableMgr; }
    MembershipHostedMgr&        getMembershipHostedMgr( void )                  { return m_MembershipHostedMgr; }
    NearbyListMgr&              getNearbyListMgr( void )                        { return m_NearbyListMgr; }
    NetConnector&				getNetConnector( void )							{ return m_NetConnector; }
    NetStatusAccum&             getNetStatusAccum( void )                       { return m_NetStatusAccum; }
    NetworkMgr&					getNetworkMgr( void )							{ return m_NetworkMgr; }
    NetworkStateMachine&		getNetworkStateMachine( void )					{ return m_NetworkStateMachine; }
	NetworkMonitor&				getNetworkMonitor( void )						{ return m_NetworkMonitor; } 
	NetServicesMgr&				getNetServicesMgr( void )						{ return m_NetServicesMgr; }
	MediaProcessor&				getMediaProcessor( void )						{ return m_MediaProcessor; }
    OfferMgr&                   getOfferMgr( void )                             { return m_OfferMgr; }
	VxPeerMgr&					getPeerMgr( void )								{ return m_PeerMgr; }
    P2PConnectList&             getConnectList( void )                          { return m_ConnectionList; }
    PluginMgr&					getPluginMgr( void )							{ return m_PluginMgr; }
    PluginSettingMgr&			getPluginSettingMgr( void )						{ return m_PluginSettingMgr; }
    RelayMgr&                   getRelayMgr( void )                             { return m_RelayMgr; }
    RcScan&						getRcScan( void )								{ return m_RcScan; }
    RunUrlAction&               getRunUrlAction( void )                         { return m_RunUrlAction; }

    ThumbMgr&                   getThumbMgr( void )                             { return m_ThumbMgr; }
    UrlMgr&                     getUrlMgr( void );

    HostJoinMgr&                getHostJoinMgr( void )                          { return m_HostJoinMgr; }
    UserJoinMgr&                getUserJoinMgr( void )                          { return m_UserJoinMgr; }
    UserOnlineMgr&              getUserOnlineMgr( void )                        { return m_UserOnlineMgr; }
    WebPageMgr&                 getWebPageMgr( void )                           { return m_WebPageMgr; }

    std::shared_ptr<VxSktBase>& getSktLoopback( void )                          { return m_SktLoopback; }

	bool						isAppPaused( void )								{ return m_AppIsPaused; }
    bool                        isNearbyAvailable( void );
    bool						isInternetAvailable( void );        // is internet available
    bool						isDirectConnectTested( void );      // has direct connect test completed
	bool						isP2POnline( void );
    bool                        isDirectConnectReady( void );       // true if have open port and ready to recieve
    bool                        isNetworkHostEnabled( void );       // true if netowrk host plugin is enabled

    bool                        getIsMyHostServiceEnabled( EHostServiceType hostService );
    bool                        getIsMyHostServiceEnabled( EHostType hostService );

    bool                        getHasAnyAnnonymousHostService( void );
    bool                        getHasFixedIpAddress( void );

    void						lockAnnouncePktAccess( void )					{ m_AnnouncePktMutex.lock(); }
    void						unlockAnnouncePktAccess( void )					{ m_AnnouncePktMutex.unlock(); }

    void						copyMyPktAnnounce( PktAnnounce& pktAnn )		{ m_AnnouncePktMutex.lock(); memcpy(&pktAnn, &m_PktAnn, sizeof(PktAnnounce)); m_AnnouncePktMutex.unlock(); }
	PktAnnounce&				getMyPktAnnounce( void )						{ return m_PktAnn; }

    void                        setPktAnnLastModTime( int64_t timeMs )          { m_PktAnnLastModTime = timeMs; }
    int64_t                     getPktAnnLastModTime( void )                    { return m_PktAnnLastModTime; }

    VxGUID&						getMyOnlineId( void )							{ return m_MyOnlineId; }
    std::string					getMyOnlineUrl( bool ipv6, EHostType hostType = eHostTypeUnknown ) { m_AnnouncePktMutex.lock(); std::string myUrl( m_PktAnn.getMyOnlineUrl( ipv6, hostType ) ); m_AnnouncePktMutex.unlock(); return myUrl; }
    VxNetIdent*				    getMyNetIdent( void )						    { return &m_PktAnn; }
    bool						addMyIdentToBlob( PktBlobEntry& blobEntry );

    bool                        setPluginSetting( PluginSetting& pluginSetting );
    bool                        getPluginSetting( EPluginType pluginType, PluginSetting& pluginSetting );

    virtual void				setPluginPermission( EPluginType pluginType, int iPluginPermission );
    virtual EFriendState		getPluginPermission( int iPluginType );

    PluginLibraryServer&        getPluginLibraryServer( void )                  { return *m_PluginLibraryServer; }
	PluginFileShareServer&		getPluginFileShareServer( void )				{ return *m_PluginFileShareServer; }
	PluginNetServices&			getPluginNetServices( void )					{ return *m_PluginNetServices; }

	virtual void				setHasPicture( int bHasPicture );
	virtual void				setHasSharedWebCam( int bHasShaeredWebCam );
	bool						isContactConnected( VxGUID& onlineId );
	bool						isSystemPlugin( EPluginType	pluginType );

    bool                        isUserConnected( VxGUID& onlineId );
    bool                        isMemberGuest( EPluginType pluginType, VxGUID& onlineId ); // true if user is member of same host as I am

	//========================================================================
	// from gui
	//========================================================================
	void						assureUserSpecificDirIsSet( const char* checkReason );

    virtual void				fromGuiSetIsAppCommercial( bool isCommercial ) override;
    virtual bool				fromGuiGetIsAppCommercial( void ) override;
    virtual uint16_t			fromGuiGetAppVersionBinary( void ) override;
    const char*				    fromGuiGetAppVersionString( void ) override;
    virtual const char*		    fromGuiGetAppName( void ) override;
    virtual const char*		    fromGuiGetAppNameNoSpaces( void ) override;
    virtual void				fromGuiAppStartup( const char* assetDir, const char* rootDataDir  ) override;

    virtual void				fromGuiSetUserSpecificDir( const char* userSpecificDir  ) override;
    virtual void				fromGuiSetUserXferDir( const char* userXferDir  ) override;
    virtual uint64_t			fromGuiGetDiskFreeSpace( void  ) override;
    virtual uint64_t			fromGuiClearCache( ECacheType cacheType ) override;
    virtual void				fromGuiAppShutdown( void  ) override;
    virtual void				fromGuiAppPause( void ) override;
    virtual void				fromGuiAppResume( void ) override;

    virtual void				fromGuiUserLoggedOn( VxNetIdent* netIdent ) override;
    void                        updateFromEngineSettings( EngineSettings& engineSettings ); 
    virtual void				fromGuiOnlineNameChanged( const char* newOnlineName ) override;
    virtual void				fromGuiMoodMessageChanged( const char* newMoodMessage ) override;
    virtual void				fromGuiIdentPersonalInfoChanged( int age, int gender, int language, int preferredContent ) override;

    virtual void				fromGuiSetUserHasProfilePicture( bool haveProfilePick ) override;
    virtual void				fromGuiUpdateMyIdent( VxNetIdent* netIdent, bool permissionAndStatsOnly = false ) override;
    virtual void				fromGuiQueryMyIdent( VxNetIdent* poRetIdent ) override;
    virtual void				fromGuiSetIdentHasTextOffers( VxGUID& onlineId, bool hasTextOffers ) override;

    virtual bool				fromGuiOrientationEvent( float f32RotX, float f32RotY, float f32RotZ  ) override;
    virtual bool				fromGuiMouseEvent( EMouseButtonType eMouseButType, EMouseEventType eMouseEventType, int iMouseXPos, int iMouseYPos  ) override;
    virtual bool				fromGuiMouseWheel( float f32MouseWheelDist ) override;
    virtual bool				fromGuiKeyEvent( EKeyEventType eKeyEventType, EKeyCode eKey, int iFlags = 0 ) override;

    virtual void				fromGuiNativeGlInit( void ) override;
    virtual void				fromGuiNativeGlResize( int w, int h ) override;
    virtual int					fromGuiNativeGlRender( void ) override;
    virtual void				fromGuiNativeGlPauseRender( void ) override;
    virtual void				fromGuiNativeGlResumeRender( void ) override;
    virtual void				fromGuiNativeGlDestroy( void ) override;

    virtual void				fromGuiMuteMicrophone( bool muteMic ) override;
    virtual bool				fromGuiIsMicrophoneMuted( void ) override;
    virtual void				fromGuiMuteSpeaker(	bool muteSpeaker ) override;
    virtual bool				fromGuiIsSpeakerMuted( void ) override;

    virtual bool				fromGuiSndRecord( ESndRecordState eRecState, VxGUID& feedId, const char* fileName ) override;
    virtual bool				fromGuiVideoRecord( EVideoRecordState eRecState, VxGUID& feedId, const char* fileName ) override;
    virtual bool				fromGuiPlayLocalMedia( const char* fileName, uint64_t fileLen, uint8_t fileType, int pos0to100000 = 0 ) override;
    virtual bool				fromGuiPlayLocalMedia( const char* fileName, uint64_t fileLen, uint8_t fileType, VxGUID assetId, int pos0to100000 = 0 ) override;

    virtual bool				fromGuiAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo, int pos0to100000 = 0 ) override;
    virtual bool				fromGuiAssetAction( EPluginType pluginType, EAssetAction assetAction, VxGUID& assetId, int pos0to100000 = 0 ) override;
    virtual bool				fromGuiSendAsset( AssetBaseInfo& assetInfo ) override;

    virtual void				fromGuiWantMediaInput( EMediaInputType mediaType, MediaCallbackInterface * callback, void * userData, EAppModule appModule, bool wantInput ) override;
    virtual void				fromGuiWantMediaInput( VxGUID& onlineId, EMediaInputType mediaType, EAppModule appModule, bool wantInput ) override;

    virtual void				fromGuiVideoData( uint32_t u32FourCc, uint8_t * pu8VidDataIn, int iWidth, int iHeight, uint32_t u32VidDataLen, int iRotation ) override;
    virtual bool				fromGuiMovieDone( void ) override							{ return true; };

    virtual void				fromGuiNetworkAvailable( const char* lclIp = nullptr, bool isCellularNetwork = false ) override;
    virtual void				fromGuiNetworkLost( void ) override;
    virtual ENetLayerState	    fromGuiGetNetLayerState( ENetLayerType netLayer = eNetLayerTypeInternet ) override;

    virtual void				fromGuiNetworkSettingsChanged( void ) override;

    virtual void				fromGuiAnnounceHost( EHostType hostType, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 ) override;
    virtual void				fromGuiJoinHost( EHostType hostType, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 ) override;
    virtual void				fromGuiLeaveHost( EHostType hostType, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 ) override;
    virtual void				fromGuiUnJoinHost( EHostType hostType, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 ) override;
    virtual void				fromGuiJoinLastJoinedHost( EHostType hostType, VxGUID& sessionId ) override;
    virtual void				fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable ) override;
    virtual void				fromGuiSendAnnouncedList( EHostType hostType, VxGUID& sessionId ) override;

    virtual void				fromGuiRunIsPortOpenTest( uint16_t port ) override;
    virtual void				fromGuiRunUrlAction( VxGUID& sessionId, const char* myUrl, const char* ptopUrl, ENetCmdType testType ) override;

	virtual void				fromGuiUpdateWebPageProfile(	const char*	pProfileDir,	// directory containing user profile
																const char*	strGreeting,	// greeting text
																const char*	aboutMe,		// about me text
																const char*	url1,			// favorite url 1
																const char*	url2,			// favorite url 2
                                                                const char*	url3,           // favorite url 3
                                                                const char*	donation ) override;	// donation		

    virtual void				fromGuiApplyNetHostSettings( NetHostSetting& netSettings ) override;
    virtual void				fromGuiSetNetSettings( NetSettings& netSettings ) override;
    virtual void				fromGuiGetNetSettings( NetSettings& netSettings ) override;
    virtual void				fromGuiSetRelaySettings( int userRelayMaxCnt, int systemRelayMaxCnt ) override;

    virtual void				fromGuiGetFileShareSettings( FileShareSettings& fileShareSettings ) override;
    virtual void				fromGuiSetFileShareSettings( FileShareSettings& fileShareSettings ) override;

    virtual void				fromGuiSetPluginPermission( EPluginType pluginType, EFriendState eFriendState ) override;
    virtual int					fromGuiGetPluginPermission( EPluginType pluginType ) override;
    virtual EPluginServerState	fromGuiGetPluginServerState( EPluginType pluginType ) override;

    virtual void				fromGuiStartPluginSession( EPluginType pluginType, VxGUID onlineId, int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
    virtual void				fromGuiStopPluginSession( EPluginType pluginType, VxGUID onlineId, int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID()  ) override;
    virtual bool				fromGuiIsPluginInSession( EPluginType pluginType, VxNetIdent* netIdent = nullptr, int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;

	virtual bool				fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo ) override;
    virtual bool				fromGuiToPluginOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo ) override;

    virtual EXferError			fromGuiFileXferControl( EPluginType pluginType, EXferAction xferAction, FileInfo& fileInfo ) override;

	virtual bool				fromGuiInstMsg(	EPluginType	pluginType, VxGUID&	onlineId, const char* pMsg ) override;
                                                   
    virtual bool				fromGuiPushToTalk( VxGUID& onlineId, bool enableTalk ) override;

	virtual bool				fromGuiChangeMyFriendshipToHim(	VxGUID&	onlineId, EFriendState myFriendshipToHim, EFriendState hisFriendshipToMe ) override;															
                                                                
    virtual void				fromGuiSendContactList( EFriendViewType eFriendView, int maxContactsToSend ) override;
    virtual void				fromGuiRefreshContactList( int maxContactsToSend ) override;

    virtual void				fromGuiRequireRelay( bool bRequireRelay ) override;

    virtual void				fromGuiRelayPermissionCount( int userPermittedCount, int anonymousCount );

    virtual void				fromGuiStartScan( EScanType eScanType, uint8_t searchFlags, uint8_t fileTypeFlags, const char* pSearchPattern = "" ) override;
    virtual void				fromGuiNextScan( EScanType eScanType ) override;
    virtual void				fromGuiStopScan( EScanType eScanType ) override;

    virtual InetAddress			fromGuiGetMyIPv4Address( void ) override;
    virtual InetAddress			fromGuiGetMyIPv6Address( void ) override;

    virtual void				fromGuiUserModifiedStoryboard( void ) override;

    virtual void				fromGuiCancelDownload( VxGUID& fileInstance ) override;
    virtual void				fromGuiCancelUpload( VxGUID& fileInstance ) override;

	virtual bool				fromGuiSetGameValueVar( EPluginType	pluginType, VxGUID& onlineId, int32_t varId, int32_t varValue ) override;
														                                            
	virtual bool				fromGuiSetGameActionVar( EPluginType pluginType, VxGUID& onlineId, int32_t actionId, int32_t actionValue ) override;

	virtual bool				fromGuiTestCmd(	ETestParam1 testParam1, int	testParam2 = 0, const char* testParam3 = nullptr ) override;                                            

    virtual uint16_t			fromGuiGetRandomTcpPort( void ) override;
    /// Get url for this node
    virtual void                fromGuiGetNodeUrl( bool ipv6, std::string& nodeUrl ) override;
    /// Get internet status
    virtual EInternetStatus     fromGuiGetInternetStatus( void ) override;
    /// Get network status
    virtual ENetAvailStatus     fromGuiGetNetAvailStatus( void ) override;
    virtual bool				fromGuiNearbyBroadcastEnable( bool enable ) override;

    virtual void				fromGuiDebugSettings( uint32_t u32LogFlags, const char*	pLogFileName = nullptr ) override;
    virtual void				fromGuiSendLog( uint32_t u32LogFlags ) override;
    virtual bool				fromGuiBrowseFiles( std::string& folderName, uint8_t fileFilterMask = VXFILE_TYPE_ALLNOTEXE | VXFILE_TYPE_DIRECTORY ) override;
    virtual bool				fromGuiGetSharedFiles( uint8_t fileTypeFilter ) override;
    virtual bool				fromGuiSetFileIsShared( FileInfo& fileInfo, bool isShared ) override;
    virtual bool				fromGuiRemoveSharedFile( std::string& fileName ) override; // for remove before deletion
    virtual bool				fromGuiGetIsFileShared( std::string& fileName ) override;
	// returns -1 if unknown else percent downloaded
    virtual int					fromGuiGetFileDownloadState( uint8_t* fileHashId ) override;
    virtual bool				fromGuiSetFileIsInLibrary( FileInfo& fileInfo, bool isInLibrary ) override;
    virtual bool				fromGuiSetFileIsInLibrary( std::string& fileName, bool isInLibrary ) override;
    virtual bool				fromGuiRemoveFromLibrary( std::string& fileName ) override; // for remove before deletion
    virtual void				fromGuiGetFileLibraryList( uint8_t fileTypeFilter ) override;
    virtual bool				fromGuiGetIsFileInLibrary( std::string& fileName ) override;
    virtual bool				fromGuiIsNoLimitVideoFile( const char* fileName ) override;
    virtual bool				fromGuiIsNoLimitAudioFile( const char* fileName ) override;

    virtual int					fromGuiDeleteFile( std::string fileName, bool shredFile ) override;

    virtual void				fromGuiQuerySessionHistory( VxGUID& historyId, EPluginType pluginType = ePluginTypeInvalid ) override;
    virtual bool				fromGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID& onlineId, int pos0to100000, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
    virtual int					fromGuiGetJoinedListCount( EPluginType pluginType ) override;
    virtual void                fromGuiListAction( EListAction listAction ) override;
    virtual std::string			fromGuiQueryDefaultUrl( EHostType hostType ) override;
    virtual bool                fromGuiSetDefaultUrl( EHostType hostType, std::string& hostUrl ) override;
    virtual bool				fromGuiQueryIdentity( std::string& url, VxNetIdent& retNetIdent, bool requestIdentityIfUnknown ) override;
    virtual bool				fromGuiQueryIdentity( GroupieInfo& groupieInfo, VxNetIdent& retNetIdent, bool requestIdentityIfUnknown ) override;
    virtual bool				fromGuiQueryHosts( std::string& netHostUrl, EHostType hostType, std::vector<HostedInfo>& hostedInfoList, VxGUID& hostIdIfNullThenAll ) override;
    virtual bool				fromGuiQueryMyHostedInfo( EHostType hostType, std::vector<HostedInfo>& hostedInfoList ) override;
    virtual bool				fromGuiQueryHostListFromNetworkHost( VxPtopUrl& netHostUrl, EHostType hostType, VxGUID& hostIdIfNullThenAll ) override;
    virtual bool				fromGuiQueryGroupiesFromHosted( VxPtopUrl& hostedUrl, EHostType hostType, VxGUID& onlineIdIfNullThenAll ) override;

    virtual bool				fromGuiDownloadWebPage( EWebPageType webPageType, VxGUID& onlineId ) override;
    virtual bool				fromGuiCancelWebPage( EWebPageType webPageType, VxGUID& onlineId ) override;

    virtual bool				fromGuiDownloadFileList( EPluginType pluginType, VxGUID& onlineId, VxGUID& sessionId, uint8_t fileTypes = 0 ) override;
    virtual bool				fromGuiDownloadFileListCancel( EPluginType pluginType, VxGUID& onlineId, VxGUID& sessionId ) override;

    virtual EJoinState		    fromGuiQueryJoinState( EHostType hostType, VxNetIdent& netIdent ) override;

    virtual void				fromGuiUpdatePluginPermission( EPluginType pluginType, EFriendState pluginPermission ) override;

    virtual bool				fromGuiQueryFileHash( FileInfo& fileInfo ) override;
    virtual void				fromGuiFileHashGenerated( std::string& fileName, int64_t fileLen, VxSha1Hash& fileHash ) override;

    virtual void				fromGuiConnectToLastJoinedHost( std::string& lastConnectedHostInvite ) override;

	//========================================================================
	// to gui
	//========================================================================
    void						sendToGuiStatusMessage( const char* statusMsg, ... );
	void						toGuiPluginPermissionChange( PktAnnounce * poPktAnn );

	void						toGuiContactDescChange( PktAnnounce * poPktAnn );
	void						toGuiContactNameChange( PktAnnounce * poPktAnn );

	void						toGuiContactHisFriendshipChange( PktAnnounce * poPktAnn );
	void						toGuiContactSearchFlagsChange( PktAnnounce * poPktAnn );
	void						toGuiContactConnectionChange( PktAnnounce * poPktAnn );
	void						toGuiContactAnythingChange( PktAnnounce * poPktAnn );

	int 						toGuiSendAdministratorList( int iSentCnt, int iMaxSendCnt );
	int 						toGuiSendFriendList( int iSentCnt, int iMaxSendCnt );
	int 						toGuiSendGuestList( int iSentCnt, int iMaxSendCnt );
	int							toGuiSendAnonymousList( int iSentCnt, int iMaxSendCnt );
	int							toGuiSendIgnoreList( int iSentCnt, int iMaxSendCnt );

	//========================================================================
	// asset mgr callbacks
	//========================================================================
    virtual void				callbackFileWasShredded( std::string& fileName ) override;
    virtual void				callbackAssetAdded( AssetBaseInfo* assetInfo ) override;
    virtual void				callbackAssetRemoved( AssetBaseInfo* assetInfo ) override;

	virtual void				callbackSharedFileTypesChanged( uint16_t fileTypes );
	virtual void				callbackSharedPktFileListUpdated( void );

    virtual void				callbackAssetHistory( void * userData, AssetBaseInfo* assetInfo ) override;
    //========================================================================
    // host list mgr callbacks
    //========================================================================
    virtual void				callbackBlobAdded( BlobInfo* blobInfo ) override;
    virtual void				callbackBlobRemoved( BlobInfo* blobInfo ) override;
    virtual void				callbackBlobHistory( BlobInfo* blobInfo ) override;

	//========================================================================
	// media processor callbacks
	//========================================================================
    virtual void				callbackVideoJpgBig( void * userData, VxGUID& vidFeedId, uint8_t * jpgData, uint32_t jpgDataLen ) override;
    virtual void				callbackVideoJpgSmall(	void * userData, VxGUID& vidFeedId, uint8_t * jpgData, uint32_t jpgDataLen, int motion0to100000 ) override;

	//========================================================================
	//========================================================================
    void                        enableTimerThread( bool enable );
    void                        executeTimerThreadFunctions( void );

    void						fromGuiOncePerSecond( void ); // from qt gui thread triggers release of engine thread timed events.. allows better gui performance
	void						onOncePerSecond( void );
	void						onOncePer30Seconds( void );
	void						onOncePerMinute( void );
    void						onOncePer15Minutes( void );
    void						onOncePer30Minutes( void );
	void						onOncePerHour( void );

	void						onBigListInfoRestored( BigListInfo * poInfo ); 
	void						onBigListLoadComplete( RCODE rc );
	void						onBigListInfoDelete( BigListInfo * poInfo );

	virtual	void				onContactConnected		( RcConnectInfo * poInfo, bool connectionListLocked, bool newContact = false );
	virtual	void				onContactDisconnected	( RcConnectInfo * poInfo, bool connectionListLocked );

	void						onConnectionLost( std::shared_ptr<VxSktBase>& sktBase );
	void						onSessionStart( EPluginType pluginType, VxNetIdent* netIdent );
	//========================================================================
	//========================================================================

	void						handleTcpData( std::shared_ptr<VxSktBase>& sktBase );
	void						handleMulticastData( std::shared_ptr<VxSktBase>& sktBase );

	std::string					describeContact( BigListInfo * bigListInfo );
	std::string					describeContact( VxConnectInfo& connectInfo );
	std::string					describeContact( ConnectRequest& connectRequest );

	void						broadcastSystemPkt( VxPktHdr* pkt, bool onlyIncludeMyContacts );
	void						broadcastSystemPkt( VxPktHdr* pkt, VxGUIDList& retIdsSentPktTo );

	virtual bool				txSystemPkt(	VxNetIdentBase *	        netIdent,
												std::shared_ptr<VxSktBase>&	sktBase, 
												VxPktHdr*			        poPkt, 
												bool				        bDisconnectAfterSend = false );

    virtual bool				txSystemPkt(	const VxGUID&		        destOnlineId,
												std::shared_ptr<VxSktBase>&	sktBase, 
												VxPktHdr*			        poPkt, 
												bool				        bDisconnectAfterSend = false );

	virtual bool				txImAlivePkt(	VxGUID&				        destOnlineId, 
												std::shared_ptr<VxSktBase>&	sktBase );

	virtual void				replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt );

	bool						connectToContact(	VxConnectInfo&		        connectInfo, 
													std::shared_ptr<VxSktBase>&	ppoRetSkt,
													bool&				        retIsNewConnection,
													EConnectReason		        connectReason = eConnectReasonStayConnected );

	virtual void				doPktAnnHasChanged( bool connectionListIsLocked );

	bool						shouldInfoBeInDatabase( BigListInfo * poInfo );

    void                        sktMgrStatusCallback( std::string& statParam, int64_t statValue );

	//! called if hacker offense is detected
    void						hackerOffense(  EHackerLevel	hackerLevel,
                                                EHackerReason   hackerReason,
                                                InetAddress		ipAddr,					// ip address 
                                                VxGUID          signatureGuid,			// users identity info ( may be null if not known then use ipAddress )
                                                const char*     pMsg, ... );			// message about the offense

	void						hackerOffense(	EHackerLevel	hackerLevel,			    
                                                EHackerReason   hackerReason,
                                                VxNetIdent*	    poContactIdent,			// users identity info ( may be null if not known then use ipAddress )
												InetAddress		IpAddr,					// ip address if identity not known
												const char*	    pMsg, ... );			// message about the offense

    void                        hackerOffense(  EHackerLevel	hackerLevel,			    
                                                EHackerReason   hackerReason,
                                                VxPktHdr*	    pktHdr,			// users identity info ( may be null if not known then use ipAddress )
                                                std::shared_ptr<VxSktBase>&      sktBase,
                                                const char*	    pMsg, ... );			// message about the offense

	//========================================================================
	// pkt handlers
	//========================================================================

    //=== packet handlers ===//
    virtual void                handlePkt                   ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktUnhandled              ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktInvalid				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktAnnounce				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAnnList				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktScanReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktScanReply			    ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktPluginOfferReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktPluginOfferReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktChatReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktChatReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktVoiceReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktVoiceReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktVideoFeedReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktVideoFeedStatus		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktVideoFeedPic			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktVideoFeedPicChunk		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktVideoFeedPicAck		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktFileGetReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileGetReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileSendReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileSendReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFindFileReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFindFileReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileListReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileListReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktFileInfoReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktFileChunkReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileChunkReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileSendCompleteReq	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileSendCompleteReply	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileGetCompleteReq		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileGetCompleteReply	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileShareErr			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktAssetGetReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetGetReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetSendReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetSendReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetChunkReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetChunkReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetGetCompleteReq	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetGetCompleteReply	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetSendCompleteReq	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetSendCompleteReply	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktAssetXferErr			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktMultiSessionReq		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktMultiSessionReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktSessionStartReq		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktSessionStartReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktSessionStopReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktSessionStopReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktMyPicSendReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktMyPicSendReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktWebServerPicChunkTx	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktWebServerPicChunkAck	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktWebServerGetChunkTx	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktWebServerGetChunkAck	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktWebServerPutChunkTx	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktWebServerPutChunkAck	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktTodGameStats			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktTodGameAction			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktTodGameValue			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktTcpPunch				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktPingReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktPingReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktImAliveReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktImAliveReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktBlobSendReq            ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktBlobSendReply          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktBlobChunkReq           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktBlobChunkReply         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktBlobSendCompleteReq    ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktBlobSendCompleteReply  ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktBlobXferErr            ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktHostJoinReq            ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostJoinReply          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostLeaveReq           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostLeaveReply         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktHostUnJoinReq          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostUnJoinReply        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostSearchReq          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostSearchReply        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostOfferReq           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostOfferReply         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFriendOfferReq         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFriendOfferReply       ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktThumbGetReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbGetReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbSendReq           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbSendReply         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbChunkReq          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbChunkReply        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbGetCompleteReq	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbGetCompleteReply	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbSendCompleteReq   ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbSendCompleteReply ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktThumbXferErr           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktOfferSendReq           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktOfferSendReply         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktOfferChunkReq          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktOfferChunkReply        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktOfferSendCompleteReq   ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktOfferSendCompleteReply ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktOfferXferErr           ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktPushToTalkReq          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktPushToTalkReply        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktPushToTalkStart        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktPushToTalkStop         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktMembershipReq          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktMembershipReply        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktHostInfoReq            ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostInfoReply          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktHostInviteAnnReq       ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostInviteAnnReply     ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostInviteSearchReq    ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostInviteSearchReply  ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostInviteMoreReq      ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostInviteMoreReply    ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktGroupieInfoReq         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktGroupieInfoReply       ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktGroupieAnnReq          ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktGroupieAnnReply        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktGroupieSearchReq       ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktGroupieSearchReply     ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktGroupieMoreReq         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktGroupieMoreReply       ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktFileInfoInfoReq        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileInfoInfoReply      ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktFileInfoAnnReq         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileInfoAnnReply       ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileInfoSearchReq      ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileInfoSearchReply    ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileInfoMoreReq        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktFileInfoMoreReply      ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktRelayUserDisconnect    ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktHostUserInfoReq        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostUserInfoReply      ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostUserStatusReq      ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostUserStatusReply    ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    virtual void				onPktHostUserListReq        ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostUserListReply      ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostUserListMoreReq    ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    virtual void				onPktHostUserListMoreReply  ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
    
	virtual void				onPktTestConnTestReq		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
	virtual void				onPktTestConnTestReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
	virtual void				onPktTestConnPingReq		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
	virtual void				onPktTestConnPingReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

	virtual void				onPktQueryHostUrlReq		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;
	virtual void				onPktQueryHostUrlReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr ) override;

    bool                        validateIdent( VxNetIdent* netIdent ); // extra validatation for at risk connections like multicast

    void                        executeAfterLogOnThreadFunctions( void );

    void                        onNetworkConnectionReady( bool requiresRelay, std::string& ipAddr, uint16_t ipPort );

    /// extract online id from either url if url is valid
    static VxGUID               getOnlineIdFromUrl( std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 );

protected:
    //========================================================================
    //========================================================================
    void						iniitializePtoPEngine( void );

	virtual bool				txPluginPkt( 	EPluginType			pluginType, 
												VxNetIdentBase *	netIdent, 
												std::shared_ptr<VxSktBase>&			sktBase, 
												VxPktHdr*			poPkt, 
												bool				bDisconnectAfterSend );

	virtual void				doAppStateChange( EAppState eAppState );
	virtual bool				shouldNotifyGui( VxNetIdent* netIdent );

	// pkt ann has changed and needs to be re announced
	void						doPktAnnConnectionInfoChanged( bool connectionListIsLocked );
	virtual	void				attemptConnectionToRelayService( BigListInfo * poInfo );
	void						handleIncommingRelayData( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr );
	void						sendToGuiTheContactList( int maxContactsToSend );
    void                        updateIdentLists( BigListInfo* bigListInfo, int64_t timestampMs = 0 );
    void                        updateOnFirstConnect( std::shared_ptr<VxSktBase>& sktBase, BigListInfo* bigListInfo, bool nearbyLanConnected );
    void						onFirstPktAnnounce( PktAnnounce* pktAnn, std::shared_ptr<VxSktBase>& sktBase, BigListInfo* bigListInfo );

    EMembershipState            getMembershipState( PktAnnounce& myPktAnn, VxNetIdent* netIdent, EPluginType pluginType, EFriendState myFriendshipToHim );

    void                        enableAfterLogOnThread( bool enable );

	//=== vars ===//
	VxPeerMgr&					m_PeerMgr;
    ConnectIdListMgr            m_ConnectIdListMgr;
    IgnoreListMgr               m_IgnoreListMgr;
    FriendListMgr               m_FriendListMgr;
    GroupieListMgr              m_GroupieListMgr;
    HostUrlListMgr              m_HostUrlListMgr;
    HostedListMgr               m_HostedListMgr;
    NearbyListMgr               m_NearbyListMgr;
    BigListMgr					m_BigListMgr;
	PktAnnounce					m_PktAnn;
    int64_t                     m_PktAnnLastModTime{ 0 };
    VxGUID                      m_MyOnlineId;
	VxMutex						m_AnnouncePktMutex;
	EngineSettings				m_EngineSettings;
	EngineParams				m_EngineParams;
    NetStatusAccum              m_NetStatusAccum;
	AssetMgr&					m_AssetMgr;
    BlobMgr&				    m_BlobMgr;
    OfferMgr&				    m_OfferMgr;
    ThumbMgr&					m_ThumbMgr;
    ConnectionMgr&              m_ConnectionMgr;
    ConnectMgr&                 m_ConnectMgr;
	P2PConnectList				m_ConnectionList;
    MediaProcessor&				m_MediaProcessor;
    MembershipAvailableMgr      m_MembershipAvailableMgr;
    MembershipHostedMgr         m_MembershipHostedMgr;
    NetworkMgr&					m_NetworkMgr;
	NetworkMonitor&				m_NetworkMonitor;
	NetServicesMgr&				m_NetServicesMgr;
	NetConnector&				m_NetConnector;
	NetworkStateMachine&		m_NetworkStateMachine;

	PluginMgr&					m_PluginMgr;
    PluginSettingMgr			m_PluginSettingMgr;

	PluginFileShareServer*	    m_PluginFileShareServer;
    PluginLibraryServer*        m_PluginLibraryServer;
	PluginNetServices*			m_PluginNetServices;

	IsPortOpenTest&				m_IsPortOpenTest;
    RelayMgr                    m_RelayMgr;
    RunUrlAction&			    m_RunUrlAction;
    HostJoinMgr&				m_HostJoinMgr;
    UserJoinMgr&				m_UserJoinMgr;
    UserOnlineMgr&				m_UserOnlineMgr;
    WebPageMgr&                 m_WebPageMgr;
    std::shared_ptr<VxSktBase>  m_SktLoopback;

	RcScan						m_RcScan;

    EAppState					m_eAppState{ eAppStateInvalid };

    EFriendViewType				m_eFriendView{ eFriendViewEverybody };
	unsigned int				m_iCurPreferredRelayConnectIdx{ 0 };
	VxGUID						m_NextFileInstance;
    bool						m_AppStartupCalled{ false };
	bool						m_AppIsPaused{ false };
	bool						m_IsUserSpecificDirSet{ false };
    bool                        m_EngineInitialized{ false };
    bool                        m_IsEngineCreated{ false };
    bool                        m_IsEngineReady{ false };
    bool                        m_NetworkConnectionReady{ false };

	PktImAliveReq				m_PktImAliveReq;

    VxSemaphore                 m_TimerThreadSemaphore;
    VxThread                    m_TimerThread;
    VxThread                    m_AfterLogOnThread;
};

extern P2PEngine& GetPtoPEngine();
