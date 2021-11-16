/* kb_reader.c */
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>

#include <kb_worker.h>
#include <kb_mapper.h>
#include <kb_decoder.h>

#define TRUE 1

_Noreturn void *start_worker(void *worker) {
    struct kb_worker *worker_infos = (struct kb_worker *) worker;
    struct input_event device_event; // Current keyboard device event.
    struct kb_dec_key *decoded;
    char device_event_path[strlen(worker_infos->kb_event_file) - 1];
    // Remove an empty extra character that came from the parse.
    strncpy(device_event_path, worker_infos->kb_event_file, strlen(worker_infos->kb_event_file) - 1);

    int device_fd = open(device_event_path, O_RDONLY);
    if (device_fd != 0) {
        // TODO - Make a log.
        worker_infos->kb_status = KB_WORKER_FAILED;
        pthread_cancel(*worker_infos->kb_thread);
    }

    worker_infos->kb_status = KB_WORKER_RUNNING;
    while (TRUE) {
        if (read(device_fd, &device_event, sizeof(device_event)) != 0) {
            // TODO - Make a log.
            worker_infos->kb_status = KB_WORKER_FAILED;
            pthread_cancel(*worker_infos->kb_thread);
        }

        if (device_event.type == 1 && device_event.value == 1) {
            decoded = decode(device_event.code);
            if (decoded == NULL) continue;
            // TODO - Make a log to print the key and to log the key in a file.
        }
    }

    close(device_fd);
}