#pragma once

#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <vector>
#include "AudioFrameBuffer.h"

#include "MainWindow.h"

class AudioWaveformWidget : public QFrame {
    Q_OBJECT
public:
    explicit AudioWaveformWidget(QWidget *parent = nullptr) : QFrame(parent) {
        setBackgroundRole(QPalette::Base);
        setAutoFillBackground(true);
        setMinimumHeight(150);
        
        // Update at 60 FPS for smooth scrolling
        m_refreshTimer = new QTimer(this);
        connect(m_refreshTimer, &QTimer::timeout, this, QOverload<>::of(&AudioWaveformWidget::update));
        m_refreshTimer->start(16); 
    }

    std::vector<AudioFrame>     getAudioInHistory()
    {
        // Implementation: Return buffer from Mic Raw
        return getApp().getAudioMgr().getAudioInBuffer().getHistory();
    }

    std::vector<AudioFrame>     getAudioOutHistory()
    {
        // Implementation: Return buffer from AEC Processed
        return getApp().getAudioMgr().getAudioOutBuffer().getHistory();
    }

protected:
    void                        paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), Qt::black);

        auto inFrames = getAudioInHistory();
        auto outFrames = getAudioOutHistory();

        if (inFrames.empty()) return;

        int sectionHeight = height() / 3;
        
        // 1. Draw Raw Mic (Red)
        drawWaveform(painter, inFrames, 0, sectionHeight, Qt::red, "MIC RAW");
        
        // 2. Draw Processed Output (Green)
        drawWaveform(painter, outFrames, sectionHeight, sectionHeight, Qt::green, "AEC PROCESSED");
        
        // 3. Draw Difference/Leakage (Yellow)
        drawDifference(painter, inFrames, outFrames, sectionHeight * 2, sectionHeight, Qt::yellow, "ECHO LEAKAGE");
    }

private:
    QTimer* m_refreshTimer;

    void drawWaveform(QPainter &p, const std::vector<AudioFrame> &frames, int yOffset, int h, QColor color, QString label) {
        p.setPen(color);
        int midY = yOffset + (h / 2);
        
        // Draw Label
        p.drawText(10, yOffset + 15, label);

        // Simple line-based visualization
        float xStep = (float)width() / (frames.size() * 160); // 160 samples per frame
        float currentX = 0;

        QPainterPath path;
        path.moveTo(0, midY);

        for (const auto& frame : frames) {
            for (int16_t sample : frame.samples) {
                // Normalize 16-bit to height
                float normSample = sample / 32768.0f;
                path.lineTo(currentX, midY - (normSample * (h / 2)));
                currentX += xStep;
            }
        }
        p.drawPath(path);
    }

    void drawDifference(QPainter &p, const std::vector<AudioFrame> &in, const std::vector<AudioFrame> &out, int yOffset, int h, QColor color, QString label) {
        p.setPen(color);
        int midY = yOffset + (h / 2);
        p.drawText(10, yOffset + 15, label);

        if (in.size() != out.size()) return;

        float xStep = (float)width() / (in.size() * 160);
        float currentX = 0;
        QPainterPath path;
        path.moveTo(0, midY);

        for (size_t f = 0; f < in.size(); ++f) {
            size_t sampleCount = std::min(in[f].samples.size(), out[f].samples.size());
            for (size_t s = 0; s < sampleCount; ++s) {
                // Difference shows what the AEC actually "removed" or failed to remove
                float diff = (in[f].samples[s] - out[f].samples[s]) / 32768.0f;
                path.lineTo(currentX, midY - (diff * (h / 2)));
                currentX += xStep;
            }
        }
        p.drawPath(path);
    }
};



