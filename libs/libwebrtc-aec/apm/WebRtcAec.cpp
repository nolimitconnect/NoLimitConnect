#include "WebRtcAec.h"

#include <cstring>
#include <mutex>
#include <vector>

#include "api/audio/audio_processing.h"
#include "api/audio/builtin_audio_processing_builder.h"
#include "api/environment/environment_factory.h"
#include "api/scoped_refptr.h"

class WebRtcAec::Impl {
public:
    std::unique_ptr<webrtc::Environment> env;
    webrtc::scoped_refptr<webrtc::AudioProcessing> apm;
    int sampleRateHz{0};
    size_t channels{0};
    size_t frameSize{0};
    float lastVadProbability{0.0f};
    float lastEchoReturnLoss{0.0f};
    std::vector<int16_t> renderScratch;
    mutable std::mutex mutex;
};

WebRtcAec::WebRtcAec()
    : m_impl(std::make_unique<Impl>())
{
    m_impl->env = std::make_unique<webrtc::Environment>(webrtc::CreateEnvironment());

    webrtc::AudioProcessing::Config config;
    config.echo_canceller.enabled = true;
    config.echo_canceller.mobile_mode = false;
    config.high_pass_filter.enabled = true;
    config.noise_suppression.enabled = true;
    config.noise_suppression.level =
        webrtc::AudioProcessing::Config::NoiseSuppression::kModerate;
    config.gain_controller1.enabled = false;
    config.gain_controller2.enabled = true;
    config.gain_controller2.adaptive_digital.enabled = true;

    webrtc::BuiltinAudioProcessingBuilder builder(config);
    m_impl->apm = builder.Build(*m_impl->env);
    if (m_impl->apm) {
        m_impl->apm->ApplyConfig(config);
    }
}

WebRtcAec::~WebRtcAec() = default;

float WebRtcAec::lastVadProbability() const
{
    std::lock_guard<std::mutex> lock(m_impl->mutex);
    return m_impl->lastVadProbability;
}

float WebRtcAec::lastEchoReturnLoss() const
{
    std::lock_guard<std::mutex> lock(m_impl->mutex);
    return m_impl->lastEchoReturnLoss;
}

void WebRtcAec::setStreamFormat(int sample_rate_hz, size_t channels)
{
    std::lock_guard<std::mutex> lock(m_impl->mutex);
    if (!m_impl->apm || sample_rate_hz <= 0 || channels == 0) {
        return;
    }

    if (sample_rate_hz == m_impl->sampleRateHz && channels == m_impl->channels) {
        return;
    }

    m_impl->sampleRateHz = sample_rate_hz;
    m_impl->channels = channels;
    m_impl->frameSize = static_cast<size_t>(
        webrtc::AudioProcessing::GetFrameSize(sample_rate_hz));

    webrtc::ProcessingConfig config;
    config.input_stream().set_sample_rate_hz(sample_rate_hz);
    config.input_stream().set_num_channels(channels);
    config.output_stream().set_sample_rate_hz(sample_rate_hz);
    config.output_stream().set_num_channels(channels);
    config.reverse_input_stream().set_sample_rate_hz(sample_rate_hz);
    config.reverse_input_stream().set_num_channels(channels);
    config.reverse_output_stream().set_sample_rate_hz(sample_rate_hz);
    config.reverse_output_stream().set_num_channels(channels);

    m_impl->apm->Initialize(config);
}

void WebRtcAec::processCapture(int16_t* data, int frames)
{
    std::lock_guard<std::mutex> lock(m_impl->mutex);
    if (!m_impl->apm || !data || frames <= 0 || m_impl->sampleRateHz <= 0 || m_impl->channels == 0) {
        return;
    }

    const size_t frameCount = static_cast<size_t>(frames);
    if (m_impl->frameSize != 0 && frameCount != m_impl->frameSize) {
        return;
    }

    webrtc::StreamConfig stream_config(m_impl->sampleRateHz, m_impl->channels);
    m_impl->apm->set_stream_delay_ms(0);
    m_impl->apm->ProcessStream(data, stream_config, stream_config, data);

    const webrtc::AudioProcessingStats stats = m_impl->apm->GetStatistics(true);
    m_impl->lastEchoReturnLoss = stats.echo_return_loss.has_value()
        ? static_cast<float>(*stats.echo_return_loss)
        : 0.0f;
    double vadEstimate = stats.residual_echo_likelihood.value_or(0.0);
    if (vadEstimate < 0.0) {
        vadEstimate = 0.0;
    } else if (vadEstimate > 1.0) {
        vadEstimate = 1.0;
    }
    m_impl->lastVadProbability = static_cast<float>(vadEstimate);
}

void WebRtcAec::processRender(int16_t* data, int frames)
{
    std::lock_guard<std::mutex> lock(m_impl->mutex);
    if (!m_impl->apm || !data || frames <= 0 || m_impl->sampleRateHz <= 0 || m_impl->channels == 0) {
        return;
    }

    const size_t frameCount = static_cast<size_t>(frames);
    if (m_impl->frameSize != 0 && frameCount != m_impl->frameSize) {
        return;
    }

    const size_t sampleCount = frameCount * m_impl->channels;
    if (m_impl->renderScratch.size() < sampleCount) {
        m_impl->renderScratch.resize(sampleCount);
    }

    std::memcpy(m_impl->renderScratch.data(), data, sampleCount * sizeof(int16_t));

    webrtc::StreamConfig stream_config(m_impl->sampleRateHz, m_impl->channels);
    m_impl->apm->ProcessReverseStream(m_impl->renderScratch.data(), stream_config,
                                      stream_config, m_impl->renderScratch.data());
}
