//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "INlc.h"

#include "ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h"
#include <BigListLib/BigListInfo.h>

#include <CoreLib/VxGlobals.h>

//============================================================================
//=== from gui ===//
//============================================================================
void INlc::fromGuiSetupContext( void )
{
    //LogMsg( LOG_DEBUG, "INlc::fromGuiSetupContext\n");
    //if( 0 == m_AudioMgr )
    //{
    //    //__android_log_write( ANDROID_LOG_INFO, ENGINE_LOG_TAG, "fromGuiSetupContext create Audio Manager" );
    //    m_AudioMgr = new webrtc::AudioManager( *this );
    //    //__android_log_write( ANDROID_LOG_INFO, ENGINE_LOG_TAG, "fromGuiSetupContext initAndroidAudioMgr" );
    //    m_AudioMgr->initAndroidAudioMgr();
    //    //__android_log_write( ANDROID_LOG_INFO, ENGINE_LOG_TAG, "fromGuiSetupContext create Audio Manager done" );
    //}
}

//============================================================================
void INlc::fromGuiSetIsAppCommercial( bool isCommercial )
{
    VxSetIsApplicationCommercial( isCommercial );
}

//============================================================================
bool INlc::fromGuiGetIsAppCommercial( void )
{
    return VxGetIsApplicationCommercial();
}

//============================================================================
uint16_t INlc::fromGuiGetAppVersionBinary( void )
{
    return VxGetAppVersion();
}

//============================================================================
const char* INlc::fromGuiGetAppVersionString( void )
{
    return VxGetAppVersionString();
}

//============================================================================
const char* INlc::fromGuiGetAppName( void )
{
    return VxGetApplicationTitle();
}

//============================================================================
const char* INlc::fromGuiGetAppNameNoSpaces( void )
{
    return VxGetApplicationNameNoSpaces();
}

//============================================================================
void INlc::fromGuiAppStartup( std::string assetDir, std::string rootDataDir, bool fromThread )
{
    //LogMsg( LOG_DEBUG, "INlc::fromGuiAppStartup.. calling fromGuiSetupContext\n");
    //if( 0 == m_AudioMgr )
    //{
    //    fromGuiSetupContext();
    //}

    getPtoP().fromGuiAppStartup( assetDir, rootDataDir );

    //if( 0 != m_AudioMgr )
    //{
    //    m_AudioMgr->toGuiWantMicrophoneRecording( true );
    //}

    //if( 0 != m_AudioMgr )
    //{
    //    m_AudioMgr->toGuiWantSpeakerOutput( true );
    //}
}

//============================================================================
void INlc::fromGuiSetUserXferDir( std::string userDir, bool fromThread )
{
    //LogMsg( LOG_DEBUG, "INlc::fromGuiSetUserXferDir %s\n", userDir );
    getPtoP().fromGuiSetUserXferDir( userDir, fromThread );
}

//============================================================================
void INlc::fromGuiSetUserSpecificDir( std::string userDir, bool fromThread )
{
    getPtoP().fromGuiSetUserSpecificDir( userDir, fromThread );
    // needs called after user specific data folder is set
    getOsInterface().initDirectories();
}

//============================================================================
void INlc::fromGuiUserLoggedOn( VxNetIdent* netIdent, bool fromThread )
{
    getPtoP().fromGuiUserLoggedOn( netIdent, fromThread );
}

//============================================================================
bool INlc::fromGuiDeleteUser( VxGUID& onlineId )
{
    return getPtoP().fromGuiDeleteUser( onlineId );
}

//============================================================================
uint64_t INlc::fromGuiGetDiskFreeSpace( const char* dir )
{
    return getPtoP().fromGuiGetDiskFreeSpace( dir );
}

//============================================================================
uint64_t INlc::fromGuiClearCache( ECacheType cacheType )
{
    return getPtoP().fromGuiClearCache( cacheType );
}

//============================================================================
void INlc::fromGuiAppShutdown( void )
{
    LogMsg( LOG_INFO, "fromGuiAppShutdown" );
    VxSetAppIsShuttingDown( true );

    getPtoP().fromGuiAppShutdown();
}

