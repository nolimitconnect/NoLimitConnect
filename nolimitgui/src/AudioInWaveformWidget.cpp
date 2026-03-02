//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioInWaveformWidget.h"

//============================================================================
AudioInWaveformWidget::AudioInWaveformWidget(QWidget *parent) 
: QFrame(parent) 
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMinimumHeight(150);
    
    // Update at 60 FPS for smooth scrolling
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, QOverload<>::of(&AudioInWaveformWidget::update));
}

//============================================================================
std::vector<AudioFrameAec> AudioInWaveformWidget::getAudioInRawHistory()
{
    // Implementation: Return buffer from Mic Raw
    return GetAppInstance().getAudioMgr().getAudioInRawWaveformBuffer().getHistory();
}

//============================================================================
std::vector<AudioFrameAec> AudioInWaveformWidget::getAudioInAecProcessedHistory()
{
    // Implementation: Return buffer from AEC Processed
    return GetAppInstance().getAudioMgr().getAudioAecProcessedWaveformBuffer().getHistory();
}

//============================================================================
void AudioInWaveformWidget::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);
    GetAppInstance().getAudioMgr().wantAudioInVisualization( true );
    if (m_refreshTimer && !m_refreshTimer->isActive()) {
        m_refreshTimer->start(16);
    }
}

//============================================================================
void AudioInWaveformWidget::hideEvent(QHideEvent *event)
{
    QFrame::hideEvent(event);
    GetAppInstance().getAudioMgr().wantAudioInVisualization( false );
    if (m_refreshTimer && m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
    }
}

//============================================================================
void AudioInWaveformWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);

    auto inFrames = getAudioInRawHistory();
    auto outFrames = getAudioInAecProcessedHistory();

    if (inFrames.empty()) return;

    int sectionHeightTop = height() / 3;
    int sectionHeightMid = height() / 3;
    int sectionHeightBottom = height() - sectionHeightTop - sectionHeightMid;
    
    // 1. Draw Raw Mic (Red)
    drawWaveform(painter, inFrames, 0, sectionHeightTop, Qt::red, "MIC RAW");
    
    // 2. Draw Processed Output (Green)
    drawWaveform(painter, outFrames, sectionHeightTop, sectionHeightMid, Qt::green, "AEC PROCESSED");
    
    // 3. Draw Difference/Leakage (Yellow)
    drawDifference(painter, inFrames, outFrames, sectionHeightTop + sectionHeightMid, sectionHeightBottom, Qt::yellow, "ECHO LEAKAGE");
}

//============================================================================
void AudioInWaveformWidget::drawWaveform(QPainter &p, const std::vector<AudioFrameAec> &frames, int yOffset, int h, QColor color, QString label) {
    if (frames.empty() || h <= 0 || width() <= 1) {
        return;
    }

    p.setPen(color);
    int labelHeight = 18;
    int drawableTop = yOffset + labelHeight;
    int drawableHeight = h - labelHeight;
    if (drawableHeight <= 0) {
        return;
    }
    int midY = drawableTop + (drawableHeight / 2);
    
    // Draw Label
    p.drawText(10, yOffset + 15, label);

    size_t totalSamples = 0;
    for (const auto& frame : frames) {
        totalSamples += frame.samples.size();
    }
    if (totalSamples == 0) {
        return;
    }

    float xStep = (totalSamples > 1) ? static_cast<float>(width() - 1) / static_cast<float>(totalSamples - 1) : 0.0f;
    float yScale = static_cast<float>(drawableHeight / 2) / 32768.0f;
    float currentX = 0;

    QPainterPath path;
    path.moveTo(0, midY);

    for (const auto& frame : frames) {
        for (int16_t sample : frame.samples) {
            float scaledSample = static_cast<float>(sample) * yScale;
            path.lineTo(currentX, midY - scaledSample);
            currentX += xStep;
        }
    }
    p.drawPath(path);
}

//============================================================================
void AudioInWaveformWidget::drawDifference(QPainter &p, const std::vector<AudioFrameAec> &in, const std::vector<AudioFrameAec> &out, int yOffset, int h, QColor color, QString label) {
    if (h <= 0 || width() <= 1) {
        return;
    }

    p.setPen(color);
    int labelHeight = 18;
    int drawableTop = yOffset + labelHeight;
    int drawableHeight = h - labelHeight;
    if (drawableHeight <= 0) {
        return;
    }
    int midY = drawableTop + (drawableHeight / 2);
    p.drawText(10, yOffset + 15, label);

    if (in.size() != out.size()) return;

    size_t totalSamples = 0;
    for (size_t f = 0; f < in.size(); ++f) {
        totalSamples += std::min(in[f].samples.size(), out[f].samples.size());
    }
    if (totalSamples == 0) {
        return;
    }

    float xStep = (totalSamples > 1) ? static_cast<float>(width() - 1) / static_cast<float>(totalSamples - 1) : 0.0f;
    float yScale = static_cast<float>(drawableHeight / 2) / 32768.0f;
    float currentX = 0;
    QPainterPath path;
    path.moveTo(0, midY);

    for (size_t f = 0; f < in.size(); ++f) {
        size_t sampleCount = std::min(in[f].samples.size(), out[f].samples.size());
        for (size_t s = 0; s < sampleCount; ++s) {
            int diffSample = static_cast<int>(in[f].samples[s]) - static_cast<int>(out[f].samples[s]);
            float scaledDiff = static_cast<float>(diffSample) * yScale;
            path.lineTo(currentX, midY - scaledDiff);
            currentX += xStep;
        }
    }
    p.drawPath(path);
}