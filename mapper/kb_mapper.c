/* kb_mapper.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <kb_mapper.h>
#include <kb_worker.h>
#include <mem.h>
#include <constants/constants.h>
#include <logs/logger.h>

#define DEVICE_LOCATION             "/proc/bus/input/devices"
#define DEVICE_HANDLER_PATH         "/dev/input/"
#define KEYBOARD_ID                 "EV=120013"

#define INITIAL_WORKER_MAKER_DELAY  5
#define REDESCOVER_DELAY            3

struct kb_worker **workers = NULL;
static size_t workers_s = 0;
static int latest_worker_id = 0;

static char *correct_event_file(const char *event_file)
{
    char tmp[strlen(event_file)];
    strcpy(tmp, event_file);
    char *check_correct = strtok(tmp, " ");
    char *result;
    if (check_correct == NULL)
    {
        ALLOC_MEM(result, strlen(event_file), sizeof(char))
        strncpy(result, event_file, strlen(event_file) - 1);
    }
    ALLOC_MEM(result, strlen(check_correct) + 1, sizeof(char))
    strcpy(result, check_correct);

    return result;
}

static char **kb_discovery(size_t *size)
{
    size_t event_files_s = 1;
    char **event_files;
    ALLOC_MEM(event_files, event_files_s, sizeof(char **))

    // Open the file that has all the devices.
    int devices_fd = open(DEVICE_LOCATION, O_RDONLY);
    if (devices_fd == -1) return NULL;

    ssize_t devices_s = 1;
    char *devices;
    char *buffer;
    ALLOC_MEM(devices, 2,sizeof(char))
    ALLOC_MEM(buffer, 2, sizeof(char))

    ssize_t bytes = 1;
    while (bytes)
    {
        bytes = read(devices_fd, buffer, 1);
        if (bytes == -1) break;
        strcat(devices, buffer);
        devices_s += bytes;
        REALLOC_MEM(devices, devices_s + 1, sizeof(char))
    }
    FREE_MEM(buffer)

    char *current_handler = strstr(devices, "Handlers");
    char *current_ev;
    char *current_kb_id;
    char *current_event_file;
    char *current_event_path;
    char *tmp;
    char *corrected_event;
    int  is_keyboard;

    while (current_handler)
    {
        current_ev = strstr(current_handler, "EV=");

        ALLOC_MEM(tmp, strlen(current_ev) + 1, sizeof(char))
        strcpy(tmp, current_ev);
        current_kb_id = strtok(tmp, "\n");
        is_keyboard = !strcmp(current_kb_id, KEYBOARD_ID);
        FREE_MEM(tmp)

        if (is_keyboard)
        {
            ALLOC_MEM(tmp, strlen(current_handler) + 1, sizeof(char))
            strcpy(tmp, current_handler);
            current_event_file = strstr(tmp, "event");
            current_event_file = strtok(current_event_file, "\n");
            corrected_event = correct_event_file(current_event_file);
            ALLOC_MEM(current_event_path, strlen(corrected_event) + strlen(DEVICE_HANDLER_PATH) + 1, sizeof(char))
            strcpy(current_event_path, DEVICE_HANDLER_PATH);
            strcat(current_event_path, corrected_event);

            event_files[event_files_s - 1] = current_event_path;
            REALLOC_MEM(event_files, ++event_files_s, sizeof(char **))
            FREE_MEM(corrected_event)
            FREE_MEM(tmp)
        }

        current_handler = strstr(current_ev, "Handlers");
    }

    close(devices_fd);
    *size = event_files_s - 1;
    FREE_MEM(devices)
    return event_files;
}

static inline void initialize_workers()
{
    ALLOC_MEM(workers, 1, sizeof(struct kb_worker **))
}

static inline void worker_killer()
{
    // Kill all the active threads.
    for (int th = 0; th < workers_s; th++)
        if (pthread_cancel(*(workers[th]->kb_thread)) != 0) return;
}

/**
 * Free the memory of a worker that we made, but already
 * exists.
 *
 * @param new_worker  The worker that already exists.
 */
static inline void free_worker_mem(struct kb_worker *worker)
{
    FREE_MEM(worker->kb_event_file)
    FREE_MEM(worker->kb_thread)
    FREE_MEM(worker)
}

static inline void destroy_workers()
{
    for (int fr = 0; fr < workers_s; fr++) free_worker_mem(workers[fr]);
    FREE_MEM(workers)
    latest_worker_id = 0;
}