//============================================================================
void INlc::fromGuiAppPauseOrResume( bool isPaused )
{
    if( isPaused )
    {
        getPtoP().fromGuiAppPause();
    }
    else
    {
        getPtoP().fromGuiAppResume();
    }
}

//============================================================================
void INlc::fromGuiOnlineNameChanged( const char* newOnlineName )
{
    getPtoP().fromGuiOnlineNameChanged( newOnlineName );
}

//============================================================================
void INlc::fromGuiMoodMessageChanged( const char* newMoodMessage )
{
    getPtoP().fromGuiMoodMessageChanged( newMoodMessage );
}

//============================================================================
void INlc::fromGuiIdentPersonalInfoChanged( int age, int gender, int language, int preferredContent )
{
    getPtoP().fromGuiIdentPersonalInfoChanged( age, gender, language, preferredContent );
}

//============================================================================
void INlc::fromGuiSetUserHasProfilePicture( bool haveProfilePick )
{
    getPtoP().fromGuiSetUserHasProfilePicture( haveProfilePick );
}

//============================================================================
bool INlc::fromGuiOrientationEvent( float f32RotX, float f32RotY, float f32RotZ )
{
    return getPtoP().fromGuiOrientationEvent( f32RotX, f32RotY, f32RotZ );
}

//============================================================================
bool INlc::fromGuiMouseEvent( EMouseButtonType eMouseButType, EMouseEventType eMouseEventType, int iMouseXPos, int iMouseYPos )
{
    return getPtoP().fromGuiMouseEvent( eMouseButType, eMouseEventType, iMouseXPos, iMouseYPos );
}

//============================================================================
bool INlc::fromGuiMouseWheel( float f32MouseWheelDist )
{
    return getPtoP().fromGuiMouseWheel( f32MouseWheelDist );
}

//============================================================================
bool INlc::fromGuiKeyEvent( EKeyEventType eKeyEventType, EKeyCode eKey, int iFlags )
{
    return getPtoP().fromGuiKeyEvent( eKeyEventType, eKey, iFlags );
}

//============================================================================
void INlc::fromGuiNativeGlInit( void )
{
    getPtoP().fromGuiNativeGlInit();
}

//============================================================================
void INlc::fromGuiNativeGlResize( int width, int height )
{
    getPtoP().fromGuiNativeGlResize( width, height );
}

//============================================================================
int INlc::fromGuiNativeGlRender( void )
{
    return getPtoP().fromGuiNativeGlRender();
}

//============================================================================
void INlc::fromGuiNativeGlPauseRender( void )
{
    getPtoP().fromGuiNativeGlPauseRender();
}

//============================================================================
void INlc::fromGuiNativeGlResumeRender( void )
{
    getPtoP().fromGuiNativeGlResumeRender();
}

//============================================================================
void INlc::fromGuiNativeGlDestroy( void )
{
    getPtoP().fromGuiNativeGlDestroy();
}

//============================================================================
void INlc::fromGuiNeedMorePlayData( int16_t * retAudioSamples, int deviceReqDataLen )
{
//    getPtoP().fromGuiNeedMorePlayData( retAudioSamples, deviceReqDataLen );
}

//============================================================================
void INlc::fromGuiMuteMicrophone( bool mute )
{
    getPtoP().fromGuiMuteMicrophone( mute );
}

//============================================================================
void INlc::fromGuiMuteSpeaker( bool mute )
{
    getPtoP().fromGuiMuteSpeaker( mute );
}

//============================================================================
bool INlc::fromGuiIsMicrophoneMuted( void )
{
    return getPtoP().fromGuiIsMicrophoneMuted();
}

//============================================================================
bool INlc::fromGuiIsSpeakerMuted( void )
{
    return getPtoP().fromGuiIsSpeakerMuted();
}

//============================================================================
void INlc::fromGuiWantMediaInput( VxGUID& id, EMediaInputType eMediaType, EAppModule appModule, bool wantInput )
{
    getPtoP().fromGuiWantMediaInput( id, eMediaType, appModule, wantInput );
}

