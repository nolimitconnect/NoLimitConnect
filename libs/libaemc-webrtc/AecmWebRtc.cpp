
#include "AecmWebRtc.h"
#include <GuiInterface/IAudioDefs.h>

#include <stdlib.h> // for NULL operator
#include <assert.h>
#include "echo_control_mobile.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AecmWebRtc::AecmWebRtc()
{
	//m_SampleFreq = ECHO_SAMPLE_RATE;
	// m_EchoChunkSampleCnt = m_SampleFreq == 8000 ? 80 : 160;
}

//============================================================================
AecmWebRtc::~AecmWebRtc()
{
	if( m_AecmHandler )
	{
		freeAecmInstance( m_AecmHandler );
		m_AecmHandler = nullptr;
	}
}

//============================================================================
bool AecmWebRtc::initializeAecm( int echoDelay, int sampFreq, int echoMode )
{
	if( !(sampFreq == 16000) )
	{
		LogMsg( LOG_ERROR, "AecmWebRtc::initializeAecm failed invalid sample frequency %d", sampFreq );
		return false;
	}

	m_AecmDelayMs = echoDelay;
	m_EchoMode = echoMode;
	m_SampleFreq = sampFreq;
	m_EchoChunkSampleCnt = 160;

	return resetAecm();
}

//============================================================================
bool AecmWebRtc::resetAecm( void )
{
	if( !m_SampleFreq )
	{
		LogMsg( LOG_ERROR, "AecmWebRtc::initializeAecm failed createAecmInstance sample frequency" );
		return false;
	}

	if( m_AecmHandler )
	{
		freeAecmInstance( m_AecmHandler );
		m_AecmHandler = nullptr;
	}

	m_AecmHandler = createAecmInstance();

	if( !m_AecmHandler )
	{
		LogMsg( LOG_ERROR, "AecmWebRtc::initializeAecm failed createAecmInstance" );
		return false;
	}

	if( initializeAecmInstance( m_AecmHandler, m_SampleFreq ) )
	{
		LogMsg( LOG_ERROR, "AecmWebRtc::initializeAecm failed initializeAecmInstance" );
		freeAecmInstance( m_AecmHandler );
		m_AecmHandler = nullptr;
		return false;
	}

	if( m_EchoMode != 3 ) // 3 is set by default
	{
		if( aecmSetConfig( m_AecmHandler, (short)m_EchoMode, AecmTrue ) )
		{
			LogMsg( LOG_ERROR, "AecmWebRtc::initializeAecm failed aecmSetConfig" );
			freeAecmInstance( m_AecmHandler );
			m_AecmHandler = nullptr;
			return false;
		}
	}

	return true;
}


/**
 * This function is a wrapper which wraps the WebRtcAecm_Create function in WebRtc echo_control_mobile.c
 * Allocates the memory needed by the AECM. The memory needs to be initialized
 * separately using the WebRtcAecm_Init() function.
 *
 * Returns:
 *         -1: error
 *         other values: created AECM instance handler.
 *
 */
void* AecmWebRtc::createAecmInstance( void )
{
	void *aecmInstHandler = nullptr;
	if (WebRtcAecm_Create(&aecmInstHandler) == -1)
		return nullptr;
	else
		return ((void*) aecmInstHandler);
}

/**
 * This is a wrapper wraps WebRtcAecm_Free function in echo_control_mobile.c
 * This function releases the memory allocated by WebRtcAecm_Create().
 *
 * Inputs:
 *         aecmHandler - handler of the AECM instance created by nativeCreateAecmInstance()
 *
 * Returns         0: OK
 *                 -1: error
 *
 */
int AecmWebRtc::freeAecmInstance( void* aecmHandler )
{
	void *aecmInst = (void *) aecmHandler;
	if (aecmInst == NULL)
		return -1;
	int ret = WebRtcAecm_Free(aecmInst);
	aecmInst = NULL;
	return ret;
}

/**
 * This wrapper wraps the WebRtcAecm_Init() function in WebRtc echo_control_mobile.c
 * Initializes an AECM instance.
 *
 * Inputs:
 *            aecmHandler     - Handler of AECM instance
 *            sampFreq        - Sampling frequency of data
 *
 * Return:          0: OK
 *                  -1: error
 *
 */
int AecmWebRtc::initializeAecmInstance(void* aecmHandler, int sampFreq)
{
	void *aecmInst = (void *) aecmHandler;
	if (aecmInst == NULL)
		return -1;
	return WebRtcAecm_Init(aecmInst, sampFreq);
}

/**
 * This wrapper wraps the WebRtcAecm_BufferFarend function in echo_control_mobile.c
 * Inserts an 80 or 160 sample block of data into the farend buffer.
 *
 * Inputs:
 *       aecmHandler    - Handler to the AECM instance
 *       farend               - In buffer containing one frame of farend signal for L band
 *       nrOfSamples    - Number of samples in farend buffer
 *
 * Return:     0: OK
 *             -1: error
 *
 */
