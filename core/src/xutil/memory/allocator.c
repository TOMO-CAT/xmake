/* *******************************************************
 *  trace
 */
#define XU_TRACE_MODULE_NAME "allocator"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/memory/allocator.h"
#include "xutil/memory/impl/prefix.h"

/* *******************************************************
 *  globals
 */

// the allocator
xu_allocator_ref_t g_allocator = xu_null;

/* *******************************************************
 *  implementation
 */

xu_allocator_ref_t xu_allocator() { return g_allocator; }
xu_size_t          xu_allocator_type(xu_allocator_ref_t allocator)
{
    // check
    xu_assert_and_check_return_val(allocator, XU_ALLOCATOR_TYPE_DEFAULT);

    // get it
    return (xu_size_t)allocator->type;
}
xu_pointer_t xu_allocator_malloc_(xu_allocator_ref_t allocator, xu_size_t size __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // enter
    xu_bool_t lockit = !(allocator->flag & XU_ALLOCATOR_FLAG_NOLOCK);
    if (lockit) xu_spinlock_enter(&allocator->lock);

    // malloc it
    xu_pointer_t data = xu_null;
    if (allocator->malloc)
        data = allocator->malloc(allocator, size __xu_debug_args__);
    else if (allocator->large_malloc)
        data = allocator->large_malloc(allocator, size, xu_null __xu_debug_args__);

    // trace
    xu_trace_d("malloc(%lu): %p at %s(): %d, %s", size, data __xu_debug_args__);

    // check
    xu_assertf(data, "malloc(%lu) failed!", size);
    xu_assertf(!(((xu_size_t)data) & (XU_POOL_DATA_ALIGN - 1)), "malloc(%lu): unaligned data: %p", size, data);

    // leave
    if (lockit) xu_spinlock_leave(&allocator->lock);

    // ok?
    return data;
}
xu_pointer_t xu_allocator_malloc0_(xu_allocator_ref_t allocator, xu_size_t size __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // malloc it
    xu_pointer_t data = xu_allocator_malloc_(allocator, size __xu_debug_args__);

    // clear it
    if (data) xu_memset_(data, 0, size);

    // ok?
    return data;
}
xu_pointer_t xu_allocator_nalloc_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // nalloc it
    return xu_allocator_malloc_(allocator, item * size __xu_debug_args__);
}
xu_pointer_t xu_allocator_nalloc0_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // nalloc0 it
    xu_pointer_t data = xu_allocator_malloc_(allocator, item * size __xu_debug_args__);

    // clear it
    if (data) xu_memset_(data, 0, item * size);

    // ok?
    return data;
}
xu_pointer_t xu_allocator_ralloc_(xu_allocator_ref_t allocator, xu_pointer_t data, xu_size_t size __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // enter
    xu_bool_t lockit = !(allocator->flag & XU_ALLOCATOR_FLAG_NOLOCK);
    if (lockit) xu_spinlock_enter(&allocator->lock);

    // ralloc it
    xu_pointer_t data_new = xu_null;
    if (allocator->ralloc)
        data_new = allocator->ralloc(allocator, data, size __xu_debug_args__);
    else if (allocator->large_ralloc)
        data_new = allocator->large_ralloc(allocator, data, size, xu_null __xu_debug_args__);

    // trace
    xu_trace_d("ralloc(%p, %lu): %p at %s(): %d, %s", data, size, data_new __xu_debug_args__);

    // failed? dump it
#ifdef __xu_debug__
    if (!data_new)
    {
        // trace
        xu_trace_e("ralloc(%p, %lu) failed! at %s(): %lu, %s", data, size, func_, line_, file_);

        // dump data
        xu_pool_data_dump((xu_byte_t const*)data, xu_true, "[large_allocator]: [error]: ");

        // abort
        xu_abort();
    }
#endif

    // check
    xu_assertf(!(((xu_size_t)data_new) & (XU_POOL_DATA_ALIGN - 1)), "ralloc(%lu): unaligned data: %p", size, data);

    // leave
    if (lockit) xu_spinlock_leave(&allocator->lock);

    // ok?
    return data_new;
}
xu_bool_t xu_allocator_free_(xu_allocator_ref_t allocator, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_false);

    // enter
    xu_bool_t lockit = !(allocator->flag & XU_ALLOCATOR_FLAG_NOLOCK);
    if (lockit) xu_spinlock_enter(&allocator->lock);

    // trace
    xu_trace_d("free(%p): at %s(): %d, %s", data __xu_debug_args__);

    // free it
    xu_bool_t ok = xu_false;
    if (allocator->free)
        ok = allocator->free(allocator, data __xu_debug_args__);
    else if (allocator->large_free)
        ok = allocator->large_free(allocator, data __xu_debug_args__);

        // failed? dump it
#ifdef __xu_debug__
    if (!ok)
    {
        // trace
        xu_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        xu_pool_data_dump((xu_byte_t const*)data, xu_true, "[large_allocator]: [error]: ");

        // abort
        xu_abort();
    }