//============================================================================
void INlc::fromGuiVideoData( uint32_t u32FourCc, uint8_t * pu8VidDataIn, int iWidth, int iHeight, uint32_t u32VidDataLen, int iRotation )
{
    getPtoP().fromGuiVideoData( u32FourCc, pu8VidDataIn, iWidth, iHeight, u32VidDataLen, iRotation );
}

//============================================================================
bool INlc::fromGuiMovieDone( void )
{
    return getPtoP().fromGuiMovieDone();
}

//============================================================================
void INlc::fromGuiApplyNetHostSettings( NetHostSetting& netSettings )
{
    getPtoP().fromGuiApplyNetHostSettings( netSettings );
}

//============================================================================
void INlc::fromGuiGetNetSettings( NetSettings& netSettings )
{
    getPtoP().fromGuiGetNetSettings( netSettings );
}

//============================================================================
void INlc::fromGuiSetNetSettings( NetSettings& netSettings )
{
    getPtoP().fromGuiSetNetSettings( netSettings );
}

//============================================================================
void INlc::fromGuiNetworkSettingsChanged( void )
{
    getPtoP().fromGuiNetworkSettingsChanged();
}

//============================================================================
void INlc::fromGuiNetworkAvailable( const char* lclIp, bool isCellularNetwork )
{
    getPtoP().fromGuiNetworkAvailable( lclIp, isCellularNetwork );
}

//============================================================================
void INlc::fromGuiNetworkLost( void )
{
    getPtoP().fromGuiNetworkLost();
}

//============================================================================
ENetLayerState INlc::fromGuiGetNetLayerState( ENetLayerType netLayer )
{
    return getPtoP().fromGuiGetNetLayerState( netLayer );
}

//============================================================================
void INlc::fromGuiRunIsPortOpenTest( uint16_t port )
{
    getPtoP().fromGuiRunIsPortOpenTest( port );
}

