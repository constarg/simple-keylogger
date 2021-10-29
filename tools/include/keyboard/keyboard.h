/* keyboard.h */
#ifndef LINUX_KEYLOGGER_KEYBOARD_H
#define LINUX_KEYLOGGER_KEYBOARD_H

/**
 * This function looks at /proc/bus/input/devices
 * and determines the current active keyboard.
 * @return The path to the event handler for the specific keyboard.
 */
const char *get_active_keyboard();

/**
 * This function decodes the raw bits from the event handler
 * and translate those in ascii encode.
 * @param keyboard_data The data to be decoded.
 * @return The decoded data or Null on error.
 */
const char *decode_keyboard_signals(const char *keyboard_data);


#endif
