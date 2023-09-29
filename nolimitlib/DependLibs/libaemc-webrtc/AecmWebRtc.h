
#ifndef WEBRTC_SYSTEM_WRAPPERS_INTERFACE_AECM_WEBRTC_H_
#define WEBRTC_SYSTEM_WRAPPERS_INTERFACE_AECM_WEBRTC_H_

#include <stdint.h>
#include <vector>

// wrapper of mobile version of web rct echo cancel
class AecmWebRtc
{
public:
    AecmWebRtc();
    ~AecmWebRtc();

    void            setEchoDelay( int delayMs )                 { m_AecmDelayMs = delayMs; } // 40 to 500 milliseconds
    int             getEchoDelay( void )                        { return m_AecmDelayMs; }

    bool            initializeAecm( int sampFreq, int echoMode = 3 ); // echo mode 0-4 controls noise supression gain. may need to boost volume if set to high echoMode
    bool            resetAecm( void ); // reset aecm using previous initialize values

    bool            processEchoCancel( int16_t* nearendNoisyMic, int16_t* farSpeakerOut, int nrOfSamples, int16_t* retEchoCanceledSamples );


protected:
    static void*    createAecmInstance( void );
    static int      freeAecmInstance( void* aecmHandler );
    static int      initializeAecmInstance( void* aecmHandler, int sampFreq );
    static int      aecmSetConfig( void* aecmHandler, short echoMode, short cngMode );

    static int      aecmBufferFarend( void* aecmHandler, int16_t* farend, int nrOfSamples );
    static int      aecmProcess( void* aecmHandler, int16_t* retCleanedSamples, int16_t* nearendNoisy, int16_t* nearendClean, int nrOfSamples, int msInSndCardBuf );

    void*           m_AecmHandler{ nullptr };
    int             m_AecmDelayMs{ 100 };
    int             m_SampleFreq{ 0 };
    int             m_EchoMode{ 3 };
    int             m_EchoChunkSampleCnt{ 0 };
}; 

#endif // WEBRTC_SYSTEM_WRAPPERS_INTERFACE_AECM_WEBRTC_H_