//============================================================================
void INlc::fromGuiAnnounceHost( HostedId& adminId, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6, bool fromThread )
{
    getPtoP().fromGuiAnnounceHost( adminId, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
}

//============================================================================
void INlc::fromGuiJoinHost( HostedId& adminId, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6, bool fromThread )
{
    getPtoP().fromGuiJoinHost( adminId, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
}

//============================================================================
void INlc::fromGuiLeaveHost( HostedId& adminId, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6, bool fromThread )
{
    getPtoP().fromGuiLeaveHost( adminId, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
}

//============================================================================
void INlc::fromGuiUnJoinHost( HostedId& adminId, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6, bool fromThread )
{
    getPtoP().fromGuiUnJoinHost( adminId, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
}

//============================================================================
void INlc::fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable, bool fromThread )
{
    getPtoP().fromGuiSearchHost( hostType, searchParams, enable );
}

//============================================================================
void INlc::fromGuiSendAnnouncedList( EHostType hostType, VxGUID& sessionId )
{
    getPtoP().fromGuiSendAnnouncedList( hostType, sessionId );
}

//============================================================================
void INlc::fromGuiDisconnectFromUser( VxGUID& onlineId )
{
     getPtoP().fromGuiDisconnectFromUser( onlineId );
}

//============================================================================
void INlc::fromGuiRunUrlAction( VxGUID& sessionId, const char* myUrl, const char* ptopUrl, ENetCmdType testType )
{
    getPtoP().fromGuiRunUrlAction( sessionId, myUrl, ptopUrl, testType );
}

//============================================================================
void INlc::fromGuiGetFileShareSettings( FileShareSettings& fileShareSettings )
{
    getPtoP().fromGuiGetFileShareSettings( fileShareSettings );
}

//============================================================================
void INlc::fromGuiSetFileShareSettings( FileShareSettings& fileShareSettings )
{
    getPtoP().fromGuiSetFileShareSettings( fileShareSettings );
}

//============================================================================
void INlc::fromGuiUpdateWebPageProfile( const char*	pProfileDir,	// directory containing user profile
                                             const char*	strGreeting,	// greeting text
                                             const char*	aboutMe,		// about me text
                                             const char*	url1,			// favorite url 1
                                             const char*	url2,			// favorite url 2
                                             const char*	url3,
                                             const char*	donation )
{
    getPtoP().fromGuiUpdateWebPageProfile( pProfileDir,	// directory containing user profile
                                          strGreeting,	// greeting text
                                          aboutMe,		// about me text
                                          url1,			// favorite url 1
                                          url2,			// favorite url 2
                                          url3,
                                          donation );
}

//============================================================================
void INlc::fromGuiSetPluginPermission( EPluginType pluginType, int eFriendState )
{
    getPtoP().fromGuiSetPluginPermission( pluginType, ( EFriendState )eFriendState );
}

//============================================================================
int INlc::fromGuiGetPluginPermission( EPluginType pluginType )
{
    return ( int )getPtoP().fromGuiGetPluginPermission( pluginType );
}

//============================================================================
int INlc::fromGuiGetPluginServerState( EPluginType pluginType )
{
    return getPtoP().fromGuiGetPluginServerState( pluginType );
}

//============================================================================
void INlc::fromGuiStartPluginSession( EPluginType pluginType, VxGUID oOnlineId, int pvUserData, VxGUID lclSessionId )
{
    getPtoP().fromGuiStartPluginSession( pluginType, oOnlineId, pvUserData, lclSessionId );
}

//============================================================================
void INlc::fromGuiStopPluginSession( EPluginType pluginType, VxGUID onlineId, int pvUserData, VxGUID lclSessionId )
{
    getPtoP().fromGuiStopPluginSession( pluginType, onlineId, pvUserData, lclSessionId );
}

//============================================================================
bool INlc::fromGuiIsPluginInSession( EPluginType pluginType, VxGUID onlineId, int pvUserData, VxGUID lclSessionId )
{
    VxNetIdent* netIdent = getPtoP().getBigListMgr().findNetIdent( onlineId );
    if( netIdent )
    {
        return getPtoP().fromGuiIsPluginInSession( pluginType, onlineId, pvUserData, lclSessionId );
    }

    return false;
}

//============================================================================
bool INlc::fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
    return getPtoP().fromGuiMakePluginOffer( onlineId, offerInfo );
}

//============================================================================
bool INlc::fromGuiToPluginOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
    return getPtoP().fromGuiToPluginOfferReply( onlineId, offerInfo );
}

//============================================================================
EXferError INlc::fromGuiFileXferControl( EPluginType pluginType, EXferAction xferAction, FileInfo& fileInfo )
{
    return getPtoP().fromGuiFileXferControl( pluginType, xferAction, fileInfo );
}

//============================================================================
bool INlc::fromGuiInstMsg( EPluginType pluginType, VxGUID& oOnlineId, const char* pMsg )                           
{
    return getPtoP().fromGuiInstMsg( pluginType, oOnlineId, pMsg );
}

//============================================================================
bool INlc::fromGuiPushToTalk( VxGUID& onlineId, bool enableTalk )
{
    return getPtoP().fromGuiPushToTalk( onlineId, enableTalk );
}

//============================================================================
bool INlc::fromGuiChangeMyFriendshipToHim( VxGUID& oOnlineId, EFriendState eMyFriendshipToHim, EFriendState	eHisFriendshipToMe )                                           
{
    return getPtoP().fromGuiChangeMyFriendshipToHim( oOnlineId, eMyFriendshipToHim, eHisFriendshipToMe );
}

//============================================================================
void INlc::fromGuiSendContactList( EFriendViewType eFriendView, int maxContactsToSend )
{
    getPtoP().fromGuiSendContactList( eFriendView, maxContactsToSend );
}

//============================================================================
void INlc::fromGuiRefreshContactList( int maxContactsToSend )
{
    getPtoP().fromGuiRefreshContactList( maxContactsToSend );
}

//============================================================================
void INlc::fromGuiRequireRelay( bool bRequireRelay )
{
    getPtoP().fromGuiRequireRelay( bRequireRelay );
}

//============================================================================
void INlc::fromGuiRelayPermissionCount( int userPermittedCount, int anonymousCount )
{
    getPtoP().fromGuiRelayPermissionCount( userPermittedCount, anonymousCount );
}

//============================================================================
void INlc::fromGuiStartScan( EScanType eScanType, uint8_t searchFlags, uint8_t fileTypeFlags, const char* pSearchPattern )
{
    getPtoP().fromGuiStartScan( eScanType, searchFlags, fileTypeFlags, pSearchPattern );
}

//============================================================================
void INlc::fromGuiNextScan( EScanType eScanType )
{
    getPtoP().fromGuiNextScan( eScanType );
}

//============================================================================
void INlc::fromGuiStopScan( EScanType eScanType )
{
    getPtoP().fromGuiStopScan( eScanType );
}

//============================================================================
InetAddress INlc::fromGuiGetMyIPv4Address( void )
{
    return getPtoP().fromGuiGetMyIPv4Address();
}

//============================================================================
InetAddress INlc::fromGuiGetMyIPv6Address( void )
{
    return getPtoP().fromGuiGetMyIPv6Address();
}

//============================================================================
void INlc::fromGuiUpdateMyIdent( VxNetIdent* netIdent, bool permissionAndStatsOnly )
{
    getPtoP().fromGuiUpdateMyIdent( netIdent, permissionAndStatsOnly );
}

//============================================================================
void INlc::fromGuiQueryMyIdent( VxNetIdent* poRetIdent )
{
    getPtoP().fromGuiQueryMyIdent( poRetIdent );
}

//============================================================================
void INlc::fromGuiSetIdentHasTextOffers( VxGUID& onlineId, bool hasTextOffers )
{
    getPtoP().fromGuiSetIdentHasTextOffers( onlineId, hasTextOffers );
}

//============================================================================
void INlc::fromGuiUserModifiedStoryboard( void )
{
    getPtoP().fromGuiUserModifiedStoryboard();
}

//============================================================================
void INlc::fromGuiCancelDownload( VxGUID& fileInstanceId )
{
    getPtoP().fromGuiCancelDownload( fileInstanceId );
}

//============================================================================
void INlc::fromGuiCancelUpload( VxGUID& fileInstanceId )
{
    getPtoP().fromGuiCancelUpload( fileInstanceId );
}

//============================================================================
bool INlc::fromGuiSetGameValueVar( EPluginType	pluginType,
                                        VxGUID&	oOnlineId,
                                        int32_t			s32VarId,
                                        int32_t			s32VarValue )
{
    return getPtoP().fromGuiSetGameValueVar( pluginType,
                                            oOnlineId,
                                            s32VarId,
                                            s32VarValue );
}

//============================================================================
bool INlc::fromGuiSetGameActionVar( EPluginType	pluginType,
                                         VxGUID&	oOnlineId,
                                         int32_t			s32VarId,
                                         int32_t			s32VarValue )
{
    return getPtoP().fromGuiSetGameActionVar( pluginType,
                                             oOnlineId,
                                             s32VarId,
                                             s32VarValue );
}

//============================================================================
bool INlc::fromGuiTestCmd( IFromGui::ETestParam1		eTestParam1,
                                int							testParam2,
                                const char*				testParam3 )
{
    return getPtoP().fromGuiTestCmd( eTestParam1, testParam2, testParam3 );
}

#if ENABLE_COMPONENT_NEARBY
//============================================================================
bool INlc::fromGuiNearbyBroadcastEnable( bool enable )
{
    return getPtoP().fromGuiNearbyBroadcastEnable( enable );
}
#endif // ENABLE_COMPONENT_NEARBY

//============================================================================
void INlc::fromGuiDebugSettings( uint32_t u32LogFlags, const char*	pLogFileName )
{
    getPtoP().fromGuiDebugSettings( u32LogFlags, pLogFileName );
}

//============================================================================
void INlc::fromGuiSendLog( uint32_t u32LogFlags )
{
    getPtoP().fromGuiSendLog( u32LogFlags );
}

//============================================================================
bool INlc::fromGuiBrowseFiles( std::string& folderName, uint8_t fileFilterMask )
{
    return getPtoP().fromGuiBrowseFiles( folderName, fileFilterMask );
}

//============================================================================
bool INlc::fromGuiGetSharedFiles( uint8_t fileTypeFilter )
{
    return getPtoP().fromGuiGetSharedFiles( fileTypeFilter );
}

//============================================================================
bool INlc::fromGuiSetFileIsShared( FileInfo& fileInfo, bool isShared )
{
    return getPtoP().fromGuiSetFileIsShared( fileInfo, isShared );
}

//============================================================================
bool INlc::fromGuiGetIsFileShared( FileInfo& fileInfo )
{
    return getPtoP().fromGuiGetIsFileShared( fileInfo );
}

//============================================================================
bool INlc::fromGuiRemoveSharedFile( FileInfo& fileInfo )
{
    return getPtoP().fromGuiRemoveSharedFile( fileInfo );
}

//============================================================================
int INlc::fromGuiGetFileDownloadState( uint8_t* fileHashId )
{
    return getPtoP().fromGuiGetFileDownloadState( fileHashId );
}

//============================================================================
bool INlc::fromGuiSetFileIsInLibrary( FileInfo& fileInfo, bool inLibrary )
{
    return getPtoP().fromGuiSetFileIsInLibrary( fileInfo, inLibrary );
}

//============================================================================
bool INlc::fromGuiGetFileIsInLibrary( FileInfo& fileInfo )
{
    return getPtoP().fromGuiGetFileIsInLibrary( fileInfo );
}

//============================================================================
bool INlc::fromGuiSetFileIsInLibrary( std::string fileName, bool inLibrary )
{
    return getPtoP().fromGuiSetFileIsInLibrary( fileName, inLibrary );
}

//============================================================================
void INlc::fromGuiGetFileLibraryList( uint8_t fileTypeFilter )
{
    getPtoP().fromGuiGetFileLibraryList( fileTypeFilter );
}

//============================================================================
bool INlc::fromGuiGetIsFileInLibrary( std::string& fileName )
{
    return getPtoP().fromGuiGetIsFileInLibrary( fileName );
}

//============================================================================
bool INlc::fromGuiRemoveFromLibrary( std::string& fileName )
{
    return getPtoP().fromGuiRemoveFromLibrary( fileName );
}

//============================================================================
bool INlc::fromGuiIsNoLimitVideoFile( const char* fileName )
{
    return getPtoP().fromGuiIsNoLimitVideoFile( fileName );
}

//============================================================================
bool INlc::fromGuiIsNoLimitAudioFile( const char* fileName )
{
    return getPtoP().fromGuiIsNoLimitAudioFile( fileName );
}

//============================================================================
int INlc::fromGuiDeleteFile( std::string fileName, bool shredFile )
{
    return getPtoP().fromGuiDeleteFile( fileName, shredFile );
}

//============================================================================
bool INlc::fromGuiAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo, int pos0to100000 )
{
    return getPtoP().fromGuiAssetAction( assetAction, assetInfo, pos0to100000 );
}

