//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "../CommonSrc/QtSource/AppCommon.h"
#include "INlc.h" // should be first but is included after AppCommon so QWidget is defined first for linux build

#include "../CommonSrc/QtSource/SoundMgr.h"

#include <P2PEngine/P2PEngine.h>

//============================================================================
void INlc::toGuiPlayNlcMedia( AssetBaseInfo* assetInfo )
{
    getAppCommon().toGuiPlayNlcMedia( assetInfo );
}

//============================================================================
void INlc::toGuiLog( int logLevel, const char* msg )
{
    getAppCommon().toGuiLog( logLevel, msg );
}

//============================================================================
void INlc::toGuiAppErr( EAppErr eAppErr, const char* errMsg )
{
    getAppCommon().toGuiAppErr( eAppErr, errMsg );
}

//============================================================================
void INlc::toGuiStatusMessage( const char* statusMsg )
{
    getAppCommon().toGuiStatusMessage( statusMsg );
}

//============================================================================
void INlc::toGuiPluginMsg( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, const char* paramMsg, ... )
{
    std::string paramValue{ "" };
    if( paramMsg )
    {
        char szBuffer[2048];
        szBuffer[0] = 0;
        va_list arg_ptr;
        va_start( arg_ptr, paramMsg );
#ifdef TARGET_OS_WINDOWS
        vsnprintf( szBuffer, 2048, paramMsg, ( char* )arg_ptr );
#else
        vsnprintf( szBuffer, 2048, paramMsg, arg_ptr );
#endif //  TARGET_OS_WINDOWS
        szBuffer[2047] = 0;
        va_end( arg_ptr );
        if( 0 != szBuffer[0] )
        {
            paramValue = szBuffer;
        }
    }

    getAppCommon().toGuiPluginMsg( pluginType, onlineId, msgType, paramValue );
}

//============================================================================
void INlc::toGuiPluginMsg( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, std::string& paramMsg )
{
    getAppCommon().toGuiPluginMsg( pluginType, onlineId, msgType, paramMsg );
}

//============================================================================
void INlc::toGuiPluginCommError( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, ECommErr commErr )
{
    getAppCommon().toGuiPluginCommError( pluginType, onlineId, msgType, commErr );
}

//============================================================================
bool INlc::toGuiIsMicrophoneDeviceAvailable( void )
{
    return getAppCommon().getSoundMgr().toGuiIsMicrophoneDeviceAvailable();
}

//============================================================================
void INlc::toGuiWantMicrophoneRecording( EAppModule appModule, bool wantMicInput )
{
    getAppCommon().toGuiWantMicrophoneRecording( appModule, wantMicInput );
}

//============================================================================
void INlc::toGuiWantUserVoiceMicrophone( EAppModule appModule, VxGUID& onlineId, bool wantMicInput )
{
    getAppCommon().toGuiWantUserVoiceMicrophone( appModule, onlineId, wantMicInput );
}

//============================================================================
void INlc::toGuiWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput )
{
    getAppCommon().toGuiWantSpeakerOutput( appModule, wantSpeakerOutput );
}

//============================================================================
void INlc::toGuiWantUserVoiceSpeaker( EAppModule appModule, VxGUID& onlineId, bool wantSpeakerOutput )
{
    getAppCommon().toGuiWantUserVoiceSpeaker( appModule, onlineId, wantSpeakerOutput );
}

//============================================================================
int INlc::toGuiModuleAudioFrame( EAppModule appModule, int16_t * pu16PcmData, int pcmDataLenInBytes, bool isSilence )
{
    return getAppCommon().getSoundMgr().toGuiModuleAudioFrame( appModule, pu16PcmData, pcmDataLenInBytes, isSilence );
}

//============================================================================
int INlc::toGuiPlayerNlcAudio( EAppModule appModule, float* audioSamples48000, int dataLenInBytes )
{
    return getAppCommon().getSoundMgr().toGuiPlayerNlcAudio( appModule, audioSamples48000, dataLenInBytes );
}

//============================================================================
float INlc::toGuiGetAudioDelaySeconds( EAppModule appModule )
{
    return getAppCommon().getSoundMgr().toGuiGetAudioDelaySeconds( appModule );
}

//============================================================================
float INlc::toGuiGetAudioCacheFreeSpace( EAppModule appModule )
{
    return getAppCommon().getSoundMgr().toGuiGetAudioCacheFreeSpace( appModule );
}

//============================================================================
float INlc::toGuiGetAudioCacheTotalSeconds( EAppModule appModule )
{
    return getAppCommon().getSoundMgr().toGuiGetAudioCacheTotalSeconds( appModule );
}

