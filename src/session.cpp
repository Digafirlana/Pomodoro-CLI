#include "session.hpp"

Session::Session(const SessionConfig& config)
    : config_(config)
    , timer_(config.workMinutes * 60)
    , phase_(Phase::Work)
    , round_(1)
{}

void Session::startPhase() {
    switch (phase_) {
        case Phase::Work:
            timer_ = Timer(config_.workMinutes * 60);
            break;
        case Phase::ShortBreak:
            timer_ = Timer(config_.shortBreakMinutes * 60);
            break;
        case Phase::LongBreak:
            timer_ = Timer(config_.longBreakMinutes * 60);
            break;
    }
    timer_.start();
}

void Session::skipPhase() {
    advancePhase();
    startPhase();
}

void Session::togglePause() {
    timer_.togglePause();
}

void Session::reset() {
    phase_ = Phase::Work;
    round_ = 1;
    timer_ = Timer(config_.workMinutes * 60);
}

Phase Session::currentPhase() const {
    return phase_;
}

int Session::currentRound() const {
    return round_;
}

int Session::totalRounds() const {
    return config_.roundsBeforeLongBreak;
}

const Timer& Session::timer() const {
    return timer_;
}

bool Session::isFinished() const {
    return round_ > config_.roundsBeforeLongBreak;
}

void Session::advancePhase() {
    if (phase_ == Phase::Work) {
        if (round_ % config_.roundsBeforeLongBreak == 0) {
            phase_ = Phase::LongBreak;
        } else {
            phase_ = Phase::ShortBreak;
        }
    } else {
        if (phase_ == Phase::LongBreak) {
            round_++;
            if (round_ > config_.roundsBeforeLongBreak) return;
        } else {
            round_++;
        }
        phase_ = Phase::Work;
    }
}
