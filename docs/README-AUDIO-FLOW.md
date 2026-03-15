# Audio Flow (Operator Guide)

## Purpose

Quick reference for how voice is captured, encoded, sent to peers, decoded, mixed, and played.
Includes focused checkpoints for failures after migrating codec internals.

## Core Components

- AudioMgr
	Captures microphone input and submits 60 ms PCM frames into MediaProcessor.

- MediaProcessor
	Encodes PCM to Opus for outbound transport and routes audio to registered audio clients.

- VoiceFeedMgr / PushToTalkFeedMgr
	Network-facing plugin feed managers that send Opus packets and decode incoming Opus packets.

- AudioMixerMgr
	Pulls decoded remote PCM from MediaProcessor and pushes mixed audio to speaker hardware.

- OpusCodec
	Shared encoder/decoder wrapper currently located at libs/ffmpeg/opus/OpusCodec.*.

## Flow A: Microphone Capture to Opus Packet TX

```text
Audio input callback (10 ms AEC frames)
	-> AudioMgr::callbackAudioIn... accumulates 6 x 10 ms
	-> AudioMgr::callbackAudioIn60msFrameAvail
	-> MediaProcessor::fromGuiEchoCanceledSamplesThreaded
	-> MediaProcessor::processAudioInThreaded
	-> MediaProcessor::processRawAudioIn
			-> OpusCodec::encode
			-> PktVoiceReq payload fill
			-> callbackOpusPkt on audio packet clients
	-> Plugin*::callbackOpusPkt
	-> VoiceFeedMgr/PushToTalkFeedMgr::callbackOpusPkt
	-> txPacket to peer socket
```

Code links:

- [nolimitgui/src/AudioMgrIn.cpp](nolimitgui/src/AudioMgrIn.cpp)
- [libs/libptopengine/MediaProcessor/MediaProcessor.cpp](libs/libptopengine/MediaProcessor/MediaProcessor.cpp)
- [libs/libptopengine/Plugins/VoiceFeedMgr.cpp](libs/libptopengine/Plugins/VoiceFeedMgr.cpp)
- [libs/libptopengine/Plugins/PushToTalkFeedMgr.cpp](libs/libptopengine/Plugins/PushToTalkFeedMgr.cpp)
- [libs/ffmpeg/opus/OpusCodec.cpp](libs/ffmpeg/opus/OpusCodec.cpp)
- [libs/PktLib/PktVoiceReq.h](libs/PktLib/PktVoiceReq.h)

## Flow B: RX Opus Packet to Decoded PCM

```text
Peer sends PktVoiceReq
	-> Plugin onPktVoiceReq
	-> VoiceFeedMgr/PushToTalkFeedMgr::onPktVoiceReq
	-> session OpusCodec::decode into jitter buffer
	-> callbackAudioOutSpaceAvail when speaker path requests data
	-> MediaProcessor::playAudio (mix accumulator)
	-> MediaProcessor::processFriendAudioFeed (fan-out PCM clients)
```

Code links:

- [libs/libptopengine/Plugins/VoiceFeedMgr.cpp](libs/libptopengine/Plugins/VoiceFeedMgr.cpp)
- [libs/libptopengine/Plugins/PushToTalkFeedMgr.cpp](libs/libptopengine/Plugins/PushToTalkFeedMgr.cpp)
- [libs/libptopengine/MediaProcessor/MediaProcessor.cpp](libs/libptopengine/MediaProcessor/MediaProcessor.cpp)

## Flow C: Speaker Pull and Mixer Loop

```text
AudioMgr speaker callback requests samples
	-> AudioMgr::callbackReadSpeakerData
	-> AudioMgr::processQueuedAudioOutput (10 ms chunks for AEC state)
	-> AudioMixerMgr::callbackAudioOut60msSpaceAvail
			-> mix module buffers
			-> write speaker hardware
			-> AudioMixerMgr::fromGuiAudioOutSpaceAvaiThreaded
	-> MediaProcessor::fromGuiAudioOutSpaceAvaiThreaded
			-> callbackAudioOutSpaceAvail for mixer clients
```