//============================================================================
void INlc::toGuiWantVideoCapture( EAppModule appModule, bool wantVidCapture )
{
    getAppCommon().toGuiWantVideoCapture( appModule, wantVidCapture );
}

//============================================================================
void INlc::toGuiPlayVideoFrame( VxGUID& onlineId, uint8_t * pu8Jpg, uint32_t u32JpgDataLen, int motion0to100000 )
{
    if( false == getPtoP().isAppPaused() )
    {
        getAppCommon().toGuiPlayVideoFrame( onlineId, pu8Jpg, u32JpgDataLen, motion0to100000 );
    }
}

//============================================================================
//! show jpeg for video playback
int INlc::toGuiPlayVideoFrame( VxGUID& onlineId, uint8_t * picBuf, uint32_t picBufLen, int picWidth, int picHeight )
{
    if( false == getPtoP().isAppPaused() )
    {
        return getAppCommon().toGuiPlayVideoFrame( onlineId, picBuf, picBufLen, picWidth, picHeight );
    }

    return 0;
}

//============================================================================
void INlc::toGuiNetAvailableStatus( ENetAvailStatus eNetAvailStatus )
{
    getAppCommon().toGuiNetAvailableStatus( eNetAvailStatus );
}

//============================================================================
void INlc::toGuiNetworkState( ENetworkStateType eNetworkState, const char* stateMsg )
{
    getAppCommon().toGuiNetworkState( eNetworkState, stateMsg );
}

//============================================================================
void INlc::toGuiHostAnnounceStatus( EHostType hostType, VxGUID& sessionId, EHostAnnounceStatus annStatus, const char* msg )
{
    getAppCommon().toGuiHostAnnounceStatus( hostType, sessionId, annStatus, msg );
}

//============================================================================
void INlc::toGuiHostJoinStatus( EHostType hostType, VxGUID& sessionId, EHostJoinStatus joinStatus, const char* msg )
{
    getAppCommon().toGuiHostJoinStatus( hostType, sessionId, joinStatus, msg );
}

//============================================================================
void INlc::toGuiHostSearchStatus( EHostType hostType, VxGUID& sessionId, EHostSearchStatus searchStatus, ECommErr commErr, const char* msg )
{
    getAppCommon().toGuiHostSearchStatus( hostType, sessionId, searchStatus, commErr, msg );
}

//============================================================================
void INlc::toGuiHostSearchResult( EHostType hostType, VxGUID& sessionId, HostedInfo& hostedInfo )
{
    getAppCommon().toGuiHostSearchResult( hostType, sessionId, hostedInfo );
}

//============================================================================
void INlc::toGuiHostSearchComplete( EHostType hostType, VxGUID& sessionId )
{
    getAppCommon().toGuiHostSearchComplete( hostType, sessionId );
}

//============================================================================
void INlc::toGuiGroupieSearchStatus( EHostType hostType, VxGUID& sessionId, EHostSearchStatus searchStatus, ECommErr commErr, const char* msg )
{
    getAppCommon().toGuiGroupieSearchStatus( hostType, sessionId, searchStatus, commErr, msg );
}

//============================================================================
void INlc::toGuiGroupieSearchResult( EHostType hostType, VxGUID& sessionId, GroupieInfo& groupieInfo )
{
    getAppCommon().toGuiGroupieSearchResult( hostType, sessionId, groupieInfo );
}

//============================================================================
void INlc::toGuiGroupieSearchComplete( EHostType hostType, VxGUID& sessionId )
{
    getAppCommon().toGuiGroupieSearchComplete( hostType, sessionId );
}

//============================================================================
void INlc::toGuiIsPortOpenStatus( EIsPortOpenStatus eIsPortOpenStatus, const char* msg )
{
    getAppCommon().toGuiIsPortOpenStatus( eIsPortOpenStatus, msg );
}

//============================================================================
void INlc::toGuiRunTestStatus( const char*testName, ERunTestStatus eRunTestStatus, const char* msg )
{
    getAppCommon().toGuiRunTestStatus( testName, eRunTestStatus, msg );
}

//============================================================================
void INlc::toGuiRandomConnectStatus( ERandomConnectStatus eRandomConnectStatus, const char* msg )
{
    getAppCommon().toGuiRandomConnectStatus( eRandomConnectStatus, msg );
}

//============================================================================
void INlc::toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp )
{
    getAppCommon().toGuiIndentListUpdate( listType, onlineId, timestamp );
}

//============================================================================
void INlc::toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId )
{
    getAppCommon().toGuiIndentListRemove( listType, onlineId );
}

//============================================================================
void INlc::toGuiContactAdded( VxNetIdent* netIdent )
{
    getAppCommon().toGuiContactAdded( netIdent );
}

