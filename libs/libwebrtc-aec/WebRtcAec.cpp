#include "AECWebRTC.h"

#include <cstring>

#include "api/audio/audio_processing.h"
#include "api/audio/builtin_audio_processing_builder.h"
#include "api/environment/environment_factory.h"

AECWebRTC::AECWebRTC()
    : m_env(std::make_unique<webrtc::Environment>(webrtc::CreateEnvironment()))
{
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
    m_apm = builder.Build(*m_env);
    if (m_apm) {
        m_apm->ApplyConfig(config);
    }
}

AECWebRTC::~AECWebRTC() = default;

void AECWebRTC::setStreamFormat(int sample_rate_hz, size_t channels)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_apm || sample_rate_hz <= 0 || channels == 0) {
        return;
    }

    if (sample_rate_hz == m_sampleRateHz && channels == m_channels) {
        return;
    }

    m_sampleRateHz = sample_rate_hz;
    m_channels = channels;
    m_frameSize = static_cast<size_t>(
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

    m_apm->Initialize(config);
}

void AECWebRTC::processCapture(int16_t* data, int frames)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_apm || !data || frames <= 0 || m_sampleRateHz <= 0 || m_channels == 0) {
        return;
    }

    const size_t frameCount = static_cast<size_t>(frames);
    if (m_frameSize != 0 && frameCount != m_frameSize) {
        return;
    }

    webrtc::StreamConfig stream_config(m_sampleRateHz, m_channels);
    m_apm->set_stream_delay_ms(0);
    m_apm->ProcessStream(data, stream_config, stream_config, data);

    const webrtc::AudioProcessingStats stats = m_apm->GetStatistics(true);
    m_lastEchoReturnLoss = stats.echo_return_loss.has_value()
        ? static_cast<float>(*stats.echo_return_loss)
        : 0.0f;
    m_lastVadProbability = stats.voice_detected.has_value()
        ? (*stats.voice_detected ? 1.0f : 0.0f)
        : 0.0f;
}

void AECWebRTC::processRender(int16_t* data, int frames)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_apm || !data || frames <= 0 || m_sampleRateHz <= 0 || m_channels == 0) {
        return;
    }

    const size_t frameCount = static_cast<size_t>(frames);
    if (m_frameSize != 0 && frameCount != m_frameSize) {
        return;
    }

    const size_t sampleCount = frameCount * m_channels;
    if (m_renderScratch.size() < sampleCount) {
        m_renderScratch.resize(sampleCount);
    }

    std::memcpy(m_renderScratch.data(), data, sampleCount * sizeof(int16_t));

    webrtc::StreamConfig stream_config(m_sampleRateHz, m_channels);
    m_apm->ProcessReverseStream(m_renderScratch.data(), stream_config,
                                stream_config, m_renderScratch.data());
}
