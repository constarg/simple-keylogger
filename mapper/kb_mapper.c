/* kb_mapper.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <kb_mapper.h>


#define DEVICE_LOCATION         "/proc/bus/input/devices"
#define DEVICE_HANDLER_PATH     "/dev/input/"
#define KEYBOARD_ID             "EV=120013"

