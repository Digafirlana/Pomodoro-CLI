#pragma once

#include "timer.hpp"

enum class Phase {
    Work,
    ShortBreak,
    LongBreak
};

struct SessionConfig {
    int workMinutes = 25;
    int shortBreakMinutes = 5;
    int longBreakMinutes = 15;
    int roundsBeforeLongBreak = 4;
};

class Session {
public:
    explicit Session(const SessionConfig& config);

    void startPhase();
    void skipPhase();
    void togglePause();
    void reset();

    Phase currentPhase() const;
    int currentRound() const;
    int totalRounds() const;
    const Timer& timer() const;
    bool isFinished() const;

private:
    void advancePhase();

    SessionConfig config_;
    Timer timer_;
    Phase phase_ = Phase::Work;
    int round_ = 1;
};