#endif

    // leave
    if (lockit) xu_spinlock_leave(&allocator->lock);

    // ok?
    return ok;
}
xu_pointer_t xu_allocator_large_malloc_(xu_allocator_ref_t allocator, xu_size_t size, xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // enter
    xu_bool_t lockit = !(allocator->flag & XU_ALLOCATOR_FLAG_NOLOCK);
    if (lockit) xu_spinlock_enter(&allocator->lock);

    // malloc it
    xu_pointer_t data = xu_null;
    if (allocator->large_malloc)
        data = allocator->large_malloc(allocator, size, real __xu_debug_args__);
    else if (allocator->malloc)
    {
        // malloc it
        if (real) *real = size;
        data = allocator->malloc(allocator, size __xu_debug_args__);
    }

    // trace
    xu_trace_d("large_malloc(%lu): %p at %s(): %d, %s", size, data __xu_debug_args__);

    // check
    xu_assertf(data, "malloc(%lu) failed!", size);
    xu_assertf(!(((xu_size_t)data) & (XU_POOL_DATA_ALIGN - 1)), "malloc(%lu): unaligned data: %p", size, data);
    xu_assert(!real || *real >= size);

    // leave
    if (lockit) xu_spinlock_leave(&allocator->lock);

    // ok?
    return data;
}
xu_pointer_t xu_allocator_large_malloc0_(xu_allocator_ref_t allocator, xu_size_t size,
                                         xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // malloc it
    xu_pointer_t data = xu_allocator_large_malloc_(allocator, size, real __xu_debug_args__);

    // clear it
    if (data) xu_memset_(data, 0, real ? *real : size);

    // ok
    return data;
}
xu_pointer_t xu_allocator_large_nalloc_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size,
                                        xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // malloc it
    return xu_allocator_large_malloc_(allocator, item * size, real __xu_debug_args__);
}
xu_pointer_t xu_allocator_large_nalloc0_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size,
                                         xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // malloc it
    xu_pointer_t data = xu_allocator_large_malloc_(allocator, item * size, real __xu_debug_args__);

    // clear it
    if (data) xu_memset_(data, 0, real ? *real : (item * size));

    // ok
    return data;
}
xu_pointer_t xu_allocator_large_ralloc_(xu_allocator_ref_t allocator, xu_pointer_t data, xu_size_t size,
                                        xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_null);

    // enter
    xu_bool_t lockit = !(allocator->flag & XU_ALLOCATOR_FLAG_NOLOCK);
    if (lockit) xu_spinlock_enter(&allocator->lock);

    // ralloc it
    xu_pointer_t data_new = xu_null;
    if (allocator->large_ralloc)
        data_new = allocator->large_ralloc(allocator, data, size, real __xu_debug_args__);
    else if (allocator->ralloc)
    {
        // ralloc it
        if (real) *real = size;
        data_new = allocator->ralloc(allocator, data, size __xu_debug_args__);
    }

    // trace
    xu_trace_d("large_ralloc(%p, %lu): %p at %s(): %d, %s", data, size, data_new __xu_debug_args__);

    // failed? dump it
#ifdef __xu_debug__
    if (!data_new)
    {
        // trace
        xu_trace_e("ralloc(%p, %lu) failed! at %s(): %lu, %s", data, size, func_, line_, file_);

        // dump data
        xu_pool_data_dump((xu_byte_t const*)data, xu_true, "[large_allocator]: [error]: ");

        // abort
        xu_abort();
    }
#endif

    // check
    xu_assert(!real || *real >= size);
    xu_assertf(!(((xu_size_t)data_new) & (XU_POOL_DATA_ALIGN - 1)), "ralloc(%lu): unaligned data: %p", size, data);

    // leave
    if (lockit) xu_spinlock_leave(&allocator->lock);

    // ok?
    return data_new;
}
xu_bool_t xu_allocator_large_free_(xu_allocator_ref_t allocator, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_false);

    // enter
    xu_bool_t lockit = !(allocator->flag & XU_ALLOCATOR_FLAG_NOLOCK);
    if (lockit) xu_spinlock_enter(&allocator->lock);

    // trace
    xu_trace_d("large_free(%p): at %s(): %d, %s", data __xu_debug_args__);

    // free it
    xu_bool_t ok = xu_false;
    if (allocator->large_free)
        ok = allocator->large_free(allocator, data __xu_debug_args__);
    else if (allocator->free)
        ok = allocator->free(allocator, data __xu_debug_args__);

        // failed? dump it
#ifdef __xu_debug__
    if (!ok)
    {
        // trace
        xu_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        xu_pool_data_dump((xu_byte_t const*)data, xu_true, "[large_allocator]: [error]: ");

        // abort
        xu_abort();
    }
