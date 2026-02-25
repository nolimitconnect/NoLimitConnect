#pragma once

#include <vector>
#include <mutex>
#include <deque>

struct AudioFrame {
    std::vector<int16_t> samples;
    float vadProbability = 0.0f;
    float echoReturnLoss = 0.0f;
    bool isProcessed = false;

    AudioFrame() { samples.reserve(160); }
};

class AudioFrameBuffer {
public:
    // We keep a history of 'maxFrames' to show a "rolling" window in the UI
    explicit AudioFrameBuffer(size_t maxFrames = 50)
        : m_maxFrames(maxFrames)
        , m_maxPlaybackFrames(maxFrames)
    {}

    // Called by the Audio Thread (Miniaudio callback)
    void pushFrame(const int16_t* data, size_t count, float vad, float erl, bool processed) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        // Construct the frame directly in the deque
        m_queue.emplace_back();
        AudioFrame& frame = m_queue.back();
        
        // Direct assignment is fast for small fixed-size POD (Plain Old Data)
        frame.samples.assign(data, data + count);
        frame.vadProbability = vad;
        frame.echoReturnLoss = erl;
        frame.isProcessed = processed;

        if (m_queue.size() > m_maxFrames) {
            m_queue.pop_front();
        }

        m_playbackQueue.push_back(frame);
        if (m_playbackQueue.size() > m_maxPlaybackFrames) {
            m_playbackQueue.pop_front();
        }
    }

    bool popPlaybackFrame(AudioFrame& outFrame) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_playbackQueue.empty()) {
            return false;
        }

        outFrame = m_playbackQueue.front();
        m_playbackQueue.pop_front();
        return true;
    }
    
    // Called by the GUI Thread (Qt Timer)
    // Returns a copy of the current buffer to visualize
    std::vector<AudioFrame> getHistory() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return std::vector<AudioFrame>(m_queue.begin(), m_queue.end());
    }

private:
    std::deque<AudioFrame> m_queue;
    std::deque<AudioFrame> m_playbackQueue;
    size_t m_maxFrames;
    size_t m_maxPlaybackFrames;
    std::mutex m_mutex;
};


