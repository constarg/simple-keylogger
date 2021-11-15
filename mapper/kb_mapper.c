/* kb_mapper.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include <kb_mapper.h>

#define REMAP_INTERVAL          10

#define DEVICE_LOCATION         "/proc/bus/input/devices"
#define DEVICE_HANDLER_PATH     "/dev/input/"
#define KEYBOARD_ID             "EV=120013"

static void create_kb_worker(const char *kb_event_file) {
    // TODO - Here make a new thread and call the reader.
}

static int has_kb_worker(const char *kb_event_file) {
    // TODO - Here find if the keyboard we are looking for had already a worker, so we don't make duplicate workers.
    return 0;
}

void map_keyboards(void *arg) {
    // TODO - Here find all the keyboard inside the device file and call the create_kb_worker for each of them.
}
