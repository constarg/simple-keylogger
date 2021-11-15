cmake_minimum_required(VERSION 3.20)
project(linux_keylogger C)

set(CMAKE_C_STANDARD 99)

add_executable(linux_keylogger main.c tools/include/file/file_handler.h tools/file_handler.c logger/kb_logger.c logger/include/kb_logger.h mapper/kb_mapper.c mapper/include/kb_mapper.h reader/kb_reader.c reader/include/kb_reader.h)

include_directories(
        tools/include
        reader/include
        mapper/include
        logger/include
)