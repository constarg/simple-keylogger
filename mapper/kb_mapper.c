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

#define MAXIMUM_BUFFER_SIZE         50000
#define MAXIMUM_EVENT_RETRIEVE      30

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

static char **split_string(char *string, const char *delim)
{
    static char *split_string[2];
    // Check if there is any previous allocation.
    // Initialize split_string.
    memset(split_string, 0, 2);

    // Check if the delim actually exists in string.
    if (strstr(string, delim) == NULL) return NULL;

    size_t delim_s = 0;
    size_t string_tmp_s = 0;
    while (string[delim_s])
    {
        if (string[delim_s] == delim[0]) break;
        ++delim_s;
    }
    // Calculate what remains to one part.
    size_t initial_string_s = strlen(string);
    if (initial_string_s > delim_s)
        string_tmp_s = initial_string_s - delim_s;
    else
        string_tmp_s = delim_s - initial_string_s;

    char *part_1;
    char *part_2;

    ALLOC_MEM(part_1, delim_s + 1, sizeof(char));
    ALLOC_MEM(part_2, string_tmp_s + 1, sizeof(char));
    memcpy(part_1, string, delim_s);
    memcpy(part_2, (string + delim_s + 1), string_tmp_s);

    split_string[0] = part_1;
    split_string[1] = part_2;

    return split_string;
}

static inline void split_string_free_mem(char **split) {
    FREE_MEM(split[0]);
    FREE_MEM(split[1]);
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

    // Make a copy of the device_file.
    char tmp[strlen((const char *) device_file)];
    strcpy(tmp, (const char *) device_file);

    char *curr_handler = strstr(tmp, "Handlers");
    char *curr_ev, *curr_event, **split;

    int i = 0;
    while (curr_handler)
    {
        curr_ev = strstr(curr_handler, "EV");
        split = split_string(curr_ev, "=");
        if (curr_ev == NULL) return NULL;

        if (!strcmp(curr_ev, "120013"))
        {
            curr_event = strstr(curr_handler, "event");
            split = split_string(curr_event, " ");
            curr_event = split[0];
            if (curr_event == NULL)
            {
                split_string_free_mem(split);
                curr_event = strstr(curr_handler, "event");
                curr_event = strtok(curr_event, "\n");
                if (curr_event == NULL) return NULL;
            }
            printf("%s\n", curr_event);
            retrieved_events[i++] = curr_event;
            split_string_free_mem(split);
        }
        curr_handler = strstr(curr_event, "Handlers");
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
    //printf("%s\n", retrieved_events[0]);

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
   /* initialize_signal();
    initialize_workers();
    // Make the two main threads.
    pthread_t discovery_t;
    pthread_t worker_maker_t;
    // Start the two main threads.
    if (pthread_create(&discovery_t, NULL, discovery_thread, NULL) != 0) return;
    if (pthread_create(&worker_maker_t, NULL, worker_maker_thread, NULL) != 0) return;

    pthread_join(discovery_t, NULL);
    pthread_join(worker_maker_t, NULL);

    destroy_workers();*/
    kb_discovery();
}