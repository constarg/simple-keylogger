/* kb_mapper.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <kb_mapper.h>
#include "../mem/include/mem.h"

#define REMAP_INTERVAL          10

#define DEVICE_LOCATION         "/proc/bus/input/devices"
#define DEVICE_HANDLER_PATH     "/dev/input/"
#define KEYBOARD_ID             "EV=120013"

static int latest_worker_id = 0;

static void create_kb_worker(const char *kb_event_file) {
    // TODO - Here make a new thread and call the worker.
}

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

void map_keyboards(void *arg) {
    // TODO - Here find all the keyboard inside the device file and call the create_kb_worker for each of them.
}
