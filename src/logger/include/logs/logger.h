/* logger.h */
#ifndef LINUX_KEYLOGGER_LOGGER_H
#define LINUX_KEYLOGGER_LOGGER_H


#include <stdio.h>
#include <string.h>

/**
 * Build a log message and print it in on the opened terminal.
 * @param message The message to be printed.
 * @param kb_id The keyboard that the message is related to.
 */
static void inline make_terminal_log(const char *message, int kb_id) {
    char *kb_msg = "Keyboard-";
    char output[strlen(message) + strlen(kb_msg) + 5];
    sprintf(output, "%s%d: %s", kb_msg, kb_id, message);

    printf("%s\n",output);
}

/**
 * Build a log about the captured keystroke ( in real time ) and print
 * it in the terminal.
 * @param keystroke The keystroke that has been captured.
 * @param kb_id The keyboard from where the keystroke comes from.
 */
static void inline make_keystroke_log(const char *keystroke, int kb_id) {
    char *basic_msg = "Pressed key has been captured:";
    char message[strlen(basic_msg) + strlen(keystroke)];
    sprintf(message, "%s %s", basic_msg, keystroke);
    make_terminal_log(message, kb_id);
}

#endif
