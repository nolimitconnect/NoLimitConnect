//============================================================================
// Copyright (C) 2023 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include <GuiInterface/IAudioDefs.h>

#include <BlobXferMgr/BlobInfo.h>
#include <Plugins/FileInfo.h>
#include <PluginSettings/PluginSetting.h>

//============================================================================
void  AppCommon::registerMetaData( void )
{
	qRegisterMetaType<uint8_t>( "uint8_t" );
	qRegisterMetaType<uint8_t>( "uint16_t" );
	qRegisterMetaType<int32_t>( "int32_t" );
	qRegisterMetaType<uint32_t>( "uint32_t" );
	qRegisterMetaType<uint64_t>( "uint64_t" );
	qRegisterMetaType<int64_t>( "int64_t" );

	qRegisterMetaType<EAppErr>( "EAppErr" );
	qRegisterMetaType<EApplet>( "EApplet" );
	qRegisterMetaType<EAppModule>( "EAppModule" );
	qRegisterMetaType<EAssetAction>( "EAssetAction" );
	qRegisterMetaType<EAssetSendState>( "EAssetSendState" );
	qRegisterMetaType<EAssetType>( "EAssetType" );
	
	qRegisterMetaType<EAudioTestState>( "EAudioTestState" );

	qRegisterMetaType<EConnectReason>( "EConnectReason" );
	qRegisterMetaType<EConnectStatus>( "EConnectStatus" );
	qRegisterMetaType<EContentCatagory>( "EContentCatagory" );
	qRegisterMetaType<EContentRating>( "EContentRating" );
	qRegisterMetaType<EFileFilterType>( "EFileFilterType" );
	qRegisterMetaType<EFriendState>( "EFriendState" );
	qRegisterMetaType<EFriendViewType>( "EFriendViewType" );
	qRegisterMetaType<EGenderType>( "EGenderType" );
	qRegisterMetaType<EHostAnnounceStatus>( "EHostAnnounceStatus" );
	qRegisterMetaType<EHostJoinStatus>( "EHostJoinStatus" );
	qRegisterMetaType<EHostSearchStatus>( "EHostSearchStatus" );
	qRegisterMetaType<EHostServiceType>( "EHostServiceType" );
	qRegisterMetaType<EHostTestStatus>( "EHostTestStatus" );
	qRegisterMetaType<EHostType>( "EHostType" );
	qRegisterMetaType<EInternetStatus>( "EInternetStatus" );
	qRegisterMetaType<EIsPortOpenStatus>( "EIsPortOpenStatus" );
	qRegisterMetaType<ERunTestStatus>( "ERunTestStatus" );
	qRegisterMetaType<ENetAvailStatus>( "ENetAvailStatus" );
	qRegisterMetaType<ENetworkStateType>( "ENetworkStateType" );
	qRegisterMetaType<EOfferResponse>( "EOfferResponse" );
	qRegisterMetaType<EOfferState>( "EOfferState" );
	qRegisterMetaType<EOfferType>( "EOfferType" );
	qRegisterMetaType<EPluginAccess>( "EPluginAccess" );
	qRegisterMetaType<EPluginType>( "EPluginType" );
	qRegisterMetaType<EPushToTalkStatus>( "EPushToTalkStatus" );
	qRegisterMetaType<ERandomConnectStatus>( "ERandomConnectStatus" );
	qRegisterMetaType<EMSessionAction>( "EMSessionAction" );
	qRegisterMetaType<EScanType>( "EScanType" );
	qRegisterMetaType<ESndDef>( "ESndDef" );
	qRegisterMetaType<EXferError>( "EXferError" );
	qRegisterMetaType<EXferState>( "EXferState" );
	qRegisterMetaType<PluginSetting>( "PluginSetting" );
	qRegisterMetaType<VxGUID>( "VxGUID" );
	qRegisterMetaType<VxNetIdent>( "VxNetIdent" );
	qRegisterMetaType<EUserViewType>( "EUserViewType" );
	qRegisterMetaType<ThumbInfo>( "ThumbInfo" );
	qRegisterMetaType<HostedInfo>( "HostedInfo" );
	qRegisterMetaType<HostJoinInfo>( "HostJoinInfo" );
	qRegisterMetaType<EJoinState>( "EJoinState" );
	qRegisterMetaType<EOnlineState>( "EOnlineState" );
	qRegisterMetaType<GroupieId>( "GroupieId" );
	qRegisterMetaType<ConnectId>( "ConnectId" );
	qRegisterMetaType<EWebPageType>( "EWebPageType" );
	qRegisterMetaType<EPluginMsgType>( "EPluginMsgType" );
	qRegisterMetaType<ECommErr>( "ECommErr" );
	qRegisterMetaType<VxSha1Hash>( "VxSha1Hash" );
	qRegisterMetaType<AssetBaseInfo>( "AssetBaseInfo" );
	qRegisterMetaType<BlobInfo>( "BlobInfo" );
	qRegisterMetaType<FileInfo>( "FileInfo" );
	qRegisterMetaType<EXferDirection>( "EXferDirection" );
	qRegisterMetaType<OfferBaseInfo>( "OfferBaseInfo" );
	qRegisterMetaType<ESha1GenResult>( "ESha1GenResult" );	
	qRegisterMetaType<EMediaPlayerAction>( "EMediaPlayerAction" );
	qRegisterMetaType<EMediaError>( "EMediaError" );
}

