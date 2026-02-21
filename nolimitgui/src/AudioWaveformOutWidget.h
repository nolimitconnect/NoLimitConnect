#pragma once

#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <vector>
#include "AudioFrameBuffer.h"

#include "MainWindow.h"

class AudioWaveformOutWidget : public QFrame {
    Q_OBJECT
public:
    explicit AudioWaveformOutWidget(QWidget *parent = nullptr) : QFrame(parent) {
        setBackgroundRole(QPalette::Base);
        setAutoFillBackground(true);
        setMinimumHeight(150);
        
        // Update at 60 FPS for smooth scrolling
        m_refreshTimer = new QTimer(this);
        connect(m_refreshTimer, &QTimer::timeout, this, QOverload<>::of(&AudioWaveformOutWidget::update));
        m_refreshTimer->start(16); 
    }

    std::vector<AudioFrame>     getAudioSpeakerHistory()
    {
        // Implementation: Return buffer from Speaker Out
        return getApp().getAudioMgr().getAudioOutBuffer().getHistory();
    }


protected:
    void                        paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), Qt::black);

        auto inFrames = getAudioSpeakerHistory();

        if (inFrames.empty()) return;

        int sectionHeight = height() / 3;
        
        // 1. Draw Raw Speaker out (Red)
        drawWaveform(painter, inFrames, 0, sectionHeight, Qt::red, "SPEAKER OUT");

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
};



