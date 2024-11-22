#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IFromGui.h>
#include <GuiInterface/IAudioDefs.h>

#include <PktLib/PktVoiceReq.h>

#include <CoreLib/VxMutex.h>
#include <CoreLib/VxThread.h>
#include <CoreLib/VxSemaphore.h>

#include <string>
#include <vector>

class PluginBase;
class IToGui;
class P2PEngine;
class PluginMgr;
class MediaTools;
class RawAudio;
class RawVideo;
class MediaClient;
class PktVideoFeedPic;
class PktVideoFeedPicChunk;

class ClientToRemove
{
public:
	ClientToRemove();
	ClientToRemove( VxGUID&						onlineId,
					EMediaInputType				mediaType, 
					MediaCallbackInterface *	callback,
					EAppModule					appModule)
	: m_OnlineId( onlineId )
	, m_MediaType( mediaType )
	, m_Callback( callback )
	, m_AppModule( appModule )
	{
	}

	ClientToRemove( const ClientToRemove& rhs )
	{
		if( &rhs != this )
		{
			*this = rhs;
		}
	}

	ClientToRemove&				operator =( const ClientToRemove& rhs )
	{
		if( &rhs != this )
		{
			m_OnlineId = rhs.m_OnlineId;
			m_MediaType = rhs.m_MediaType;
			m_Callback = rhs.m_Callback;
			m_AppModule = rhs.m_AppModule;
		}
		return *this;
	}

	VxGUID						m_OnlineId;
	EMediaInputType				m_MediaType;
	MediaCallbackInterface *	m_Callback;
	EAppModule					m_AppModule{ eAppModuleInvalid };
};

//#define DEBUG_PROCESSOR_LOCK 1
//#define DEBUG_AUDIO_PROCESSOR_LOCK 1
class MediaProcessor : public MediaCallbackInterface
{
public:
	const int JPG_CONVERT_QUALITY = 75; // there is very little picture quality improvement above 75 

	class AudioProcessorLock
	{
	public:
		AudioProcessorLock( MediaProcessor * processor ) : m_Mutex(processor->getAudioMutex())	
		{ 
#ifdef DEBUG_AUDIO_PROCESSOR_LOCK
	LogMsg( LOG_INFO, "AudioProcessorLock Lock start");
#endif //DEBUG_AUDIO_PROCESSOR_LOCK
			m_Mutex.lock(); 
#ifdef DEBUG_AUDIO_PROCESSOR_LOCK
			LogMsg( LOG_INFO, "AudioProcessorLock Lock complete");
#endif //DEBUG_AUDIO_PROCESSOR_LOCK
		}

		~AudioProcessorLock()																		
		{ 
#ifdef DEBUG_AUDIO_PROCESSOR_LOCK
			LogMsg( LOG_INFO, "AudioProcessorLock Unlock");
#endif //DEBUG_AUDIO_PROCESSOR_LOCK
			m_Mutex.unlock(); 
		}

		VxMutex&				m_Mutex;
	};

	class VideoProcessorLock
	{
	public:
		VideoProcessorLock( MediaProcessor * processor ) : m_Mutex(processor->getVideoMutex())	
		{ 
#ifdef DEBUG_PROCESSOR_LOCK
			LogMsg( LOG_INFO, "VideoProcessorLock Lock start");
#endif //DEBUG_PROCESSOR_LOCK
			m_Mutex.lock(); 
#ifdef DEBUG_PROCESSOR_LOCK
			LogMsg( LOG_INFO, "VideoProcessorLock Lock complete");
#endif //DEBUG_PROCESSOR_LOCK
		}

		~VideoProcessorLock()																		
		{ 
#ifdef DEBUG_PROCESSOR_LOCK
			LogMsg( LOG_INFO, "VideoProcessorLock Unlock");
#endif //DEBUG_PROCESSOR_LOCK
			m_Mutex.unlock(); 
		}

		VxMutex&				m_Mutex;
	};

	MediaProcessor( P2PEngine& engine );
	virtual ~MediaProcessor();
	
	void						shutdownMediaProcessor( void );

	VxMutex&					getAudioMutex( void )				{ return m_AudioMutex; }
	VxMutex&					getVideoMutex( void )				{ return m_VideoMutex; }
	MediaTools&					getMediaTools( void )				{ return m_MediaTools; }

	bool						isSpeakerOutputEnabled( void )		{ return m_SpeakerOutputEnabled; }
	bool						isMicrophoneCaptureEnabled( void )	{ return m_MicCaptureEnabled; }
	bool						isVideoCaptureEnabled( void )		{ return m_VidCaptureEnabled; }

	virtual void				wantAppIdle( EPluginType pluginType, bool bWantAppIdle );

	virtual void				wantMediaInput( VxGUID&						onlineId,
												EMediaInputType				mediaType, 
												MediaCallbackInterface *	callback, 
												EAppModule					appModule,
												bool						wantInput );

	virtual void				fromGuiVideoData( uint32_t u32FourCc, uint8_t * pu8VidDataIn, int iWidth, int iHeight, uint32_t u32VidDataLen, int iRotation );
	virtual void				fromGuiYUV420CaptureImage( uint8_t * yBytes, uint8_t * uBytes, uint8_t * vBytes, 
														int yRowStride, int uRowStride, int vRowStride,
														int yPixStride, int uPixStride, int vPixStride,
														int imageWidth, int imageHeight, int imageRotate );

	virtual void				fromGuiEchoCanceledSamplesThreaded( int16_t* pcmData, int sampleCnt, bool isSilence );
	virtual void				fromGuiAudioOutSpaceAvaiThreaded( int freeSpaceLen );

