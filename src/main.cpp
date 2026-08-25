#include "session.hpp"
#include "input.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>

static void clearLine() {
    std::cout << "\r\033[K";
}

static void clearScreen() {
    std::cout << "\033[2J\033[H";
}

static void setColor(const char* code) {
    std::cout << "\033[" << code << "m";
}

static void resetColor() {
    std::cout << "\033[0m";
}

static const char* phaseColor(Phase p) {
    switch (p) {
        case Phase::Work:       return "1;36";
        case Phase::ShortBreak: return "1;34";
        case Phase::LongBreak:  return "1;35";
    }
    return "0";
}

static const char* phaseName(Phase p) {
    switch (p) {
        case Phase::Work:       return "WORK";
        case Phase::ShortBreak: return "SHORT BREAK";
        case Phase::LongBreak:  return "LONG BREAK";
    }
    return "";
}

static void printStatus(const Session& session, const SessionConfig& config) {
    const Timer& t = session.timer();
    int rem = t.remaining();
    int mins = rem / 60;
    int secs = rem % 60;

    clearLine();
    setColor(phaseColor(session.currentPhase()));
    std::cout << "  " << phaseName(session.currentPhase());
    resetColor();

    int total = 0;
    switch (session.currentPhase()) {
        case Phase::Work:       total = config.workMinutes * 60; break;
        case Phase::ShortBreak: total = config.shortBreakMinutes * 60; break;
        case Phase::LongBreak:  total = config.longBreakMinutes * 60; break;
    }
    int elapsed = total - rem;
    int barWidth = 30;
    int filled = (elapsed * barWidth) / total;
    if (filled > barWidth) filled = barWidth;

    std::cout << "  [";
    setColor(phaseColor(session.currentPhase()));
    for (int i = 0; i < barWidth; i++) {
        std::cout << (i < filled ? '#' : '-');
    }
    resetColor();
    std::cout << "] ";

    char timeBuf[16];
    std::snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", mins, secs);
    setColor("1;36");
    std::cout << timeBuf;
    resetColor();

    std::cout << "  Round " << session.currentRound() << "/" << session.totalRounds();
    std::cout << std::flush;
}

static void drawHeader() {
    setColor("1;36");
    std::cout << "\n  ==================\n";
    std::cout << "      POMODORO\n";
    std::cout << "  ==================\n\n";
    resetColor();
}

static void showMainMenu(const SessionConfig& config) {
    clearScreen();
    drawHeader();

    setColor("1;36");
    std::cout << "  Work Duration      : ";
    resetColor();
    setColor("1;36");
    std::cout << config.workMinutes << " minutes\n";
    resetColor();

    setColor("1;36");
    std::cout << "  Short Break        : ";
    resetColor();
    setColor("1;36");
    std::cout << config.shortBreakMinutes << " minutes\n";
    resetColor();

    setColor("1;36");
    std::cout << "  Long Break         : ";
    resetColor();
    setColor("1;36");
    std::cout << config.longBreakMinutes << " minutes\n";
    resetColor();

    setColor("1;36");
    std::cout << "  Rounds             : ";
    resetColor();
    setColor("1;36");
    std::cout << config.roundsBeforeLongBreak << " rounds\n\n";
    resetColor();

    std::cout << "  -----------------------------------\n";
    setColor("1;36");
    std::cout << "  1. Start Pomodoro\n";
    resetColor();
    setColor("1;36");
    std::cout << "  2. Settings\n";
    resetColor();
    setColor("1;36");
    std::cout << "  3. Exit\n";
    resetColor();
    std::cout << "  -----------------------------------\n\n";
}

static void showSettingsMenu(SessionConfig& config) {
    clearScreen();
    drawHeader();

    setColor("1;36");
    std::cout << "  Timer Settings\n\n";
    resetColor();

    std::cout << "  1. Work Duration     [";
    setColor("1;36");
    std::cout << config.workMinutes;
    resetColor();
    std::cout << " minutes]\n";

    std::cout << "  2. Short Break       [";
    setColor("1;36");
    std::cout << config.shortBreakMinutes;
    resetColor();
    std::cout << " minutes]\n";

    std::cout << "  3. Long Break        [";
    setColor("1;36");
    std::cout << config.longBreakMinutes;
    resetColor();
    std::cout << " minutes]\n";

    std::cout << "  4. Number of Rounds  [";
    setColor("1;36");
    std::cout << config.roundsBeforeLongBreak;
    resetColor();
    std::cout << " rounds]\n\n";

    std::cout << "  5. Back\n\n";
    std::cout << "  -----------------------------------\n";
}

static int readIntInput(const char* prompt) {
    int val = 0;
    while (true) {
        std::cout << prompt;
        std::cin.clear();
        std::cin >> val;
        if (std::cin.eof()) return -1;
        if (std::cin && val > 0) return val;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        setColor("1;31");
        std::cout << "  Invalid input!\n";
        resetColor();
    }
}

static void runTimer(SessionConfig& config) {
    TerminalRAII terminal;
    Session session(config);

    std::cout << "\033[?25l";
    session.startPhase();

    while (!session.isFinished()) {
        printStatus(session, config);

        char key = readKey();
        if (key == 'q' || key == 'Q') break;
        if (key == ' ') session.togglePause();
        if (key == 'n' || key == 'N') session.skipPhase();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (session.timer().isFinished()) {
            std::cout << "\a";
            session.skipPhase();
        }
    }

    std::cout << "\033[?25h";
    clearLine();

    if (session.isFinished()) {
        setColor("1;36");
        std::cout << "\n  All rounds completed! Great job!\n\n";
        resetColor();
    } else {
        std::cout << "\n  Session ended.\n\n";
    }

    std::cout << "  Press Enter to return to the menu...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

int main() {
    SessionConfig config;

    while (true) {
        showMainMenu(config);

        int choice = readIntInput("  Choice: ");
        if (choice == -1) break;

        switch (choice) {
            case 1:
                clearScreen();
                runTimer(config);
                break;

            case 2:
                while (true) {
                    showSettingsMenu(config);
                    int s = readIntInput("  Choice: ");
                    if (s == 5) break;
                    switch (s) {
                        case 1:
                            config.workMinutes = readIntInput("  Work duration (minutes): ");
                            break;
                        case 2:
                            config.shortBreakMinutes = readIntInput("  Short break duration (minutes): ");
                            break;
                        case 3:
                            config.longBreakMinutes = readIntInput("  Long break duration (minutes): ");
                            break;
                        case 4:
                            config.roundsBeforeLongBreak = readIntInput("  Number of rounds: ");
                            break;
                        default:
                            setColor("1;31");
                            std::cout << "  Invalid choice!\n";
                            resetColor();
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                            break;
                    }
                }
                break;

            case 3:
                clearScreen();
                setColor("1;36");
                std::cout << "\n  Goodbye! Stay productive!\n\n";
                resetColor();
                return 0;

            default:
                setColor("1;31");
                std::cout << "  Invalid choice!\n";
                resetColor();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                break;
        }
    }
}
