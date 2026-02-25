#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

// Use the bundled shim's simple interface
class IAudioProcessor {
public:
    virtual ~IAudioProcessor() = default;
    virtual void processCapture(int16_t* data, int frames) = 0;
    virtual void processRender(int16_t* data, int frames) = 0;
};

class WebRtcAec : public IAudioProcessor
{
public:
    WebRtcAec();
    ~WebRtcAec();

    void setStreamFormat(int sample_rate_hz, size_t channels);

    void processCapture(int16_t* data, int frames) override;
    void processRender(int16_t* data, int frames) override;

    float lastVadProbability() const;
    float lastEchoReturnLoss() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};