//============================================================================
bool INlc::fromGuiQueueAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo, int pos0to100000 )
{
    return getPtoP().fromGuiQueueAssetAction( assetAction, assetInfo, pos0to100000 );
}

//============================================================================
bool INlc::fromGuiAssetAction( EPluginType pluginType, EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
    return getPtoP().fromGuiAssetAction( pluginType, assetAction, assetId, pos0to100000 );
}

//============================================================================
bool INlc::fromGuiSendAsset( AssetBaseInfo& assetInfo )
{
    return getPtoP().fromGuiSendAsset( assetInfo );
}

//============================================================================
bool INlc::fromGuiVideoRecord( EVideoRecordState eRecState, VxGUID& feedId, const char* fileName )
{
    return getPtoP().fromGuiVideoRecord( eRecState, feedId, fileName );
}

//============================================================================
bool INlc::fromGuiSndRecord( ESndRecordState eRecState, VxGUID& feedId, const char* fileName )
{
    return getPtoP().fromGuiSndRecord( eRecState, feedId, fileName );
}

//============================================================================
void INlc::fromGuiQuerySessionHistory( GroupieId& groupieId )
{
    getPtoP().fromGuiQuerySessionHistory( groupieId );
}

//============================================================================
bool INlc::fromGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID& onlineId, int pos0to100000 )
{
    return getPtoP().fromGuiMultiSessionAction( mSessionAction, onlineId, pos0to100000 );
}

