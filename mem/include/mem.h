#ifndef LINUX_KEYLOGGER_MEM_H
#define LINUX_KEYLOGGER_MEM_H

#include <malloc.h>
#include <logs/logger.h>

#define FAILED_ALLOC_MEMORY     "Failed to alloc memory"
#define FAILED_REALLOC_MEMORY   "Failed to realloc memory"

#ifndef ALLOC_MEM
#define ALLOC_MEM(VAR, SIZE, SIZE_OF_TYPE) {                            \
        VAR = calloc(SIZE, SIZE_OF_TYPE);                               \
        if (VAR == NULL) make_terminal_log(FAILED_ALLOC_MEMORY, 0);     \
} while(0)
#endif

#ifndef REALLOC_MEM
#define REALLOC_MEM(VAR, NEW_SIZE, SIZE_OF_TYPE) {                      \
        VAR = realloc(VAR, NEW_SIZE * SIZE_OF_TYPE);                    \
        if (VAR == NULL) make_terminal_log(FAILED_REALLOC_MEMORY, 0);   \
} while(0)
#endif


#endif
