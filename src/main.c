#include <kb_mapper.h>
#include <unistd.h>
#include <logs/logger.h>

#define NO_ROOT "Failed to start, run as root"

int main() {
    if (geteuid() != 0) {
        make_terminal_log(NO_ROOT, 0);
        return 0;
    }

    map_keyboards();
    return 0;
}