//============================================================================
void INlc::toGuiContactRemoved( VxGUID& onlineId )
{
    getAppCommon().toGuiContactRemoved( onlineId );
}

//============================================================================
void INlc::toGuiContactOnline( VxNetIdent* netIdent )
{
    getAppCommon().toGuiContactOnline( netIdent );
}

////============================================================================
//void INlc::toGuiContactNameChange( VxNetIdent* netIdent )
//{
//    getAppCommon().toGuiContactNameChange( netIdent );
//}
//
////============================================================================
//void INlc::toGuiContactDescChange( VxNetIdent* netIdent )
//{
//    getAppCommon().toGuiContactDescChange( netIdent );
//}
//
////============================================================================
//void INlc::toGuiContactFriendshipChange( VxNetIdent* netIdent )
//{
//    getAppCommon().toGuiContactFriendshipChange( netIdent );
//}
//
////============================================================================
//void INlc::toGuiContactSearchFlagsChange( VxNetIdent* netIdent )
//{
//    getAppCommon().toGuiContactSearchFlagsChange( netIdent );
//}

//============================================================================
void INlc::toGuiContactLastSessionTimeChange( VxNetIdent* netIdent )
{
    getAppCommon().toGuiContactLastSessionTimeChange( netIdent );
}

//============================================================================
void INlc::toGuiContactAnythingChange( VxNetIdent* netIdent )
{
    getAppCommon().toGuiContactAnythingChange( netIdent );
}

////============================================================================
//void INlc::toGuiPluginPermissionChange( VxNetIdent* netIdent )
//{
//    getAppCommon().toGuiPluginPermissionChange( netIdent );
//}

//============================================================================
void INlc::toGuiUpdateMyIdent( VxNetIdent* netIdent )
{
    getAppCommon().toGuiUpdateMyIdent( netIdent );
}

//============================================================================
void INlc::toGuiSaveMyIdent( VxNetIdent* netIdent )
{
    getAppCommon().toGuiSaveMyIdent( netIdent );
}

//============================================================================
void INlc::toGuiRxedPluginOffer( VxGUID onlineId, OfferBaseInfo& offerInfo )
{
    getAppCommon().toGuiRxedPluginOffer( onlineId, offerInfo );
}

//============================================================================
void INlc::toGuiRxedOfferReply( VxGUID onlineId, OfferBaseInfo& offerInfo )
{
    getAppCommon().toGuiRxedOfferReply( onlineId, offerInfo );
}

//============================================================================
void INlc::toGuiPluginSessionEnded( VxNetIdent* netIdent, EPluginType pluginType, VxGUID& lclSessionId )
{
    getAppCommon().toGuiPluginSessionEnded( netIdent, pluginType, lclSessionId );
}

//============================================================================
void INlc::toGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue )
{                             
    getAppCommon().toGuiPluginStatus( pluginType, statusType, statusValue );                               
}

//============================================================================
void INlc::toGuiInstMsg( VxNetIdent*	    netIdent,
                          EPluginType		pluginType,
                          const char*	    pMsg )
{
    getAppCommon().toGuiInstMsg(  netIdent,
                                  pluginType,
                                  pMsg );
}

//============================================================================
void INlc::toGuiFileList( VxGUID& appInstId, FileInfo& fileInfo )
{
    getAppCommon().toGuiFileList( appInstId, fileInfo );
}

//============================================================================
void INlc::toGuiFileListReply( VxGUID& onlineId, EPluginType pluginType, FileInfo& fileInfo )
{
    getAppCommon().toGuiFileListReply( onlineId, pluginType, fileInfo );
}

//============================================================================
void INlc::toGuiFileListCompleted( VxGUID& appInstId )
{
    getAppCommon().toGuiFileListCompleted( appInstId );
}

//============================================================================
void INlc::toGuiFileUploadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
    getAppCommon().toGuiFileUploadStart( onlineId, pluginType, lclSessionId, fileInfo );
}

//============================================================================
void INlc::toGuiFileDownloadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
    getAppCommon().toGuiFileDownloadStart( onlineId, pluginType, lclSessionId, fileInfo );
}

//============================================================================
void INlc::toGuiFileXferState( EPluginType pluginType, VxGUID&	lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 )
{
    getAppCommon().toGuiFileXferState( pluginType, lclSessionId, xferDir, xferState, xferErr, param1 );
}

//============================================================================
void INlc::toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, std::string& fileName, EXferError xferError )
{
    getAppCommon().toGuiFileDownloadComplete( pluginType, lclSessionId, fileName, xferError );
}

//============================================================================
void INlc::toGuiFileDeleted( std::string& fileName )
{
    getAppCommon().toGuiFileDeleted( fileName );
}

