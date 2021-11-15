#ifndef LINUX_KEYLOGGER_MEM_H
#define LINUX_KEYLOGGER_MEM_H

#include <malloc.h>

#define ALLOC_MEM(VAR, SIZE, SIZE_OF_TYPE) {                \
        VAR = calloc(SIZE, SIZE_OF_TYPE);                   \
        if (VAR == NULL) exit(1);                           \
} while(0)

#define REALLOC_MEM(VAR, NEW_SIZE, SIZE_OF_TYPE) {          \
        VAR = realloc(VAR, NEW_SIZE * SIZE_OF_TYPE);        \
        if (VAR == NULL) exit(1);                           \
} while(0)



#endif
