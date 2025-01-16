#include "xutil/platform/cpu.h"

#if defined(XU_CONFIG_POSIX_HAVE_SYSCONF)
#    include "xutil/platform/posix/cpu.c"
#else
xu_size_t xu_cpu_count() { return 1; }
#endif
