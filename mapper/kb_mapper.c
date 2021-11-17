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


static char **kb_discovery(size_t *size) {
    size_t event_files_s = 1;
    char **event_files;
    ALLOC_MEM(event_files, event_files_s, sizeof(char **));

    // Open the file that has all the devices.
    int devices_fd = open(DEVICE_LOCATION, O_RDONLY);
    if (devices_fd == -1) return NULL;

    ssize_t devices_s = 1;
    char *devices;
    char *buffer;
    ALLOC_MEM(devices, 2,sizeof(char));
    ALLOC_MEM(buffer, 2, sizeof(char));

    ssize_t bytes = 1;
    while (bytes) {
        bytes = read(devices_fd, buffer, 1);
        if (bytes == -1) break;
        strcat(devices, buffer);
        devices_s += bytes;
        REALLOC_MEM(devices, devices_s + 1, sizeof(char));
    }
    free(buffer);

    char *current_handler = strstr(devices, "Handlers");
    char *current_ev;
    char *current_kb_id;
    char *current_event_file;
    char *current_event_path;
    char *tmp;
    int  is_keyboard;

    while (current_handler) {
        current_ev = strstr(current_handler, "EV=");

        ALLOC_MEM(tmp, strlen(current_ev) + 1, sizeof(char));
        strcpy(tmp, current_ev);
        current_kb_id = strtok(tmp, "\n");
        is_keyboard = !strcmp(current_kb_id, KEYBOARD_ID);
        free(tmp);

        if (is_keyboard) {
            ALLOC_MEM(tmp, strlen(current_handler) + 1, sizeof(char));
            strcpy(tmp, current_handler);
            current_event_file = strstr(tmp, "event");
            current_event_file = strtok(current_event_file, "\n");
            ALLOC_MEM(current_event_path, strlen(current_event_file) + strlen(DEVICE_HANDLER_PATH) + 1, sizeof(char));
            strcpy(current_event_path, DEVICE_HANDLER_PATH);
            strcat(current_event_path, current_event_file);

            event_files[event_files_s - 1] = current_event_path;
            REALLOC_MEM(event_files, ++event_files_s, sizeof(char **));
            free(tmp);
        }

        current_handler = strstr(current_ev, "Handlers");
    }

    close(devices_fd);
    *size = event_files_s - 1;
    free(devices);
    return event_files;
}

static inline void initialize_workers() {
    ALLOC_MEM(workers, 1, sizeof(struct kb_worker **));
}

static inline void worker_killer() {
    pthread_t curr_thread;

    // Kill all the active threads.
    for (int th = 0; th < workers_s; th++) {
        curr_thread = *(workers[th]->kb_thread);
        if (pthread_cancel(curr_thread) != 0) return;
    }
}


static inline void destroy_workers() {
    for (int fr = 0; fr < workers_s; fr++) {
        free(workers[fr]->kb_event_file);
        free(workers[fr]->kb_thread);
        free(workers[fr]);
    }
    free(workers);
    latest_worker_id = 0;
}

static inline void append_to_workers(struct kb_worker *new_worker) {
    REALLOC_MEM(workers, ++workers_s, sizeof(struct kb_worker **));
    // Add the new worker.
    workers[workers_s - 1] = new_worker;
}

static inline void remove_from_workers(int index) {
    if (index > workers_s) return;
    struct kb_worker *removed_worker = workers[index];

    for (int wr = index; wr < (workers_s - 1); wr++) workers[wr + 1] = workers[wr];

    --workers_s;
    free(removed_worker);
}

static inline int has_kb_worker(const struct kb_worker *worker) {

    for (int wr = 0; wr < workers_s; wr++) {
        if (workers == NULL) return FALSE;
        if (!strcmp(worker->kb_event_file, workers[wr]->kb_event_file)) {
            if (workers[wr]->kb_status != KB_WORKER_FAILED) return TRUE;
            else remove_from_workers(wr);
        }
    }

    return FALSE;
}

static void discovery() {
    size_t discovered_kbs_s = 0;
    char **discovered_kbs = kb_discovery(&discovered_kbs_s);
    if (discovered_kbs == NULL) return;

    int new_discovery = FALSE;
    // Make a new worker.
    struct kb_worker *new_worker;
    pthread_t *worker_thread;

    for (int kb = 0; kb < discovered_kbs_s; kb++) {
        ALLOC_MEM(new_worker, 1, sizeof(struct kb_worker));
        ALLOC_MEM(worker_thread, 1, sizeof(pthread_t));
        new_worker->kb_id = ++latest_worker_id;
        new_worker->kb_status = KB_WORKER_INITIAL;
        new_worker->kb_thread = worker_thread;
        new_worker->kb_event_file = discovered_kbs[kb];

        if (!has_kb_worker((const struct kb_worker *) new_worker)) {
            append_to_workers(new_worker);
            new_discovery = TRUE;
        } else {
            --latest_worker_id;
            free(new_worker->kb_event_file);
            free(new_worker);
            free(worker_thread);
        }
    }

    free(discovered_kbs);
    if (new_discovery) {
        make_terminal_log("New keyboard has been discovered", 0);
        worker_killer();
    }
}

_Noreturn static void *discovery_thread(void *arg) {
    while (TRUE) {
        discovery();
        sleep(REDESCOVER_DELAY);
    }
}

_Noreturn static void *worker_maker_thread(void *arg) {
    // Wait for 5 seconds to make the first discovery.
    // Yes 5 seconds is too much but for this function to work it must be sure that the discovery has been made.
    sleep(INITIAL_WORKER_MAKER_DELAY);
    while (TRUE) {
        for (int wr = 0; wr < workers_s; wr++) pthread_create(workers[wr]->kb_thread, NULL, start_worker, (void *) workers[wr]);
        // Wait for all the threads to end.
        for (int wr = 0; wr < workers_s; wr++) pthread_join(*workers[wr]->kb_thread, NULL);
        if (workers_s == 0) sleep(2);
    }
}

static void cleanup(int sig) {
    destroy_workers();
    exit(0);
}

static inline void initialize_signal() {
    signal(SIGINT, cleanup);
}

void map_keyboards() {
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