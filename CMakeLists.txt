cmake_minimum_required(VERSION 3.20)
project(linux_keylogger C)

set(CMAKE_C_STANDARD 99)

add_executable(linux_keylogger main.c tools/include/keyboard/keyboard.h tools/include/file/file_handler.h tools/keyboard.c tools/file_handler.c)

include_directories(
        tools/include
)