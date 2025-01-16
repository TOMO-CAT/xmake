/* *******************************************************
 * trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "native_allocator"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/memory/native_allocator.h"
#include "xutil/platform/native_memory.h"
#include "xutil/utils/singleton.h"

/* *******************************************************
 * private implementation
 * *******************************************************
 */
static xu_pointer_t xu_native_allocator_malloc(xu_allocator_ref_t allocator, xu_size_t size __xu_debug_decl__)
{
    // trace
    xu_trace_d("malloc(%lu) at %s(): %lu, %s", size, func_, line_, file_);

    // malloc it
    return xu_native_memory_malloc(size);
}
static xu_pointer_t xu_native_allocator_ralloc(xu_allocator_ref_t allocator, xu_pointer_t data,
                                               xu_size_t size __xu_debug_decl__)
{
    // trace
    xu_trace_d("ralloc(%p, %lu) at %s(): %lu, %s", data, size, func_, line_, file_);

    // ralloc it
    return xu_native_memory_ralloc(data, size);
}
static xu_bool_t xu_native_allocator_free(xu_allocator_ref_t allocator, xu_pointer_t data __xu_debug_decl__)
{
    // trace
    xu_trace_d("free(%p) at %s(): %lu, %s", data, func_, line_, file_);

    // free it
    return xu_native_memory_free(data);
}
static xu_bool_t xu_native_allocator_instance_init(xu_handle_t instance, xu_cpointer_t priv)
{
    // check
    xu_allocator_ref_t allocator = (xu_allocator_ref_t)instance;
    xu_check_return_val(allocator, xu_false);

    /* init the native memory first
     *
     * because this allocator may be called before xu_init()
     */
    if (!xu_native_memory_init()) return xu_false;

    // init allocator
    allocator->type   = XU_ALLOCATOR_TYPE_NATIVE;
    allocator->flag   = XU_ALLOCATOR_FLAG_NOLOCK;
    allocator->malloc = xu_native_allocator_malloc;
    allocator->ralloc = xu_native_allocator_ralloc;
    allocator->free   = xu_native_allocator_free;
#ifdef __xu_debug__
    allocator->dump = xu_null;
    allocator->have = xu_null;
#endif

    // ok
    return xu_true;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_allocator_ref_t xu_native_allocator()
{
    // init
    static xu_atomic32_t  s_inited    = 0;
    static xu_allocator_t s_allocator = {0};

    // init the static instance
    xu_singleton_static_init(&s_inited, &s_allocator, xu_native_allocator_instance_init, xu_null);

    // ok
    return &s_allocator;
}
