/* kb_reader.h */
#ifndef LINUX_KEYLOGGER_KB_WORKER_H
#define LINUX_KEYLOGGER_KB_WORKER_H

#include <kb_mapper.h>

typedef u_int8_t status;

#define KB_WORKER_RESET   3
#define KB_WORKER_INITIAL 2
#define KB_WORKER_RUNNING 1
#define KB_WORKER_FAILED  0

struct kb_worker {
    pthread_t *kb_thread;             // The associated thread for the keyboard.
    char      *kb_event_file;         // The event file of the keyboard.
    int        kb_id;                 // The id of the current keyboard worker.
    status     kb_status: 2;          // The status of the keyboard worker.
};


_Noreturn extern void *start_worker(void *worker);



#endif
