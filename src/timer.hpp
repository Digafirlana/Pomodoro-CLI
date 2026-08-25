#pragma once

#include <chrono>

class Timer {
public:
    explicit Timer(int durationSeconds);

    void start();
    void togglePause();
    bool isFinished() const;
    bool isPaused() const;
    int remaining() const;

    void reset();

private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    int totalSeconds_;
    TimePoint start_;
    TimePoint pauseStart_;
    bool running_ = false;
    bool paused_ = false;
};
