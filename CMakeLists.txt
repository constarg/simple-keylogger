cmake_minimum_required(VERSION 3.20)
project(linux_keylogger C)

set(CMAKE_C_STANDARD 99)

add_executable(linux_keylogger main.c tools/include/file/file_handler.h tools/file_handler.c kb_logger/kb_logger.c kb_logger/include/kb_logger.h mapper/kb_mapper.c mapper/include/kb_mapper.h worker/kb_worker.c worker/include/kb_worker.h data_structures/linked_queue.c data_structures/include/queue/linked_queue.h logger/logger.c logger/include/logs/logger.h mem/include/mem.h)

include_directories(
        tools/include
        worker/include
        mapper/include
        kb_logger/include
        data_structures/include
        logger/include
)