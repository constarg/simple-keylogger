/* kb_mapper.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <kb_mapper.h>
#include <mem.h>

#define DEVICE_LOCATION         "/proc/bus/input/devices"
#define DEVICE_HANDLER_PATH     "/dev/input/"
#define KEYBOARD_ID             "EV=120013"


struct kb_worker **workers;
static int latest_worker_id = 1;

static int has_kb_worker(const char *kb_event_file) {
    // TODO - Here find if the keyboard we are looking for had already a worker, so we don't make duplicate workers.
    return 0;
}

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

static void worker_killer() {

}

static inline void initialize_workers() {
    ALLOC_MEM(workers, 1, sizeof(struct kb_worker **));
}

static inline void destroy_workers() { free(workers); }

static void *discovery_thread(void *arg) {
    // TODO - Discover new keyboards and modify the list of workers.
    // TODO - If a new keyboard is found then make a killer signal.
    return NULL;
}

static void *worker_maker_thread(void *arg) {
    // TODO - Make threads according to the list of workers.
    return NULL;
}

void map_keyboards() {
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