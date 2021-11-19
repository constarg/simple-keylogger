/* decode.h */
#ifndef LINUX_KEYLOGGER_KB_DECODER_H
#define LINUX_KEYLOGGER_KB_DECODER_H

/**
 * Decoded information contains.
 */
struct kb_dec_key {
    char *kb_key_name;
    int   kb_key_code;
};

/**
 * Takes a keycode and responds with a structure that
 * contains both the keycode of the current keystroke
 * and the name of the key that has been pressed.
 * @param key_code The key code of interest.
 * @return on success a structure that contains both the name and the keycode of the keystroke.
 * On error returns NULL.
 */
extern struct kb_dec_key *decode(int key_code);

#endif