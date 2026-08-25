#pragma once

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

struct TerminalRAII {
    termios original{};

    TerminalRAII() {
        tcgetattr(STDIN_FILENO, &original);
        termios raw = original;
        raw.c_lflag &= ~(ECHO | ICANON);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }

    ~TerminalRAII() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
    }

    TerminalRAII(const TerminalRAII&) = delete;
    TerminalRAII& operator=(const TerminalRAII&) = delete;
};

inline char readKey() {
    char c = 0;
    read(STDIN_FILENO, &c, 1);
    return c;
}
