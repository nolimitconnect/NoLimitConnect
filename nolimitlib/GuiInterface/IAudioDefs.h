#pragma once
// only one of these echo cancelers may be defined at time
// uncomment the define of the echo canceler desired to use
// # define USE_SPEEX_ECHO_CANCEL
// # define USE_WEB_RTC_ECHO_CANCEL_1
// # define USE_WEB_RTC_ECHO_CANCEL_3
# define USE_WEB_RTC_ECHO_CANCEL_MOBILE

#if defined( USE_SPEEX_ECHO_CANCEL )
constexpr int ECHO_SAMPLE_RATE = 8000;  
constexpr int AUDIO_DEVICE_SAMPLE_RATE = 48000;

#elif defined( USE_WEB_RTC_ECHO_CANCEL_1 )
constexpr int ECHO_SAMPLE_RATE = 8000;
constexpr int AUDIO_DEVICE_SAMPLE_RATE = 48000;

#elif defined( USE_WEB_RTC_ECHO_CANCEL_3 )
constexpr int ECHO_SAMPLE_RATE = 8000;
constexpr int AUDIO_DEVICE_SAMPLE_RATE = 48000;

#elif defined( USE_WEB_RTC_ECHO_CANCEL_MOBILE )
constexpr int ECHO_SAMPLE_RATE = 16000;
constexpr int AUDIO_DEVICE_SAMPLE_RATE = 16000;

#endif // defined( USE_SPEEX_ECHO_CANCEL )

constexpr int AUDIO_CHANNELS = 1;	
constexpr int AUDIO_BYTES_PER_SAMPLE = 2;			// PCM 2 bytes per sample

// internal to application a frame is 60 ms
constexpr int AUDIO_MS_PER_FRAME = 60;               // 60 ms = 0.06 sec of audio data (required to be a multiple of 960 samples per frame to play nice with opus)
constexpr int AUDIO_SAMPLES_PER_FRAME = (ECHO_SAMPLE_RATE * AUDIO_CHANNELS * AUDIO_MS_PER_FRAME) / 1000;	
constexpr int AUDIO_BUF_SIZE = AUDIO_SAMPLES_PER_FRAME * AUDIO_BYTES_PER_SAMPLE; 

constexpr int PLAYER_CACHE_FRAMES_CNT = 4; // how many frames of audio to cache for player

constexpr int AUDIO_FRAME_TO_DEVICE_RATE_MULTIPLIER = AUDIO_DEVICE_SAMPLE_RATE / ECHO_SAMPLE_RATE;

constexpr double AUDIO_BYTES_TO_MS_MULTIPLIER = ((double)AUDIO_MS_PER_FRAME / (double)AUDIO_BUF_SIZE);

// echo canceler can only handle 10 ms at a time
constexpr int ECHO_MS_PER_FRAME = 10; 
constexpr int ECHO_SAMPLES_PER_FRAME = AUDIO_SAMPLES_PER_FRAME / ( AUDIO_MS_PER_FRAME / ECHO_MS_PER_FRAME );
constexpr int ECHO_BUF_SIZE = AUDIO_BUF_SIZE / (AUDIO_MS_PER_FRAME / ECHO_MS_PER_FRAME);

// qt application mixer
constexpr int MAX_GUI_MIXER_FRAMES = 2;
// ptop mixer
constexpr int MAX_PTOP_MIXER_FRAMES = 2;

// kodi and player-nlc output (float) 960 frames 
constexpr int AUDIO_SAMPLE_RATE_KODI = 48000;        // kodi is configured for 48000 hz
constexpr int AUDIO_CHANNELS_KODI = 2;               // kodi is configured for stereo
constexpr int AUDIO_MS_KODI = 20;                    // 20 ms = 0.02 sec of audio data
constexpr float AUDIO_SEC_KODI = 0.02;               // 20 ms = 0.02 sec of audio data
constexpr int AUDIO_BYTES_PER_SAMPLE_KODI = 4;       // 4 bytes per sample (sizeof float)
// size of kodi frame in float input stream
constexpr int AUDIO_FRAME_SIZE_KODI = (int)((AUDIO_SAMPLE_RATE_KODI * ((float)AUDIO_MS_KODI/1000)) * AUDIO_BYTES_PER_SAMPLE_KODI * AUDIO_CHANNELS_KODI);   

constexpr int AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO = (AUDIO_SAMPLE_RATE_KODI / ECHO_SAMPLE_RATE) * (AUDIO_CHANNELS_KODI / AUDIO_CHANNELS);

enum EAudioTestState
{
	eAudioTestStateNone,
	eAudioTestStateInit,
	eAudioTestStateRun,
	eAudioTestStateResult,
	eAudioTestStateDone,

	eMaxAudioTestState
};