static inline void append_to_workers(struct kb_worker *new_worker)
{
    REALLOC_MEM(workers, ++workers_s, sizeof(struct kb_worker **))
    // Add the new worker.
    workers[workers_s - 1] = new_worker;
}

static inline void remove_from_workers(int index)
{
    if (index > workers_s) return;
    struct kb_worker *removed_worker = workers[index];

    for (int wr = index; wr < (workers_s - 1); wr++) workers[wr + 1] = workers[wr];

    --workers_s;
    free_worker_mem(removed_worker);
}

static inline int has_kb_worker(const char *event_file)
{

    for (int wr = 0; wr < workers_s; wr++)
    {
        if (workers == NULL) return FALSE;

        if (!strcmp(workers[wr]->kb_event_file, event_file) &&
            workers[wr]->kb_status == KB_WORKER_FAILED)
        {
            remove_from_workers(wr);
            return FALSE;
        }

        if (!strcmp(workers[wr]->kb_event_file, event_file)) return TRUE;
    }

    return FALSE;
}

static inline struct kb_worker *produce_worker(const char *event_file)
{
    pthread_t *worker_thread;
    struct kb_worker *new_worker;
    ALLOC_MEM(worker_thread, 1, sizeof(pthread_t))
    ALLOC_MEM(new_worker, 1, sizeof(struct kb_worker))
    // Initialize worker.
    new_worker->kb_id = ++latest_worker_id;
    new_worker->kb_event_file = (char *) event_file;
    new_worker->kb_status = KB_WORKER_INITIAL;
    new_worker->kb_thread = worker_thread;

    return new_worker;
}

static inline void abort_discovery(char *event_file)
{
    FREE_MEM(event_file);
}

static inline void reset_workers()
{
    worker_killer();
}

static void discovery()
{
    size_t discovered_kbs_s = 0;
    char **discovered_kbs = kb_discovery(&discovered_kbs_s);
    if (discovered_kbs == NULL) return;

    int prev_latest_worker_id = latest_worker_id;

    for (int kb = 0; kb < discovered_kbs_s; kb++)
    {
        // If there is an actual new keyboard then produce worker and append the new worker in the workers.
        if (!has_kb_worker(discovered_kbs[kb])) append_to_workers(produce_worker(discovered_kbs[kb]));
        else abort_discovery(discovered_kbs[kb]);
    }

    FREE_MEM(discovered_kbs)
    if (latest_worker_id > prev_latest_worker_id)
    {
        int new_discoveries = latest_worker_id - prev_latest_worker_id;

        if (new_discoveries == 1) make_terminal_log("New keyboard has been discovered", 0);
        else make_terminal_log("New keyboards has been discovered", 0);

        // Reset workers to start capturing the new discovered keyboards.
        reset_workers();
    }
}

__attribute__((noreturn)) static void *discovery_thread(void *arg)
{
    while (TRUE)
    {
        discovery();
        sleep(REDESCOVER_DELAY);
    }
}

__attribute__((noreturn)) static void *worker_maker_thread(void *arg)
{
    // Wait for 5 seconds to make the first discovery.
    // Yes 5 seconds is too much but for this function to work it must be sure that the discovery has been made.
    sleep(INITIAL_WORKER_MAKER_DELAY);
    while (TRUE) {
        for (int wr = 0; wr < workers_s; wr++) pthread_create(workers[wr]->kb_thread, NULL, start_worker, (void *) workers[wr]);
        // Wait for all the threads to end.
        for (int wr = 0; wr < workers_s; wr++) pthread_join(*workers[wr]->kb_thread, NULL);
        sleep(2);
    }
}

static void cleanup(int sig)
{
    worker_killer();
    destroy_workers();
    exit(0);
}

static inline void initialize_signal()
{
    signal(SIGINT, cleanup);
}

void map_keyboards()
{
    initialize_signal();
    initialize_workers();
    // Make the two main threads.
    pthread_t discovery_t;
    pthread_t worker_maker_t;
    // Start the two main threads.
    if (pthread_create(&discovery_t, NULL, discovery_thread, NULL) != 0) return;
    if (pthread_create(&worker_maker_t, NULL, worker_maker_thread, NULL) != 0) return;

    pthread_join(discovery_t, NULL);
    pthread_join(worker_maker_t, NULL);

    destroy_workers();
}