//============================================================================
void AppCommon::connectSignals( void )
{
    connect( this, SIGNAL( signalPlaySound(ESndDef) ), this, SLOT( slotPlaySound(ESndDef) ) );
    connect( this, SIGNAL( signalStatusMsg(QString) ), this, SLOT( slotStatusMsg(QString) ) );
    connect( this, SIGNAL( signalUserMsg(QString) ), this, SLOT( slotStatusMsg(QString) ) );
    connect( this, SIGNAL( signalAppErr(EAppErr,QString) ), this, SLOT( slotAppErr(EAppErr,QString) ) );
    connect( this, SIGNAL( signalNetworkStateChanged(ENetworkStateType) ), this, SLOT( slotNetworkStateChanged(ENetworkStateType) ) );

    connect( this, SIGNAL( signalToGuiInstMsg(GuiUser*,EPluginType,QString) ), this, SLOT( slotToGuiInstMsg(GuiUser*,EPluginType,QString) ), Qt::QueuedConnection );

    connect( this, SIGNAL(signalInternalWantMicrophoneRecording(EAppModule,bool)), this, SLOT(slotInternalWantMicrophoneRecording(EAppModule,bool)), Qt::QueuedConnection );
	connect( this, SIGNAL(signalInternalWantUserVoiceMicrophone(EAppModule,VxGUID,bool)), this, SLOT(slotInternalWantUserVoiceMicrophone(EAppModule,VxGUID,bool)), Qt::QueuedConnection );

    connect( this, SIGNAL(signalInternalWantSpeakerOutput(EAppModule,bool)), this, SLOT(slotInternalWantSpeakerOutput(EAppModule,bool)), Qt::QueuedConnection );
	connect( this, SIGNAL(signalInternalWantUserVoiceSpeaker(EAppModule,VxGUID,bool)), this, SLOT(slotInternalWantUserVoiceSpeaker(EAppModule,VxGUID,bool)), Qt::QueuedConnection );

	connect( this, SIGNAL(signalInternalWantVideoCapture(EAppModule,bool)), this, SLOT(slotInternalWantVideoCapture(EAppModule,bool)), Qt::QueuedConnection );

    connect( this, SIGNAL( signalSetRelayHelpButtonVisibility(bool) ), this, SLOT( slotSetRelayHelpButtonVisibility(bool) ) );
    connect( this, SIGNAL( signalInternalPluginMessage(EPluginType,VxGUID,EPluginMsgType,QString) ), this, SLOT( slotInternalPluginMessage(EPluginType,VxGUID,EPluginMsgType,QString) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalPluginErrorMsg(EPluginType,VxGUID,EPluginMsgType,ECommErr) ), this, SLOT( slotInternalPluginErrorMsg(EPluginType,VxGUID,EPluginMsgType,ECommErr) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalToGuiSetGameValueVar(EPluginType,VxGUID,int32_t,int32_t) ), this, SLOT( slotInternalToGuiSetGameValueVar(EPluginType,VxGUID,int32_t,int32_t) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiSetGameActionVar(EPluginType,VxGUID,int32_t,int32_t) ), this, SLOT( slotInternalToGuiSetGameActionVar(EPluginType,VxGUID,int32_t,int32_t) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalToGuiAssetAdded(AssetBaseInfo) ), this, SLOT( slotInternalToGuiAssetAdded(AssetBaseInfo) ), Qt::QueuedConnection );
	connect( this, SIGNAL( signalInternalToGuiAssetUpdated(AssetBaseInfo) ), this, SLOT( slotInternalToGuiAssetUpdated(AssetBaseInfo) ), Qt::QueuedConnection );
	connect( this, SIGNAL( signalInternalToGuiAssetRemoved(AssetBaseInfo) ), this, SLOT( slotInternalToGuiAssetRemoved(AssetBaseInfo) ), Qt::QueuedConnection );

	connect( this, SIGNAL( signalInternalToGuiAssetXferState(VxGUID,EAssetSendState,int) ), this, SLOT( slotInternalToGuiAssetXferState(VxGUID,EAssetSendState,int) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalToGuiAssetSessionHistory(AssetBaseInfo*) ), this, SLOT( slotInternalToGuiAssetSessionHistory(AssetBaseInfo*) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiAssetAction(EAssetAction,VxGUID,int) ), this, SLOT( slotInternalToGuiAssetAction(EAssetAction,VxGUID,int) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalMultiSessionAction(VxGUID,EMSessionAction,int) ), this, SLOT( slotInternalMultiSessionAction(VxGUID,EMSessionAction,int) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalBlobAction(EAssetAction,VxGUID,int) ), this, SLOT( slotInternalBlobAction(EAssetAction,VxGUID,int) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalBlobAdded(BlobInfo) ), this, SLOT( slotInternalBlobAdded(BlobInfo) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalBlobSessionHistory(BlobInfo) ), this, SLOT( slotInternalBlobSessionHistory(BlobInfo) ), Qt::QueuedConnection );


    connect( this, SIGNAL( signalInternalToGuiIndentListUpdate(EUserViewType,VxGUID,uint64_t) ), this, SLOT( slotInternalToGuiIndentListUpdate(EUserViewType,VxGUID,uint64_t) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiIndentListRemove(EUserViewType,VxGUID) ), this, SLOT( slotInternalToGuiIndentListRemove(EUserViewType,VxGUID) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalToGuiContactAdded(VxNetIdent) ), this, SLOT( slotInternalToGuiContactAdded(VxNetIdent) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiContactRemoved(VxGUID) ), this, SLOT( slotInternalToGuiContactRemoved(VxGUID) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalToGuiContactOnline(VxNetIdent) ), this, SLOT( slotInternalToGuiContactOnline(VxNetIdent) ), Qt::QueuedConnection );

    //connect( this, SIGNAL( signalInternalToGuiContactNameChange(VxNetIdent) ), this, SLOT( slotInternalToGuiContactNameChange(VxNetIdent) ), Qt::QueuedConnection );
    //connect( this, SIGNAL( signalInternalToGuiContactDescChange(VxNetIdent) ), this, SLOT( slotInternalToGuiContactDescChange(VxNetIdent) ), Qt::QueuedConnection );
    //connect( this, SIGNAL( signalInternalToGuiContactFriendshipChange(VxNetIdent) ), this, SLOT( slotInternalToGuiContactFriendshipChange(VxNetIdent) ), Qt::QueuedConnection );

    //connect( this, SIGNAL( signalInternalToGuiPluginPermissionChange(VxNetIdent) ), this, SLOT( slotInternalToGuiPluginPermissionChange(VxNetIdent) ), Qt::QueuedConnection );
    //connect( this, SIGNAL( signalInternalToGuiContactSearchFlagsChange(VxNetIdent) ), this, SLOT( slotInternalToGuiContactSearchFlagsChange(VxNetIdent) ), Qt::QueuedConnection );

	connect( this, SIGNAL(signalInternalToGuiContactUpdated(VxNetIdent)), this, SLOT(slotInternalToGuiContactUpdated(VxNetIdent)), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalToGuiContactLastSessionTimeChange(VxNetIdent) ), this, SLOT( slotInternalToGuiContactLastSessionTimeChange(VxNetIdent) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalToGuiUpdateIdent(VxNetIdent) ), this, SLOT( slotInternalToGuiUpdateIdent(VxNetIdent) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiSaveMyIdent(VxNetIdent) ), this, SLOT( slotInternalToGuiSaveMyIdent(VxNetIdent) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalToGuiScanSearchComplete(EScanType) ), this, SLOT( slotInternalToGuiScanSearchComplete(EScanType) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiScanResultSuccess(EScanType,VxNetIdent) ), this, SLOT( slotInternalToGuiScanResultSuccess(EScanType,VxNetIdent) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiSearchResultError(EScanType,VxNetIdent,int) ), this, SLOT( slotInternalToGuiSearchResultError(EScanType,VxNetIdent,int) ), Qt::QueuedConnection );

    connect( this, SIGNAL( signalInternalNetworkIsTested(bool,QString,uint16_t) ), this, SLOT( slotInternalNetworkIsTested(bool,QString,uint16_t) ), Qt::QueuedConnection );

	connect( this, SIGNAL( signalInternalPlayNlcMedia(AssetBaseInfo) ), this, SLOT( slotInternalPlayNlcMedia(AssetBaseInfo) ), Qt::QueuedConnection );

	connect( this, SIGNAL( signalInternalToGuiPluginStatus(EPluginType,int,int) ), this, SLOT( slotInternalToGuiPluginStatus(EPluginType,int,int) ), Qt::QueuedConnection );

	connect( this, SIGNAL( signalInternalToGuiSearchResultFileSearch(VxGUID,EPluginType,VxGUID,FileInfo)), this, 
		           SLOT( slotInternalToGuiSearchResultFileSearch(VxGUID,EPluginType,VxGUID,FileInfo)), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiFileListReply(VxGUID,EPluginType,FileInfo)), this,
                   SLOT( slotInternalToGuiFileListReply(VxGUID,EPluginType,FileInfo)), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiFileDownloadStart(VxGUID,EPluginType,VxGUID,FileInfo) ), this,
        SLOT( slotInternalToGuiFileDownloadStart(VxGUID,EPluginType,VxGUID,FileInfo) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiFileDownloadComplete(EPluginType,VxGUID,QString,EXferError) ), this,
        SLOT( slotInternalToGuiFileDownloadComplete(EPluginType,VxGUID,QString,EXferError) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiFileUploadStart(VxGUID,EPluginType,VxGUID,FileInfo) ), this,
        SLOT( slotInternalToGuiFileUploadStart(VxGUID,EPluginType,VxGUID,FileInfo) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiFileUploadComplete(EPluginType,VxGUID,QString,EXferError) ), this,
        SLOT( slotInternalToGuiFileUploadComplete(EPluginType,VxGUID,QString,EXferError) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiFileXferState(EPluginType,VxGUID,EXferDirection,EXferState,EXferError,int) ), this,
        SLOT( slotInternalToGuiFileXferState(EPluginType,VxGUID,EXferDirection,EXferState,EXferError,int) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiFileList(FileInfo) ), this, SLOT( slotInternalToGuiFileList(FileInfo) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalToGuiFileListCompleted() ), this, SLOT( slotInternalToGuiFileListCompleted() ), Qt::QueuedConnection );

	connect( this, SIGNAL( signalInternalToGuiRxedPluginOffer(VxGUID,OfferBaseInfo) ), this,
		SLOT( slotInternalToGuiRxedPluginOffer(VxGUID,OfferBaseInfo) ), Qt::QueuedConnection );
	connect( this, SIGNAL( signalInternalToGuiRxedOfferReply(VxGUID,OfferBaseInfo) ), this,
		SLOT( slotInternalToGuiRxedOfferReply(VxGUID,OfferBaseInfo) ), Qt::QueuedConnection );

	connect( this, SIGNAL( signalInternalToGuiPluginSessionEnded(VxGUID,EPluginType,VxGUID) ), this,
		SLOT( slotInternalToGuiPluginSessionEnded(VxGUID,EPluginType,VxGUID) ), Qt::QueuedConnection );

	connect( this, SIGNAL( signalInternalMediaAction(EAppModule,EMediaPlayerAction,int,QString) ), this,
		SLOT( slotInternalMediaAction(EAppModule,EMediaPlayerAction,int,QString) ), Qt::QueuedConnection );
	connect( this, SIGNAL( signalInternalMediaError(EAppModule,EMediaError,QString) ), this,
		SLOT( slotInternalMediaError(EAppModule,EMediaError,QString) ), Qt::QueuedConnection );
}
