#include "xutil/platform/native_memory.h"

#include <stdlib.h>

/* *******************************************************
 *  implementation
 * *******************************************************
 */
xu_bool_t    xu_native_memory_init() { return xu_true; }
xu_void_t    xu_native_memory_exit() {}
xu_pointer_t xu_native_memory_malloc(xu_size_t size)
{
    // check
    xu_check_return_val(size, xu_null);

    // malloc it
    return malloc(size);
}
xu_pointer_t xu_native_memory_malloc0(xu_size_t size)
{
    // check
    xu_check_return_val(size, xu_null);

    // malloc0 it
    return calloc(1, size);
}
xu_pointer_t xu_native_memory_nalloc(xu_size_t item, xu_size_t size)
{
    // check
    xu_check_return_val(item && size, xu_null);

    // nalloc it
    return malloc(item * size);
}
xu_pointer_t xu_native_memory_nalloc0(xu_size_t item, xu_size_t size)
{
    // check
    xu_check_return_val(item && size, xu_null);

    // nalloc0 it
    return calloc(item, size);
}
xu_pointer_t xu_native_memory_ralloc(xu_pointer_t data, xu_size_t size)
{
    // no size? free it
    if (!size)
    {
        free(data);
        return xu_null;
    }
    // no data? malloc it
    else if (!data)
        return malloc(size);
    // realloc it
    else
        return realloc(data, size);
}
xu_bool_t xu_native_memory_free(xu_pointer_t data)
{
    // free it
    if (data) free(data);

    // ok
    return xu_true;
}
