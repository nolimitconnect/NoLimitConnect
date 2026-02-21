#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include "api/scoped_refptr.h"

namespace webrtc {
class AudioProcessing;
class Environment;
}

// Use the bundled shim's simple interface
class IAudioProcessor {
public:
    virtual ~IAudioProcessor() = default;
    virtual void processCapture(int16_t* data, int frames) = 0;
    virtual void processRender(int16_t* data, int frames) = 0;
};

class AECWebRTC : public IAudioProcessor
{
public:
    AECWebRTC();
    ~AECWebRTC();

    void setStreamFormat(int sample_rate_hz, size_t channels);

    void processCapture(int16_t* data, int frames) override;
    void processRender(int16_t* data, int frames) override;

    float lastVadProbability() const { return m_lastVadProbability; }
    float lastEchoReturnLoss() const { return m_lastEchoReturnLoss; }

private:
    std::unique_ptr<webrtc::Environment> m_env;
    webrtc::scoped_refptr<webrtc::AudioProcessing> m_apm;
    int m_sampleRateHz{0};
    size_t m_channels{0};
    size_t m_frameSize{0};
    float m_lastVadProbability{0.0f};
    float m_lastEchoReturnLoss{0.0f};
    std::vector<int16_t> m_renderScratch;
    mutable std::mutex m_mutex;
};
