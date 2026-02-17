//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "OpusCodec.h"
#include "AudioDefs.h"
#include <stdexcept>

//============================================================================
OpusCodec::OpusCodec(int sampleRate, int channels)
{
    int error;

    encoder = opus_encoder_create(sampleRate, channels, OPUS_APPLICATION_VOIP, &error);
    if (error != OPUS_OK)
        throw std::runtime_error("Failed to create Opus encoder");

    error = opus_encoder_ctl(encoder, OPUS_SET_VBR(0));
    if (error != OPUS_OK)
        throw std::runtime_error("Failed to disable Opus VBR");

    error = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(OPUS_FIXED_BITRATE_BPS));
    if (error != OPUS_OK)
        throw std::runtime_error("Failed to set Opus bitrate");

    decoder = opus_decoder_create(sampleRate, channels, &error);
    if (error != OPUS_OK)
        throw std::runtime_error("Failed to create Opus decoder");
}

//============================================================================
OpusCodec::~OpusCodec()
{
    opus_encoder_destroy(encoder);
    opus_decoder_destroy(decoder);
}

//============================================================================
std::vector<unsigned char> OpusCodec::encode(const int16_t* pcm, int frames)
{
    std::vector<unsigned char> out(OPUS_COMPRESSED_BYTES_PER_FRAME);

    int len = opus_encode(encoder, pcm, frames, out.data(), out.size());

    if (len < 0)
        throw std::runtime_error("Opus encode failed");

    out.resize(len);
    return out;
}

//============================================================================
std::vector<int16_t> OpusCodec::decode(const uint8_t* data, size_t len)
{
    std::vector<int16_t> out(AUDIO_SAMPLES_PER_FRAME);

    int frames = opus_decode(decoder, data, len, out.data(), out.size(), 0);

    if (frames < 0)
        throw std::runtime_error("Opus decode failed");

    out.resize(frames);
    return out;
}
