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

#define INITIAL_WORKER_MAKER_DELAY  5
#define REDESCOVER_DELAY            3

#define MAXIMUM_BUFFER_SIZE         100000
#define MAXIMUM_EVENT_RETRIEVE      30

struct kb_worker **workers = NULL;
static size_t workers_s = 0;
static int latest_worker_id = 0;

/**
 * Read the contents of /proc/bus/input/devices and
 * @return The contents of /proc/bus/input/devices.
 */
static unsigned char *get_devices_file_content()
{
    static unsigned char buffer[MAXIMUM_BUFFER_SIZE];
    // Initialize buffer with zeros.
    memset(buffer,0, MAXIMUM_BUFFER_SIZE);

    int device_fd = open(DEVICE_LOCATION, O_RDONLY);
    if (device_fd == -1) return NULL;

    char ch;
    ssize_t bytes = 1;
    int i = 0;
    // Read the contents of the device file.
    while (bytes)
    {
        bytes = read(device_fd, &ch, sizeof(ch));
        if (bytes == -1) return NULL;
        buffer[i++] = ch;
    }

    close(device_fd);
    return buffer;
}

static inline unsigned int get_event_ends_offset(const char *event_location)
{
    unsigned int offset = 0;
    while (event_location[offset] != ' ' && event_location[offset] != '\n') offset++;

    return offset;
}

/**
 * Find the event file each keyboard.
 * @param device_file The device file that contain the events.
 * @return  On success returns a null-terminated array that contains
 * the names of each event that corespondent to a keyboard. On error
 * NULL returned.
 */
static char **retrieve_events(const unsigned char *device_file)
{
    static char *retrieved_events[MAXIMUM_EVENT_RETRIEVE];
    // Initialize the array.
    memset(retrieved_events, 0, MAXIMUM_EVENT_RETRIEVE);

    // Make two copies of the device_file.
    char tmp[strlen((const char *) device_file)];
    strcpy(tmp, (const char *) device_file);

    char *curr_handler = strstr(tmp, "Handlers");
    char *curr_ev;
    char *curr_event;

    char *curr_state = strstr((const char *) device_file, "Handlers");
    unsigned int curr_offset = 0;

    int i = 0;
    while (curr_handler)
    {
        curr_ev = strstr(curr_handler, "EV=");
        curr_ev = strtok(curr_ev, "\n");
        if (curr_ev == NULL) return NULL;

        curr_state = strstr(curr_state, "event");
        if (!strcmp((curr_ev + 3), "120013") || 
            !strcmp((curr_ev + 3), "12001f"))
        {
            if (i > MAXIMUM_EVENT_RETRIEVE - 1) break;
            curr_offset = get_event_ends_offset(curr_state);
            // Allocate space for the new discovered event.
            ALLOC_MEM(curr_event, strlen("event") + curr_offset,
                      sizeof(char))

            memcpy(curr_event, curr_state, curr_offset);
            retrieved_events[i++] = curr_event;
        }
        curr_handler = strtok(NULL, "");
        curr_handler = strstr(curr_handler, "Handlers");
        curr_state = strstr(curr_state, "Handlers");
    }

    return retrieved_events;
}

/**
 * Discover new keyboard devices.
 * @return On success returns a null-terminated array that contains
 * the path to the event file for each keyboard on the system. On error NULL
 * returned.
 */
static char **kb_discovery()
{
    unsigned char *device_file = get_devices_file_content();
    static char *event_paths[MAXIMUM_EVENT_RETRIEVE];
    // Initialize event files.
    memset(event_paths, 0, MAXIMUM_EVENT_RETRIEVE);

    // retrieve the events.
    char **retrieved_events = retrieve_events(device_file);
    // Build up the paths.

    int curr_event = 0;
    while (retrieved_events[curr_event])
    {
        if (curr_event > MAXIMUM_EVENT_RETRIEVE - 1) break;

        ALLOC_MEM(event_paths[curr_event],
                  strlen(retrieved_events[curr_event]) + strlen(DEVICE_HANDLER_PATH) + 1,
                  sizeof(char));

        strcpy(event_paths[curr_event], DEVICE_HANDLER_PATH);
        strcat(event_paths[curr_event], retrieved_events[curr_event]);
        FREE_MEM(retrieved_events[curr_event]);
        ++curr_event;
    }

    return event_paths;
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
    char **discovered_kbs = kb_discovery();
    if (discovered_kbs == NULL) return;

    int prev_latest_worker_id = latest_worker_id;

    for (int kb = 0; discovered_kbs[kb]; kb++)
    {
        // If there is an actual new keyboard then produce worker and append the new worker in the workers.
        if (!has_kb_worker(discovered_kbs[kb])) append_to_workers(produce_worker(discovered_kbs[kb]));
        else abort_discovery(discovered_kbs[kb]);
    }

    if (latest_worker_id > prev_latest_worker_id)
    {
        int new_discoveries = latest_worker_id - prev_latest_worker_id;
        make_terminal_log("keyboard - 0, is not a real keyboard. Is something like a manager for the available keyboards.", 0);
        make_terminal_log("New keyboard has been discovered...", 0);
        make_terminal_log("Waiting for capturing to start...", 0);

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
