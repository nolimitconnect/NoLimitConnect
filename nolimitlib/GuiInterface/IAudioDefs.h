#pragma once
// only one of these echo cancelers may be defined at time
// uncomment the define of the echo canceler desired to use
// # define USE_SPEEX_ECHO_CANCEL
// # define USE_WEB_RTC_ECHO_CANCEL_1
// # define USE_WEB_RTC_ECHO_CANCEL_3
# define USE_WEB_RTC_ECHO_CANCEL_MOBILE

#if defined( USE_SPEEX_ECHO_CANCEL )
static const int ECHO_SAMPLE_RATE = 8000;  
static const int AUDIO_DEVICE_SAMPLE_RATE = 48000;

#elif defined( USE_WEB_RTC_ECHO_CANCEL_1 )
static const int ECHO_SAMPLE_RATE = 8000;
static const int AUDIO_DEVICE_SAMPLE_RATE = 48000;

#elif defined( USE_WEB_RTC_ECHO_CANCEL_3 )
static const int ECHO_SAMPLE_RATE = 8000;
static const int AUDIO_DEVICE_SAMPLE_RATE = 48000;

#elif defined( USE_WEB_RTC_ECHO_CANCEL_MOBILE )
static const int ECHO_SAMPLE_RATE = 16000;
static const int AUDIO_DEVICE_SAMPLE_RATE = 16000;

#endif // defined( USE_SPEEX_ECHO_CANCEL )

static const int AUDIO_CHANNELS = 1;	
static const int AUDIO_BYTES_PER_SAMPLE = 2;			// PCM 2 bytes per sample

// internal to application a frame is 60 ms
static const int AUDIO_MS_PER_FRAME = 60;               // 60 ms = 0.06 sec of audio data (required to be a multiple 960 samples per frame to play nice with opus)
static const int AUDIO_SAMPLES_PER_FRAME = (ECHO_SAMPLE_RATE * AUDIO_CHANNELS * AUDIO_MS_PER_FRAME) / 1000;	
static const int AUDIO_BUF_SIZE = AUDIO_SAMPLES_PER_FRAME * AUDIO_BYTES_PER_SAMPLE; 

static const int AUDIO_FRAME_TO_DEVICE_RATE_MULTIPLIER = AUDIO_DEVICE_SAMPLE_RATE / ECHO_SAMPLE_RATE;

static const double AUDIO_BYTES_TO_MS_MULTIPLIER = ((double)AUDIO_MS_PER_FRAME / (double)AUDIO_BUF_SIZE);

// echo canceler can only handle 10 ms at a time
static const int ECHO_MS_PER_FRAME = 10; 
static const int ECHO_SAMPLES_PER_FRAME = AUDIO_SAMPLES_PER_FRAME / ( AUDIO_MS_PER_FRAME / ECHO_MS_PER_FRAME );
static const int ECHO_BUF_SIZE = AUDIO_BUF_SIZE / (AUDIO_MS_PER_FRAME / ECHO_MS_PER_FRAME);

// qt application mixer
static const int MAX_GUI_MIXER_FRAMES = 2;
// ptop mixer
static const int MAX_PTOP_MIXER_FRAMES = 2;

// kodi and player-nlc output (float) 960 frames 
static const int AUDIO_SAMPLE_RATE_KODI = 48000;        // kodi is configured for 48000 hz
static const int AUDIO_CHANNELS_KODI = 2;               // kodi is configured for stereo
static const int AUDIO_MS_KODI = 20;                    // 20 ms = 0.02 sec of audio data
static const float AUDIO_SEC_KODI = 0.02;               // 20 ms = 0.02 sec of audio data
static const int AUDIO_BYTES_PER_SAMPLE_KODI = 4;       // 4 bytes per sample (sizeof float)
// size of kodi frame in float input stream
static const int AUDIO_FRAME_SIZE_KODI = (int)((AUDIO_SAMPLE_RATE_KODI * ((float)AUDIO_MS_KODI/1000)) * AUDIO_BYTES_PER_SAMPLE_KODI * AUDIO_CHANNELS_KODI);   
// after down sample to echo cancel rate the number of samples in cache for kodi
static const int AUDIO_CACHE_PCM_SAMPLES_KODI = AUDIO_SAMPLES_PER_FRAME * 2;
static const int AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO = (AUDIO_SAMPLE_RATE_KODI / ECHO_SAMPLE_RATE) * (AUDIO_CHANNELS_KODI / AUDIO_CHANNELS);

enum EAudioTestState
{
	eAudioTestStateNone,
	eAudioTestStateInit,
	eAudioTestStateRun,
	eAudioTestStateResult,
	eAudioTestStateDone,

	eMaxAudioTestState
};