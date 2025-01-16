#include "xutil/memory/default_allocator.h"
#include "xutil/memory/large_allocator.h"
#include "xutil/memory/small_allocator.h"
#include "xutil/platform/native_memory.h"
#include "xutil/platform/page.h"
#include "xutil/utils/singleton.h"

/* *******************************************************
 * types
 * *******************************************************
 */

// the default allocator type
typedef struct __xu_default_allocator_t
{
    // the base
    xu_allocator_t base;

    // the large allocator
    xu_allocator_ref_t large_allocator;

    // the small allocator
    xu_allocator_ref_t small_allocator;

} xu_default_allocator_t, *xu_default_allocator_ref_t;

/* *******************************************************
 * private implementation
 * *******************************************************
 */
static xu_void_t xu_default_allocator_exit(xu_allocator_ref_t self)
{
    // check
    xu_default_allocator_ref_t allocator = (xu_default_allocator_ref_t)self;
    xu_assert_and_check_return(allocator);

    // enter
    xu_spinlock_enter(&allocator->base.lock);

    // exit small allocator
    if (allocator->small_allocator) xu_allocator_exit(allocator->small_allocator);
    allocator->small_allocator = xu_null;

    // leave
    xu_spinlock_leave(&allocator->base.lock);

    // exit lock
    xu_spinlock_exit(&allocator->base.lock);

    // exit allocator
    if (allocator->large_allocator) xu_allocator_large_free(allocator->large_allocator, allocator);
}
static xu_pointer_t xu_default_allocator_malloc(xu_allocator_ref_t self, xu_size_t size __xu_debug_decl__)
{
    // check
    xu_default_allocator_ref_t allocator = (xu_default_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator, xu_null);

    // check
    xu_assert_and_check_return_val(allocator->large_allocator && allocator->small_allocator && size, xu_null);

    // done
    return size <= XU_SMALL_ALLOCATOR_DATA_MAXN
               ? xu_allocator_malloc_(allocator->small_allocator, size __xu_debug_args__)
               : xu_allocator_large_malloc_(allocator->large_allocator, size, xu_null __xu_debug_args__);
}
static xu_pointer_t xu_default_allocator_ralloc(xu_allocator_ref_t self, xu_pointer_t data,
                                                xu_size_t size __xu_debug_decl__)
{
    // check
    xu_default_allocator_ref_t allocator = (xu_default_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator, xu_null);

    // check
    xu_assert_and_check_return_val(allocator && allocator->large_allocator && allocator->small_allocator && size,
                                   xu_null);

    // done
    xu_pointer_t data_new = xu_null;
    do
    {
        // no data?
        if (!data)
        {
            // malloc it directly
            data_new = size <= XU_SMALL_ALLOCATOR_DATA_MAXN
                           ? xu_allocator_malloc_(allocator->small_allocator, size __xu_debug_args__)
                           : xu_allocator_large_malloc_(allocator->large_allocator, size, xu_null __xu_debug_args__);
            break;
        }

        // the data head
        xu_pool_data_head_t* data_head = &(((xu_pool_data_head_t*)data)[-1]);
        xu_assertf(data_head->debug.magic == XU_POOL_DATA_MAGIC, "ralloc invalid data: %p", data);
        xu_assert_and_check_break(data_head->size);

        // small => small
        if (data_head->size <= XU_SMALL_ALLOCATOR_DATA_MAXN && size <= XU_SMALL_ALLOCATOR_DATA_MAXN)
            data_new = xu_allocator_ralloc_(allocator->small_allocator, data, size __xu_debug_args__);
        // small => large
        else if (data_head->size <= XU_SMALL_ALLOCATOR_DATA_MAXN)
        {
            // make the new data
            data_new = xu_allocator_large_malloc_(allocator->large_allocator, size, xu_null __xu_debug_args__);
            xu_assert_and_check_break(data_new);

            // copy the old data
            xu_memcpy_(data_new, data, xu_min(data_head->size, size));

            // free the old data
            xu_allocator_free_(allocator->small_allocator, data __xu_debug_args__);
        }
        // large => small
        else if (size <= XU_SMALL_ALLOCATOR_DATA_MAXN)
        {
            // make the new data
            data_new = xu_allocator_malloc_(allocator->small_allocator, size __xu_debug_args__);
            xu_assert_and_check_break(data_new);

            // copy the old data
            xu_memcpy_(data_new, data, xu_min(data_head->size, size));

            // free the old data
            xu_allocator_large_free_(allocator->large_allocator, data __xu_debug_args__);
        }
        // large => large
        else
            data_new = xu_allocator_large_ralloc_(allocator->large_allocator, data, size, xu_null __xu_debug_args__);

    } while (0);

    // ok?
    return data_new;
}
static xu_bool_t xu_default_allocator_free(xu_allocator_ref_t self, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_default_allocator_ref_t allocator = (xu_default_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator, xu_false);

    // check
    xu_assert_and_check_return_val(allocator->large_allocator && allocator->small_allocator && data, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // the data head
        xu_pool_data_head_t* data_head = &(((xu_pool_data_head_t*)data)[-1]);
        xu_assertf(data_head->debug.magic == XU_POOL_DATA_MAGIC, "free invalid data: %p", data);

        // free it
        ok = (data_head->size <= XU_SMALL_ALLOCATOR_DATA_MAXN)
                 ? xu_allocator_free_(allocator->small_allocator, data __xu_debug_args__)
                 : xu_allocator_large_free_(allocator->large_allocator, data __xu_debug_args__);

    } while (0);

    // ok?
    return ok;
}
#ifdef __xu_debug__
static xu_void_t xu_default_allocator_dump(xu_allocator_ref_t self)
{
    // check
    xu_default_allocator_ref_t allocator = (xu_default_allocator_ref_t)self;
    xu_assert_and_check_return(allocator && allocator->small_allocator);

    // dump allocator
    xu_allocator_dump(allocator->small_allocator);
}
static xu_bool_t xu_default_allocator_have(xu_allocator_ref_t self, xu_cpointer_t data)
{
    // check
    xu_default_allocator_ref_t allocator = (xu_default_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator && allocator->large_allocator, xu_false);

    // have it?
    return xu_allocator_have(allocator->large_allocator, data);
}
#endif
static xu_handle_t xu_default_allocator_instance_init(xu_cpointer_t* ppriv)
{
    // check
    xu_check_return_val(ppriv, xu_null);

    // the data and size
    xu_value_ref_t tuple = (xu_value_ref_t)*ppriv;
    xu_byte_t*     data  = (xu_byte_t*)tuple[0].ptr;
    xu_size_t      size  = tuple[1].ul;

    // clear the private data first
    *ppriv = xu_null;

    // done
    xu_bool_t          ok              = xu_false;
    xu_allocator_ref_t allocator       = xu_null;
    xu_allocator_ref_t large_allocator = xu_null;
    do
    {
        /* init the page first
         *
         * because this allocator may be called before xu_init()
         */
        if (!xu_page_init()) break;

        /* init the native memory first
         *
         * because this allocator may be called before xu_init()
         */
        if (!xu_native_memory_init()) break;

        // init large allocator
        large_allocator = xu_large_allocator_init(data, size);
        xu_assert_and_check_break(large_allocator);

        // init allocator
        allocator = xu_default_allocator_init(large_allocator);
        xu_assert_and_check_break(allocator);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit large allocator
        if (large_allocator) xu_allocator_exit(large_allocator);
        large_allocator = xu_null;
    }

    // ok?
    return (xu_handle_t)allocator;
}
static xu_void_t xu_default_allocator_instance_exit(xu_handle_t self, xu_cpointer_t priv)
{
    // check
    xu_default_allocator_ref_t allocator = (xu_default_allocator_ref_t)self;
    xu_assert_and_check_return(allocator);

    // the large allocator
    xu_allocator_ref_t large_allocator = allocator->large_allocator;
    xu_assert_and_check_return(large_allocator);

#ifdef __xu_debug__
    // dump allocator
    if (allocator) xu_allocator_dump((xu_allocator_ref_t)allocator);
#endif

    // exit allocator
    if (allocator) xu_allocator_exit((xu_allocator_ref_t)allocator);
    allocator = xu_null;

#ifdef __xu_debug__
    // dump large allocator
    xu_allocator_dump(large_allocator);
#endif

    // exit large allocator
    xu_allocator_exit(large_allocator);
    large_allocator = xu_null;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_allocator_ref_t xu_default_allocator(xu_byte_t* data, xu_size_t size)
{
    /* init singleton first
     *
     * because this allocator may be called before xu_init()
     */
    if (!xu_singleton_init()) return xu_null;

    // init tuple
    xu_value_t tuple[2];
    tuple[0].ptr = (xu_pointer_t)data;
    tuple[1].ul  = size;

    // get it
    return (xu_allocator_ref_t)xu_singleton_instance(XU_SINGLETON_TYPE_DEFAULT_ALLOCATOR,
                                                     xu_default_allocator_instance_init,
                                                     xu_default_allocator_instance_exit, xu_null, tuple);
}
xu_allocator_ref_t xu_default_allocator_init(xu_allocator_ref_t large_allocator)
{
    // check
    xu_assert_and_check_return_val(large_allocator, xu_null);

    // done
    xu_bool_t                  ok        = xu_false;
    xu_default_allocator_ref_t allocator = xu_null;
    do
    {
        // make allocator
        allocator = (xu_default_allocator_ref_t)xu_allocator_large_malloc0(large_allocator,
                                                                           sizeof(xu_default_allocator_t), xu_null);
        xu_assert_and_check_break(allocator);

        // init base
        allocator->base.type   = XU_ALLOCATOR_TYPE_DEFAULT;
        allocator->base.flag   = XU_ALLOCATOR_FLAG_NONE;
        allocator->base.malloc = xu_default_allocator_malloc;
        allocator->base.ralloc = xu_default_allocator_ralloc;
        allocator->base.free   = xu_default_allocator_free;
        allocator->base.exit   = xu_default_allocator_exit;
#ifdef __xu_debug__
        allocator->base.dump = xu_default_allocator_dump;
        allocator->base.have = xu_default_allocator_have;
#endif

        // init lock
        if (!xu_spinlock_init(&allocator->base.lock)) break;

        // init allocator
        allocator->large_allocator = large_allocator;
        allocator->small_allocator = xu_small_allocator_init(large_allocator);
        xu_assert_and_check_break(allocator->small_allocator);

        // register lock profiler
#ifdef XU_LOCK_PROFILER_ENABLE
        xu_lock_profiler_register(xu_lock_profiler(), (xu_pointer_t)&allocator->base.lock, XU_TRACE_MODULE_NAME);
#endif

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (allocator) xu_default_allocator_exit((xu_allocator_ref_t)allocator);
        allocator = xu_null;
    }

    // ok?
    return (xu_allocator_ref_t)allocator;
}
