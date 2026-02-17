#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <opus/include/opus.h>
#include <vector>
#include <stdint.h>

class OpusCodec
{
public:
    OpusCodec(int sampleRate, int channels);
    ~OpusCodec();

    std::vector<unsigned char> encode(const int16_t* pcm, int frames);

    std::vector<int16_t> decode(const uint8_t* data, size_t len);

    std::vector<int16_t> decode(const std::vector<unsigned char>& data)
    {
        return decode(data.data(), data.size());
    }

private:
    OpusEncoder* encoder;
    OpusDecoder* decoder;
};
