#pragma once
#include <GuiInterface/IDefs.h>

class AudioMixerInterface
{
public:
	virtual int					toGuiAudioFrameThreaded( EAppModule appModule, int16_t* pcmData, bool isSilenceIn ) = 0;
};