/* decode.h */
#ifndef LINUX_KEYLOGGER_KB_DECODER_H
#define LINUX_KEYLOGGER_KB_DECODER_H

struct kb_dec_key {
    char *kb_key_name;
    int   kb_key_code;
};

extern struct kb_dec_key *decode(int key_code);

#endif