Code links:

- [nolimitgui/src/AudioMgrOut.cpp](nolimitgui/src/AudioMgrOut.cpp)
- [nolimitgui/src/AudioMixerMgr.cpp](nolimitgui/src/AudioMixerMgr.cpp)
- [libs/libptopengine/MediaProcessor/MediaProcessor.cpp](libs/libptopengine/MediaProcessor/MediaProcessor.cpp)

## Subscription and Device Enable Logic

- Plugins request audio capture/playback using pluginApiWantMediaInput.
- MediaProcessor tracks clients in m_AudioPcmList, m_AudioOpusList, m_AudioPktsList, and m_MixerList.
- Mic capture starts when first audio input client is added and stops when all are removed.
- Speaker output starts/stops similarly via mixer client counts.

Code links:

- [libs/libptopengine/Plugins/PluginBase.cpp](libs/libptopengine/Plugins/PluginBase.cpp)
- [libs/libptopengine/Plugins/PluginMgr.cpp](libs/libptopengine/Plugins/PluginMgr.cpp)
- [libs/libptopengine/MediaProcessor/MediaProcessor.cpp](libs/libptopengine/MediaProcessor/MediaProcessor.cpp)
- [nolimitgui/src/AudioMgrRequests.cpp](nolimitgui/src/AudioMgrRequests.cpp)
- [nolimitgui/src/AppCommonAudio.cpp](nolimitgui/src/AppCommonAudio.cpp)

## Timing and Frame Contracts

- Audio sample rate: 16000 Hz, mono.
- PCM frame size in this pipeline: 60 ms.
- AUDIO_SAMPLES_PER_FRAME at 16 kHz: 960 samples.
- ECHO frame size: 10 ms chunks (160 samples).
- Opus encode/decode expected frame samples: 960.

Code links:

- [libs/GuiInterface/IAudioDefs.h](libs/GuiInterface/IAudioDefs.h)
- [libs/ffmpeg/opus/OpusCodec.cpp](libs/ffmpeg/opus/OpusCodec.cpp)

## ffmpegopus Migration Checkpoints

If audio broke after codec updates, validate these first:

1. Frame-size agreement
	 - encode input sampleCnt must be 960 (60 ms at 16 kHz)
	 - decode output expectation is AUDIO_SAMPLES_PER_FRAME

2. Sample rate / channels consistency
	 - OpusCodec constructor uses AUDIO_DEVICE_SAMPLE_RATE and AUDIO_CHANNELS
	 - sender and receiver must match (16 kHz, mono)

3. Encoder return handling
	 - MediaProcessor drops frame if encode returns <= 0
	 - check logs for Opus encode failed

4. Decoder return handling
	 - feed managers expect decodedSamples == AUDIO_SAMPLES_PER_FRAME
	 - mismatch logs as decode failure and can starve playback

5. Media subscription presence
	 - without eMediaInputAudioPkts client, mic capture can appear idle
	 - without eMediaInputMixer client, decoded audio may not reach speaker path

6. Packet payload integrity
	 - PktVoiceReq compressed data length must be set before calcPktLen
	 - receiver decode uses getCompressedDataLen from packet

## Practical Debug Checklist

1. Confirm plugin requested microphone input (AudioPkts) and mixer input.
2. Confirm AudioMgr is producing 60 ms frames.
3. Confirm MediaProcessor encode succeeds and callbackOpusPkt is called.
4. Confirm peer onPktVoiceReq decode returns 960 samples.
5. Confirm callbackAudioOutSpaceAvail is active and MediaProcessor::playAudio is reached.
6. Confirm AudioMgr speaker callback is draining buffers (no persistent underflow warnings).
