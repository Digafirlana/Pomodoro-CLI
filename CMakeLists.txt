cmake_minimum_required(VERSION 3.16)
project(pomodoro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pomodoro
    src/main.cpp
    src/timer.cpp
    src/session.cpp
)

target_compile_options(pomodoro PRIVATE -Wall -Wextra -pedantic)