	void						increasePcmSampleVolume( int16_t * pcmData, uint16_t pcmDataLen, float volumePercent0To100 );
	void						playAudio( int16_t * pcmData, int dataLenInBytes );

	void						muteSpeaker( bool muteSpeaker )							{ m_MuteSpeaker = muteSpeaker; }
	bool						isSpeakerMuted( void )									{ return m_MuteSpeaker; }
	void						muteMicrophone( bool muteMic )							{ m_MuteMicrophone = muteMic; }
	bool						isMicrophoneMuted( void )								{ return m_MuteMicrophone; }

	void						processFriendVideoFeed(	VxGUID&			onlineId, 
														uint8_t *		jpgData, 
														uint32_t		jpgDataLen,
														int				motion0To100000 );

	void						processFriendAudioFeed(	VxGUID&	onlineId, int16_t * pcmData, uint16_t pcmDataLen, bool dontLock = false );

	void						processAudioInThreaded( void );
	void						processVideoIn( void );

	void 						setMyIdInVidPktListCount( int cnt ) { m_VidPktListContainsMyIdCnt = cnt; }
	int							getMyIdInVidPktListCount( void )	{ return m_VidPktListContainsMyIdCnt; }

protected:
	void						processRawAudioIn( RawAudio * rawAudio );
	void						processRawVideoIn( RawVideo * rawVideo );
	bool						isAudioMediaType( EMediaInputType mediaType );
	void						wantAudioMediaInput(	VxGUID&						onlineId,
														EMediaInputType				mediaType, 
														MediaCallbackInterface *	callback, 
														EAppModule					appModule,
														bool						wantInput );

	void						wantMixerMediaInput(	VxGUID&						onlineId,
														EMediaInputType				mediaType, 
														MediaCallbackInterface *	callback, 
														EAppModule					appModule,
														bool						wantInput );

	void						wantVideoMediaInput(	VxGUID&						onlineId,
														EMediaInputType				mediaType, 
														MediaCallbackInterface *	callback, 
														EAppModule					appModule,
														bool						wantInput );
	
	void						doMixerClientRemovals( std::vector<ClientToRemove>& clientRemoveList );
	void						doAudioClientRemovals( std::vector<ClientToRemove>& clientRemoveList );
	void						doVideoClientRemovals( std::vector<ClientToRemove>& clientRemoveList );

	bool						clientExistsInList(	std::vector<MediaClient>&		clientList, 
													VxGUID&							onlineId,
													EMediaInputType					mediaType, 
													MediaCallbackInterface *		callback );

	bool						removeClientFromListist( std::vector<MediaClient>& clientList,
														 VxGUID&						onlineId,
														EMediaInputType					mediaType,
														MediaCallbackInterface*			callback );

	bool						clientToRemoveExistsInList(	std::vector<ClientToRemove>&	clientRemoveList, 
															VxGUID&							onlineId,
															EMediaInputType					mediaType, 
															MediaCallbackInterface *		callback );

	bool						clientToRemoveRemoveFromList(	std::vector<ClientToRemove>&	clientRemoveList, 
																VxGUID&							onlineId,
																EMediaInputType					mediaType, 
																MediaCallbackInterface *		callback );

	//=== vars ===//
	P2PEngine&					m_Engine;
	PluginMgr&					m_PluginMgr;
	
	MediaTools&					m_MediaTools;
	VxMutex						m_AudioMutex;
	VxMutex						m_VideoMutex;

	std::vector<PluginBase*>	m_aoWantAppIdle;				// list of plugins that want called on app idle

	std::vector<MediaClient>	m_AudioPcmList;	
	std::vector<MediaClient>	m_AudioOpusList;	
	std::vector<MediaClient>	m_AudioPktsList;

	std::vector<RawAudio *>		m_ProcessAudioQue;
	VxMutex						m_AudioQueInMutex;
	VxThread					m_ProcessAudioInThread;
	VxSemaphore					m_AudioInSemaphore;
	PktVoiceReq					m_PktVoiceReq;

	std::vector<MediaClient>	m_VideoJpgBigList;	
	std::vector<MediaClient>	m_VideoJpgSmallList;
	std::vector<MediaClient>	m_VideoPktsList;
	std::vector<MediaClient>	m_MixerList;
	VxMutex						m_MixerClientsMutex;

	std::vector<ClientToRemove>	m_MixerClientRemoveList;
	VxMutex						m_MixerRemoveMutex;
	std::vector<ClientToRemove>	m_VideoClientRemoveList;
	VxMutex						m_VideoRemoveMutex;
	std::vector<ClientToRemove>	m_AudioClientRemoveList;
	VxMutex						m_AudioRemoveMutex;


	std::vector<RawVideo *>		m_ProcessVideoQue;
	VxMutex						m_VideoQueInMutex;
	VxThread					m_ProcessVideoThread;
	VxSemaphore					m_VideoSemaphore;
	PktVideoFeedPic *			m_PktVideoFeedPic;
	std::vector<PktVideoFeedPicChunk *> m_VidChunkList;

	VxMutex						m_MixerBufferMutex;

	int16_t						m_QuietAudioBuf[ AUDIO_SAMPLES_PER_FRAME ];
	int16_t						m_MixerBuf[ AUDIO_SAMPLES_PER_FRAME ];
	bool						m_MixerBufUsed{ false };

	bool						m_MuteSpeaker{ false };
	bool						m_MuteMicrophone{ false };
	bool						m_VidCaptureEnabled{ false };
	bool						m_MicCaptureEnabled{ false };
	bool						m_SpeakerOutputEnabled{ false };

	int							m_VidPktListContainsMyIdCnt{ 0 };
};
