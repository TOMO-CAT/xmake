#include "xutil/memory/small_allocator.h"

/* *******************************************************
 * types
 */

// the small allocator type
typedef struct __xu_small_allocator_t
{
    // the base
    xu_allocator_t base;

    // the large allocator
    xu_allocator_ref_t large_allocator;

    // the fixed pool
    xu_fixed_pool_ref_t fixed_pool[12];

} xu_small_allocator_t, *xu_small_allocator_ref_t;

/* *******************************************************
 * declaration
 */

xu_fixed_pool_ref_t xu_fixed_pool_init_(xu_allocator_ref_t large_allocator, xu_size_t slot_size, xu_size_t item_size,
                                        xu_bool_t for_small_allocator, xu_fixed_pool_item_init_func_t item_init,
                                        xu_fixed_pool_item_exit_func_t item_exit, xu_cpointer_t priv);

/* *******************************************************
 * private implementation
 */

static xu_fixed_pool_ref_t xu_small_allocator_find_fixed(xu_small_allocator_ref_t allocator, xu_size_t size)
{
    // check
    xu_assert(allocator && size && size <= XU_SMALL_ALLOCATOR_DATA_MAXN);

    // done
    xu_fixed_pool_ref_t fixed_pool = xu_null;
    do
    {
        // the fixed pool index
        xu_size_t index = 0;
        xu_size_t space = 0;
        if (size > 64 && size < 193)
        {
            if (size < 97)
            {
                index = 3;
                space = 96;
            }
            else if (size > 128)
            {
                index = 5;
                space = 192;
            }
            else
            {
                index = 4;
                space = 128;
            }
        }
        else if (size > 192 && size < 513)
        {
            if (size < 257)
            {
                index = 6;
                space = 256;
            }
            else if (size > 384)
            {
                index = 8;
                space = 512;
            }
            else
            {
                index = 7;
                space = 384;
            }
        }
        else if (size < 65)
        {
            if (size < 17)
            {
                index = 0;
                space = 16;
            }
            else if (size > 32)
            {
                index = 2;
                space = 64;
            }
            else
            {
                index = 1;
                space = 32;
            }
        }
        else
        {
            if (size < 1025)
            {
                index = 9;
                space = 1024;
            }
            else if (size > 2048)
            {
                index = 11;
                space = 3072;
            }
            else
            {
                index = 10;
                space = 2048;
            }
        }

        // trace
        xu_trace_d("find: size: %lu => index: %lu, space: %lu", size, index, space);

        // make fixed pool if not exists
        if (!allocator->fixed_pool[index])
            allocator->fixed_pool[index] =
                xu_fixed_pool_init_(allocator->large_allocator, 0, space, xu_true, xu_null, xu_null, xu_null);
        xu_assert_and_check_break(allocator->fixed_pool[index]);

        // ok
        fixed_pool = allocator->fixed_pool[index];

    } while (0);

    // ok?
    return fixed_pool;
}
#ifdef __xu_debug__
static xu_bool_t xu_small_allocator_item_check(xu_pointer_t data, xu_cpointer_t priv)
{
    // check
    xu_fixed_pool_ref_t fixed_pool = (xu_fixed_pool_ref_t)priv;
    xu_assert(fixed_pool && data);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // the data head
        xu_pool_data_head_t* data_head = &(((xu_pool_data_head_t*)data)[-1]);
        xu_assertf(data_head->debug.magic == XU_POOL_DATA_MAGIC, "invalid data: %p", data);

        // the data space
        xu_size_t space = xu_fixed_pool_item_size(fixed_pool);
        xu_assert_and_check_break(space >= data_head->size);

        // check underflow
        xu_assertf(space == data_head->size || ((xu_byte_t*)data)[data_head->size] == XU_POOL_DATA_PATCH,
                   "data underflow");

        // ok
        ok = xu_true;

    } while (0);

    // continue?
    return ok;
}
#endif
static xu_void_t xu_small_allocator_exit(xu_allocator_ref_t self)
{
    // check
    xu_small_allocator_ref_t allocator = (xu_small_allocator_ref_t)self;
    xu_assert_and_check_return(allocator && allocator->large_allocator);

    // enter
    xu_spinlock_enter(&allocator->base.lock);

    // exit fixed pool
    xu_size_t i = 0;
    xu_size_t n = xu_arrayn(allocator->fixed_pool);
    for (i = 0; i < n; i++)
    {
        // exit it
        if (allocator->fixed_pool[i]) xu_fixed_pool_exit(allocator->fixed_pool[i]);
        allocator->fixed_pool[i] = xu_null;
    }

    // leave
    xu_spinlock_leave(&allocator->base.lock);

    // exit lock
    xu_spinlock_exit(&allocator->base.lock);

    // exit pool
    xu_allocator_large_free(allocator->large_allocator, allocator);
}
static xu_void_t xu_small_allocator_clear(xu_allocator_ref_t self)
{
    // check
    xu_small_allocator_ref_t allocator = (xu_small_allocator_ref_t)self;
    xu_assert_and_check_return(allocator && allocator->large_allocator);

    // clear fixed pool
    xu_size_t i = 0;
    xu_size_t n = xu_arrayn(allocator->fixed_pool);
    for (i = 0; i < n; i++)
    {
        // clear it
        if (allocator->fixed_pool[i]) xu_fixed_pool_clear(allocator->fixed_pool[i]);
    }
}
static xu_pointer_t xu_small_allocator_malloc(xu_allocator_ref_t self, xu_size_t size __xu_debug_decl__)
{
    // check
    xu_small_allocator_ref_t allocator = (xu_small_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator && allocator->large_allocator && size, xu_null);
    xu_assert_and_check_return_val(size <= XU_SMALL_ALLOCATOR_DATA_MAXN, xu_null);

    // done
    xu_pointer_t data = xu_null;
    do
    {
        // the fixed pool
        xu_fixed_pool_ref_t fixed_pool = xu_small_allocator_find_fixed(allocator, size);
        xu_assert_and_check_break(fixed_pool);

        // done
        data = xu_fixed_pool_malloc_(fixed_pool __xu_debug_args__);
        xu_assert_and_check_break(data);

        // the data head
        xu_pool_data_head_t* data_head = &(((xu_pool_data_head_t*)data)[-1]);
        xu_assert(data_head->debug.magic == XU_POOL_DATA_MAGIC);

#ifdef __xu_debug__
        // fill the patch bytes
        if (data_head->size > size) xu_memset_((xu_byte_t*)data + size, XU_POOL_DATA_PATCH, data_head->size - size);
#endif

        // update size
        data_head->size = size;

    } while (0);

    // check
    xu_assertf(data, "malloc(%lu) failed!", size);

    // ok?
    return data;
}
static xu_pointer_t xu_small_allocator_ralloc(xu_allocator_ref_t self, xu_pointer_t data,
                                              xu_size_t size __xu_debug_decl__)
{
    // check
    xu_small_allocator_ref_t allocator = (xu_small_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator && allocator->large_allocator && data && size, xu_null);
    xu_assert_and_check_return_val(size <= XU_SMALL_ALLOCATOR_DATA_MAXN, xu_null);

    // done
    xu_pointer_t data_new = xu_null;
    do
    {
        // the old data head
        xu_pool_data_head_t* data_head_old = &(((xu_pool_data_head_t*)data)[-1]);
        xu_assertf(data_head_old->debug.magic == XU_POOL_DATA_MAGIC, "ralloc invalid data: %p", data);

        // the old fixed pool
        xu_fixed_pool_ref_t fixed_pool_old = xu_small_allocator_find_fixed(allocator, data_head_old->size);
        xu_assert_and_check_break(fixed_pool_old);

        // the old data space
        xu_size_t space_old = xu_fixed_pool_item_size(fixed_pool_old);
        xu_assert_and_check_break(space_old >= data_head_old->size);

        // check underflow
        xu_assertf(space_old == data_head_old->size || ((xu_byte_t*)data)[data_head_old->size] == XU_POOL_DATA_PATCH,
                   "data underflow");

        // the new fixed pool
        xu_fixed_pool_ref_t fixed_pool_new = xu_small_allocator_find_fixed(allocator, size);
        xu_assert_and_check_break(fixed_pool_new);

        // same space?
        if (fixed_pool_old == fixed_pool_new)
        {
#ifdef __xu_debug__
            // fill the patch bytes
            if (data_head_old->size > size)
                xu_memset_((xu_byte_t*)data + size, XU_POOL_DATA_PATCH, data_head_old->size - size);
#endif
            // only update size
            data_head_old->size = size;

            // ok
            data_new = data;
            break;
        }

        // make the new data
        data_new = xu_fixed_pool_malloc_(fixed_pool_new __xu_debug_args__);
        xu_assert_and_check_break(data_new);

        // the new data head
        xu_pool_data_head_t* data_head_new = &(((xu_pool_data_head_t*)data_new)[-1]);
        xu_assert(data_head_new->debug.magic == XU_POOL_DATA_MAGIC);

#ifdef __xu_debug__
        // fill the patch bytes
        if (data_head_new->size > size)
            xu_memset_((xu_byte_t*)data_new + size, XU_POOL_DATA_PATCH, data_head_new->size - size);
#endif

        // update size
        data_head_new->size = size;

        // copy the old data
        xu_memcpy_(data_new, data, xu_min(data_head_old->size, size));

        // free the old data
        xu_fixed_pool_free_(fixed_pool_old, data __xu_debug_args__);

    } while (0);

    // ok
    return data_new;
}
static xu_bool_t xu_small_allocator_free(xu_allocator_ref_t self, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_small_allocator_ref_t allocator = (xu_small_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator && allocator->large_allocator && data, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // the data head
        xu_pool_data_head_t* data_head = &(((xu_pool_data_head_t*)data)[-1]);
        xu_assertf(data_head->debug.magic == XU_POOL_DATA_MAGIC, "free invalid data: %p", data);

        // the fixed pool
        xu_fixed_pool_ref_t fixed_pool = xu_small_allocator_find_fixed(allocator, data_head->size);
        xu_assert_and_check_break(fixed_pool);

        // the data space
        xu_size_t space = xu_fixed_pool_item_size(fixed_pool);
        xu_assert_and_check_break(space >= data_head->size);

        // check underflow
        xu_assertf(space == data_head->size || ((xu_byte_t*)data)[data_head->size] == XU_POOL_DATA_PATCH,
                   "data underflow");

        // done
        ok = xu_fixed_pool_free_(fixed_pool, data __xu_debug_args__);

    } while (0);

    // ok?
    return ok;
}
#ifdef __xu_debug__
static xu_void_t xu_small_allocator_dump(xu_allocator_ref_t self)
{
    // check
    xu_small_allocator_ref_t allocator = (xu_small_allocator_ref_t)self;
    xu_assert_and_check_return(allocator && allocator->large_allocator);

    // trace
    xu_trace_i("");

    // dump fixed pool
    xu_size_t i = 0;
    xu_size_t n = xu_arrayn(allocator->fixed_pool);
    for (i = 0; i < n; i++)
    {
        // exists?
        if (allocator->fixed_pool[i])
        {
            // check it
            xu_fixed_pool_walk(allocator->fixed_pool[i], xu_small_allocator_item_check,
                               (xu_cpointer_t)allocator->fixed_pool[i]);

            // dump it
            xu_fixed_pool_dump(allocator->fixed_pool[i]);
        }
    }
}
static xu_bool_t xu_small_allocator_have(xu_allocator_ref_t self, xu_cpointer_t data)
{
    // check
    xu_small_allocator_ref_t allocator = (xu_small_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator && allocator->large_allocator, xu_false);

    // have it?
    return xu_allocator_have(allocator->large_allocator, data);
}
#endif

