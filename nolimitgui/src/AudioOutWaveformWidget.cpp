//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioOutWaveformWidget.h"

//============================================================================
AudioOutWaveformWidget::AudioOutWaveformWidget(QWidget *parent) : QFrame(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMinimumHeight(150);
    
    // Update at 60 FPS for smooth scrolling
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, QOverload<>::of(&AudioOutWaveformWidget::update));
}

//============================================================================
std::vector<AudioFrameAec> AudioOutWaveformWidget::getAudioSpeakerHistory()
{
    return GetAppInstance().getAudioMgr().getSpeakerOutWaveformBuffer().getHistory();
}

//============================================================================
void AudioOutWaveformWidget::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);
    GetAppInstance().getAudioMgr().wantAudioOutVisualization( true );
    if (m_refreshTimer && !m_refreshTimer->isActive()) {
        m_refreshTimer->start(16);
    }
}

//============================================================================
void AudioOutWaveformWidget::hideEvent(QHideEvent *event)
{
    QFrame::hideEvent(event);
    GetAppInstance().getAudioMgr().wantAudioOutVisualization( false );
    if (m_refreshTimer && m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
    }
}

//============================================================================
void AudioOutWaveformWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);

    auto inFrames = getAudioSpeakerHistory();

    if (inFrames.empty()) return;

    int sectionHeight = height();
    
    // 1. Draw Raw Speaker out (Red)
    drawWaveform(painter, inFrames, 0, sectionHeight, Qt::red, "SPEAKER OUT");

}

//============================================================================
void AudioOutWaveformWidget::drawWaveform(QPainter &p, const std::vector<AudioFrameAec> &frames, int yOffset, int h, QColor color, QString label) {
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