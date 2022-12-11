cmake_minimum_required(VERSION 3.20)
project(linux_keylogger C)

set(CMAKE_C_STANDARD 99)

add_executable(linux_keylogger src/main.c src/tools/include/file/file_handler.h src/tools/file_handler.c src/mapper/kb_mapper.c src/mapper/include/kb_mapper.h src/worker/kb_worker.c src/worker/include/kb_worker.h src/logger/include/logs/logger.h src/mem/include/mem.h src/kb_logger/include/kb_decoder.h src/kb_logger/kb_decoder.c src/tools/include/constants/constants.h)

set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/keylogger/)
file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/keylogger/logs)

include_directories(
        src/tools/include
        src/worker/include
        src/mapper/include
        src/kb_logger/include
        src/data_structures/include
        src/logger/include
        src/mem/include
)