int AecmWebRtc::aecmBufferFarend(void* aecmHandler, int16_t* farend, int nrOfSamples) 
{
	if( !aecmHandler )
	{
        // LOGD(TAG, "nativeBufferFarend() error: aecmInst == NULL");
        return -1;
    }

	int ret = -1;
	if( nrOfSamples )
	{
		ret = WebRtcAecm_BufferFarend( aecmHandler, farend, nrOfSamples);
	}

	return ret;
}

/**
 * This wrapper wraps the WebRtcAecm_Process in echo_control_mobile.c
 * Runs the AECM on an 80 or 160 sample blocks of data.
 *
 * Inputs:
 *         aecmHandler           - Handler to the AECM handler
 *         nearendNoisy          - In buffer containing one frame of reference nearend+echo signal.
 *                                             If noise reduction is active, provide the noisy signal here.
 *         nearendClean          -  In buffer containing one frame of nearend+echo signal.
 *                                             If noise reduction is active, provide the clean signal here.
 *                                             Otherwise pass a NULL pointer.
 *         nrOfSamples           - Number of samples in nearend buffer
 *         msInSndCardBuf    	 - Delay estimate for sound card and system buffers
 * Outputs:
 *         out    - Out buffer, one frame of processed nearend.
 * Return:     0: OK
 *             -1: error
 *
 */

int AecmWebRtc::aecmProcess( void* aecmHandler, int16_t* retEchoCanceledSamples, int16_t* nearendNoisy, int16_t* nearendClean, int nrOfSamples, int msInSndCardBuf )
{
    //LOGD(TAG, "nativeAecmProcess() nrOfSamples: %d; delay: %d", nrOfSamples, msInSndCardBuf);

	void *aecmInst = (void *) aecmHandler;
	if( !aecmHandler )
	{
		return -1;
	}

	int ret = -1;

	ret = WebRtcAecm_Process( aecmHandler, nearendNoisy, nearendClean, retEchoCanceledSamples, nrOfSamples, msInSndCardBuf );

	if (ret != 0) 
	{
		LogMsg( LOG_ERROR, "nativeAecmProcess() error ret: %d", ret);
		return -1;
	}


	return 0;
}

/**
 * This wrapper wraps the WebRtcAecm_set_config function in echo_control_mobile.c
 * Enables the user to set certain parameters on-the-fly.
 *
 * Inputs:
 *        aecHandler - Handler to the AEC instance.
 *        aecConfig - the new configuration of AEC instance to set.
 * Outputs:
 *         NONE
 * Return:     0: OK
 *             -1: error
 *
 */
int AecmWebRtc::aecmSetConfig( void* aecmHandler, short echoMode, short cngMode )
{

	void * aecmInst = (void *) aecmHandler;
	if (aecmInst == NULL)
		return -1;

	//set new configuration to AECM instance.
	AecmConfig config;
	config.echoMode = echoMode;
	config.cngMode = cngMode;

	return WebRtcAecm_set_config(aecmInst, config);
}

//============================================================================
bool AecmWebRtc::processEchoCancel( int16_t* nearendNoisyMic, int16_t* farSpeakerOut, int nrOfSamples, int16_t* retEchoCanceledSamples )
{
	int echoDelay = getEchoDelay();
	if( !m_AecmHandler || !retEchoCanceledSamples || !nearendNoisyMic || !farSpeakerOut || !nrOfSamples || !echoDelay )
	{
		if( !m_AecmHandler )
		{
			LogMsg( LOG_ERROR, "AecmWebRtc::processAecm NOT Initialized" );
		}
		else if( !echoDelay )
		{
			LogMsg( LOG_ERROR, "AecmWebRtc::processAecm Delay NOT SET" );
		}
		else
		{
			LogMsg( LOG_ERROR, "AecmWebRtc::processAecm Invalid Param" );
		}

		return false;
	}

	if( nrOfSamples % m_EchoChunkSampleCnt )
	{
		LogMsg( LOG_ERROR, "AecmWebRtc::processAecm number of samples %d is not a multiple of %d", nrOfSamples, m_EchoChunkSampleCnt );
		return false;
	}

	if( aecmBufferFarend( m_AecmHandler, farSpeakerOut, nrOfSamples ) )
	{
		LogMsg( LOG_ERROR, "AecmWebRtc::initializeAecm failed aecmBufferFarend" );
		freeAecmInstance( m_AecmHandler );
		m_AecmHandler = nullptr;
		return false;
	}

	if( aecmProcess( m_AecmHandler, retEchoCanceledSamples, nearendNoisyMic, nullptr, nrOfSamples, echoDelay ) )
	{
		LogMsg( LOG_ERROR, "AecmWebRtc::initializeAecm failed processAecm" );
		freeAecmInstance( m_AecmHandler );
		m_AecmHandler = nullptr;
		return false;
	}

	return true;
}