//============================================================================
int INlc::fromGuiGetJoinedListCount( EPluginType pluginType )
{
    return getPtoP().fromGuiGetJoinedListCount( pluginType );
}

//============================================================================
void INlc::fromGuiListAction( EListAction listAction )
{
    getPtoP().fromGuiListAction( listAction );
}

//============================================================================
std::string INlc::fromGuiQueryDefaultUrl( EHostType hostType )
{
    return getPtoP().fromGuiQueryDefaultUrl( hostType );
}

//============================================================================
bool INlc::fromGuiSetDefaultUrl( EHostType hostType, std::string& hostUrl )
{
    return getPtoP().fromGuiSetDefaultUrl( hostType, hostUrl );
}

//============================================================================
bool INlc::fromGuiQueryIdentity( std::string& url, VxNetIdent& retNetIdent, bool requestIdentityIfUnknown )
{
    return getPtoP().fromGuiQueryIdentity( url, retNetIdent, requestIdentityIfUnknown );
}

//============================================================================
bool INlc::fromGuiQueryIdentity( VxGUID onlineId, VxNetIdent& retNetIdent )
{
    return getPtoP().fromGuiQueryIdentity( onlineId, retNetIdent );
}

//============================================================================
bool INlc::fromGuiQueryHosts( std::string& netHostUrl, EHostType hostType, std::vector<HostedInfo>& hostedInfoList, VxGUID& hostIdIfNullThenAll )
{
    return getPtoP().fromGuiQueryHosts( netHostUrl, hostType, hostedInfoList, hostIdIfNullThenAll );
}