#endif

    // leave
    if (lockit) xu_spinlock_leave(&allocator->lock);

    // ok?
    return ok;
}
xu_pointer_t xu_allocator_align_malloc_(xu_allocator_ref_t allocator, xu_size_t size, xu_size_t align __xu_debug_decl__)
{
    // check
    xu_assertf(!(align & 3), "invalid alignment size: %lu", align);
    xu_check_return_val(!(align & 3), xu_null);

    // malloc it
    xu_byte_t* data = (xu_byte_t*)xu_allocator_malloc_(allocator, size + align __xu_debug_args__);
    xu_check_return_val(data, xu_null);

    // the different bytes
    xu_byte_t diff = (xu_byte_t)((~(xu_long_t)data) & (align - 1)) + 1;

    // adjust the address
    data += diff;

    // check
    xu_assert(!((xu_size_t)data & (align - 1)));

    // save the different bytes
    data[-1] = diff;

    // ok?
    return (xu_pointer_t)data;
}
xu_pointer_t xu_allocator_align_malloc0_(xu_allocator_ref_t allocator, xu_size_t size,
                                         xu_size_t align __xu_debug_decl__)
{
    // malloc it
    xu_pointer_t data = xu_allocator_align_malloc_(allocator, size, align __xu_debug_args__);
    xu_assert_and_check_return_val(data, xu_null);

    // clear it
    xu_memset(data, 0, size);

    // ok
    return data;
}
xu_pointer_t xu_allocator_align_nalloc_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size,
                                        xu_size_t align __xu_debug_decl__)
{
    return xu_allocator_align_malloc_(allocator, item * size, align __xu_debug_args__);
}
xu_pointer_t xu_allocator_align_nalloc0_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size,
                                         xu_size_t align __xu_debug_decl__)
{
    // nalloc it
    xu_pointer_t data = xu_allocator_align_nalloc_(allocator, item, size, align __xu_debug_args__);
    xu_assert_and_check_return_val(data, xu_null);

    // clear it
    xu_memset(data, 0, item * size);

    // ok
    return data;
}
xu_pointer_t xu_allocator_align_ralloc_(xu_allocator_ref_t allocator, xu_pointer_t data, xu_size_t size,
                                        xu_size_t align __xu_debug_decl__)
{
    // check align
    xu_assertf(!(align & 3), "invalid alignment size: %lu", align);
    xu_check_return_val(!(align & 3), xu_null);

    // ralloc?
    xu_byte_t diff = 0;
    if (data)
    {
        // check address
        xu_assertf(!((xu_size_t)data & (align - 1)), "invalid address %p", data);
        xu_check_return_val(!((xu_size_t)data & (align - 1)), xu_null);

        // the different bytes
        diff = ((xu_byte_t*)data)[-1];

        // adjust the address
        data = (xu_byte_t*)data - diff;

        // ralloc it
        data = xu_allocator_ralloc_(allocator, data, size + align __xu_debug_args__);
        xu_check_return_val(data, xu_null);
    }
    // no data?
    else
    {
        // malloc it directly
        data = xu_allocator_malloc_(allocator, size + align __xu_debug_args__);
        xu_check_return_val(data, xu_null);
    }

    // the different bytes
    diff = (xu_byte_t)((~(xu_long_t)data) & (align - 1)) + 1;

    // adjust the address
    data = (xu_byte_t*)data + diff;

    // check
    xu_assert(!((xu_size_t)data & (align - 1)));

    // save the different bytes
    ((xu_byte_t*)data)[-1] = diff;

    // ok?
    return data;
}
xu_bool_t xu_allocator_align_free_(xu_allocator_ref_t allocator, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(data, xu_false);
    xu_assert(!((xu_size_t)data & 3));

    // the different bytes
    xu_byte_t diff = ((xu_byte_t*)data)[-1];

    // adjust the address
    data = (xu_byte_t*)data - diff;

    // free it
    return xu_allocator_free_(allocator, data __xu_debug_args__);
}
xu_void_t xu_allocator_clear(xu_allocator_ref_t allocator)
{
    // check
    xu_assert_and_check_return(allocator);

    // enter
    xu_bool_t lockit = !(allocator->flag & XU_ALLOCATOR_FLAG_NOLOCK);
    if (lockit) xu_spinlock_enter(&allocator->lock);

    // clear it
    if (allocator->clear) allocator->clear(allocator);

    // leave
    if (lockit) xu_spinlock_leave(&allocator->lock);
}
xu_void_t xu_allocator_exit(xu_allocator_ref_t allocator)
{
    // check
    xu_assert_and_check_return(allocator);

    // clear it first
    xu_allocator_clear(allocator);

    // exit it
    if (allocator->exit) allocator->exit(allocator);
}
#ifdef __xu_debug__
xu_void_t xu_allocator_dump(xu_allocator_ref_t allocator)
{
    // check
    xu_assert_and_check_return(allocator);

    // enter
    xu_bool_t lockit = !(allocator->flag & XU_ALLOCATOR_FLAG_NOLOCK);
    if (lockit) xu_spinlock_enter(&allocator->lock);

    // dump it
    if (allocator->dump) allocator->dump(allocator);

    // leave
    if (lockit) xu_spinlock_leave(&allocator->lock);
}
xu_bool_t xu_allocator_have(xu_allocator_ref_t allocator, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return_val(allocator, xu_false);

    /* have it?
     *
     * @note cannot use locker and ensure thread safe
     */
    return allocator->have ? allocator->have(allocator, data) : xu_false;
}
#endif
