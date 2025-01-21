#include "xutil/platform/virtual_memory.h"
#include "xutil/platform/native_memory.h"

#if defined(XU_CONFIG_POSIX_HAVE_MMAP)
#    include "xutil/platform/posix/virtual_memory.c"
#else
xu_pointer_t xu_virtual_memory_malloc(xu_size_t size) { return xu_native_memory_malloc(size); }
xu_pointer_t xu_virtual_memory_ralloc(xu_pointer_t data, xu_size_t size) { return xu_native_memory_ralloc(data, size); }
xu_bool_t    xu_virtual_memory_free(xu_pointer_t data) { return xu_native_memory_free(data); }
#endif