//============================================================================
void INlc::toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, std::string& fileName, EXferError xferError )
{
    getAppCommon().toGuiFileUploadComplete( pluginType, lclSessionId, fileName, xferError );
}

//============================================================================
void INlc::toGuiScanSearchComplete( EScanType eScanType )
{
    getAppCommon().toGuiScanSearchComplete( eScanType );
}

//============================================================================
void INlc::toGuiScanResultSuccess( EScanType eScanType, VxNetIdent*	netIdent )
{
    getAppCommon().toGuiScanResultSuccess( eScanType, netIdent );
}

//============================================================================
void INlc::toGuiSearchResultError( EScanType eScanType, VxNetIdent* netIdent, int errCode )
{
    getAppCommon().toGuiSearchResultError( eScanType, netIdent, errCode );
}

//============================================================================
void INlc::toGuiSearchResultProfilePic(    VxNetIdent*	netIdent,
                                            uint8_t *		pu8JpgData,
                                            uint32_t		u32JpgDataLen )
{
    getAppCommon().toGuiSearchResultProfilePic( netIdent, pu8JpgData, u32JpgDataLen );
}

//============================================================================
void INlc::toGuiSearchResultFileSearch( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
    getAppCommon().toGuiSearchResultFileSearch( onlineId, pluginType, lclSessionId, fileInfo );
}

//============================================================================
void INlc::toGuiModuleState( EAppModule moduleNum, EModuleState moduleState )
{
	getAppCommon().toGuiModuleState( moduleNum, moduleState );
}

//============================================================================
void INlc::toGuiSetGameValueVar( EPluginType	pluginType,
                                      VxGUID&	oOnlineId,
                                      int32_t	s32VarId,
                                      int32_t	s32VarValue )
{
    getAppCommon().toGuiSetGameValueVar(  pluginType,
                                                 oOnlineId,
                                                 s32VarId,
                                                 s32VarValue );
}

//============================================================================
void INlc::toGuiSetGameActionVar( EPluginType	pluginType,
                                       VxGUID&	oOnlineId,
                                       int32_t	s32VarId,
                                       int32_t	s32VarValue )
{
    getAppCommon().toGuiSetGameActionVar(  pluginType,
                                                  oOnlineId,
                                                  s32VarId,
                                                  s32VarValue );
}

//============================================================================
void INlc::toGuiAssetAdded( AssetBaseInfo* assetInfo )
{
    LogModule( eLogAssets, LOG_VERBOSE, "INlc::toGuiAssetAdded" );
    getAppCommon().toGuiAssetAdded( assetInfo );
}

//============================================================================
void INlc::toGuiAssetUpdated( AssetBaseInfo* assetInfo )
{
    LogModule( eLogAssets, LOG_VERBOSE, "INlc::toGuiAssetUpdated" );
    getAppCommon().toGuiAssetUpdated( assetInfo );
}

//============================================================================
void INlc::toGuiAssetRemoved( AssetBaseInfo* assetInfo )
{
    LogModule( eLogAssets, LOG_VERBOSE, "INlc::toGuiAssetRemoved" );
    getAppCommon().toGuiAssetRemoved( assetInfo );
}

//============================================================================
void INlc::toGuiAssetXferState( VxGUID& assetUniqueId, EAssetSendState assetSendState, int param )
{
    getAppCommon().toGuiAssetXferState( assetUniqueId, assetSendState, param );
}

//============================================================================
void INlc::toGuiAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
    getAppCommon().toGuiAssetAction( assetAction, assetId, pos0to100000 );
}

//============================================================================
void INlc::toGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID onlineId, int pos0to100000 )
{
    getAppCommon().toGuiMultiSessionAction( mSessionAction,
                                 onlineId,
                                 pos0to100000 );
}

//============================================================================
void INlc::toGuiAssetSessionHistory( AssetBaseInfo* assetInfo )
{
    getAppCommon().toGuiAssetSessionHistory( assetInfo );
}

//============================================================================
void INlc::toGuiBlobAdded( BlobInfo * hostListInfo )
{
    getAppCommon().toGuiBlobAdded( hostListInfo );
}

//============================================================================
void INlc::toGuiBlobAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
    getAppCommon().toGuiBlobAction( assetAction,
                                        assetId,
                                        pos0to100000 );
}

//============================================================================
void INlc::toGuiBlobSessionHistory( BlobInfo * hostListInfo )
{
    getAppCommon().toGuiBlobSessionHistory( hostListInfo );
}

//============================================================================
void INlc::toGuiNetworkIsTested( bool requiresRelay, std::string& ipAddr, uint16_t ipPort )
{
    getAppCommon().toGuiNetworkIsTested( requiresRelay, ipAddr, ipPort );
}
