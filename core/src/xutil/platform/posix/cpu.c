#include "xutil/platform/cpu.h"

#include <unistd.h>

xu_size_t xu_cpu_count()
{
    // we will pre-initialize it in xu_platform_init()
    static xu_size_t ncpu = -1;
    if (ncpu == -1)
    {
        xu_size_t count = sysconf(_SC_NPROCESSORS_ONLN);
        if (!count) count = sysconf(_SC_NPROCESSORS_CONF);
        if (!count) count = 1;
        ncpu = count;
    }
    return ncpu;
}
