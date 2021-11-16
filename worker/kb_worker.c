/* kb_reader.c */
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <kb_worker.h>
#include <kb_mapper.h>
#include <kb_decoder.h>


_Noreturn void *start_worker(void *worker) {
    struct kb_worker *worker_infos = (struct kb_worker *) worker;
    struct input_event device_event; // Current keyboard device event.
    struct kb_dec_key *decoded;
    char device_event_path[strlen(worker_infos->kb_event_file) - 1];
    // Remove an empty extra character that came from the parse.
    strncpy(device_event_path, worker_infos->kb_event_file, strlen(worker_infos->kb_event_file) - 1);

    int device_fd = open(device_event_path, O_RDONLY);
    if (device_fd != 0) printf("error: %s\n", strerror(errno));
    while ( 1 ) {
        read(device_fd, &device_event, sizeof(device_event));

        if (device_event.type == 1 && device_event.value == 1) {
            
        }
    }

    close(device_fd);
}