/* *******************************************************
 * implementation
 */
xu_allocator_ref_t xu_small_allocator_init(xu_allocator_ref_t large_allocator)
{
    // done
    xu_bool_t                ok        = xu_false;
    xu_small_allocator_ref_t allocator = xu_null;
    do
    {
        // no allocator? uses the global allocator
        if (!large_allocator) large_allocator = xu_allocator();
        xu_assert_and_check_break(large_allocator);

        // make allocator
        allocator = (xu_small_allocator_ref_t)xu_allocator_large_malloc0(large_allocator, sizeof(xu_small_allocator_t),
                                                                         xu_null);
        xu_assert_and_check_break(allocator);

        // init large allocator
        allocator->large_allocator = large_allocator;

        // init base
        allocator->base.type   = XU_ALLOCATOR_TYPE_SMALL;
        allocator->base.flag   = XU_ALLOCATOR_FLAG_NONE;
        allocator->base.malloc = xu_small_allocator_malloc;
        allocator->base.ralloc = xu_small_allocator_ralloc;
        allocator->base.free   = xu_small_allocator_free;
        allocator->base.clear  = xu_small_allocator_clear;
        allocator->base.exit   = xu_small_allocator_exit;
#ifdef __xu_debug__
        allocator->base.dump = xu_small_allocator_dump;
        allocator->base.have = xu_small_allocator_have;
#endif

        // init lock
        if (!xu_spinlock_init(&allocator->base.lock)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (allocator) xu_small_allocator_exit((xu_allocator_ref_t)allocator);
        allocator = xu_null;
    }

    // ok?
    return (xu_allocator_ref_t)allocator;
}
