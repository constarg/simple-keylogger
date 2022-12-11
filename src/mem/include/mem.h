/* mem.h */
#ifndef LINUX_KEYLOGGER_MEM_H
#define LINUX_KEYLOGGER_MEM_H

#include <malloc.h>
#include <logs/logger.h>

#define FAILED_ALLOC_MEMORY     "Failed to alloc memory"
#define FAILED_REALLOC_MEMORY   "Failed to realloc memory"

/**
 * Allocate memory for a variable and check if the operation succeed.
 */
#define ALLOC_MEM(VAR, SIZE, SIZE_OF_TYPE) {                            \
        VAR = calloc(SIZE, SIZE_OF_TYPE);                               \
        if (VAR == NULL) make_terminal_log(FAILED_ALLOC_MEMORY, 0);     \
}

/**
 * Re allocate memory for a variable and check if the operation succeed.
 */
#define REALLOC_MEM(VAR, NEW_SIZE, SIZE_OF_TYPE) {                      \
        VAR = realloc(VAR, NEW_SIZE * SIZE_OF_TYPE);                    \
        if (VAR == NULL) make_terminal_log(FAILED_REALLOC_MEMORY, 0);   \
}

/**
 * Free the allocated memory and set the variable to point in NULL value
 * to be safe.
 */
#define FREE_MEM(VAR) {                                                 \
        free(VAR);                                                      \
        VAR = NULL;                                                     \
}

#endif