//============================================================================
bool INlc::fromGuiQueryMyHostedInfo( EHostType hostType, std::vector<HostedInfo>& hostedInfoList )
{
    return getPtoP().fromGuiQueryMyHostedInfo( hostType, hostedInfoList );
}

//============================================================================
bool INlc::fromGuiQueryHostListFromNetworkHost( VxPtopUrl& netHostUrl, EHostType hostType, VxGUID& hostIdIfNullThenAll )
{
    return getPtoP().fromGuiQueryHostListFromNetworkHost( netHostUrl, hostType, hostIdIfNullThenAll );
}

//============================================================================
bool INlc::fromGuiQueryGroupiesFromHosted( VxPtopUrl& hostedUrl, EHostType hostType, VxGUID& onlineIdIfNullThenAll )
{
    return getPtoP().fromGuiQueryGroupiesFromHosted( hostedUrl, hostType, onlineIdIfNullThenAll );
}

//============================================================================
bool INlc::fromGuiDownloadWebPage( EWebPageType webPageType, VxGUID& onlineId )
{
    return getPtoP().fromGuiDownloadWebPage( webPageType, onlineId );
}

//============================================================================
bool INlc::fromGuiCancelWebPage( EWebPageType webPageType, VxGUID& onlineId )
{
    return getPtoP().fromGuiCancelWebPage( webPageType, onlineId );
}

//============================================================================
bool INlc::fromGuiDownloadFileList( EPluginType pluginType, VxGUID& onlineId, VxGUID& sessionId, uint8_t fileTypes )
{
    return getPtoP().fromGuiDownloadFileList( pluginType, onlineId, sessionId, fileTypes );
}

//============================================================================
bool INlc::fromGuiDownloadFileListCancel( EPluginType pluginType, VxGUID& onlineId, VxGUID& sessionId )
{
    return getPtoP().fromGuiDownloadFileListCancel( pluginType, onlineId, sessionId );
}
