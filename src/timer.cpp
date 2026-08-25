#include "timer.hpp"

Timer::Timer(int durationSeconds) : totalSeconds_(durationSeconds) {}

void Timer::start() {
    start_ = Clock::now();
    running_ = true;
    paused_ = false;
}

void Timer::togglePause() {
    if (!running_) return;
    if (paused_) {
        auto pauseDuration = Clock::now() - pauseStart_;
        start_ += pauseDuration;
        paused_ = false;
    } else {
        pauseStart_ = Clock::now();
        paused_ = true;
    }
}

bool Timer::isFinished() const {
    if (!running_) return false;
    return remaining() <= 0;
}

bool Timer::isPaused() const {
    return paused_;
}

int Timer::remaining() const {
    if (!running_) return totalSeconds_;
    auto now = paused_ ? pauseStart_ : Clock::now();
    int elapsed = static_cast<int>(
        std::chrono::duration_cast<std::chrono::seconds>(now - start_).count()
    );
    int rem = totalSeconds_ - elapsed;
    return rem < 0 ? 0 : rem;
}

void Timer::reset() {
    running_ = false;
    paused_ = false;
}
