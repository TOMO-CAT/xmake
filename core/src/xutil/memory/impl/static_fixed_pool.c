#include "xutil/memory/impl/static_fixed_pool.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

#ifdef XU_WORDS_BIGENDIAN

// allocate the index
#    define xu_static_fixed_pool_used_set1(used, i)                                                                    \
        do                                                                                                             \
        {                                                                                                              \
            (used)[(i) >> 3] |= (0x1 << (7 - ((i)&7)));                                                                \
        } while (0)

// free the index
#    define xu_static_fixed_pool_used_set0(used, i)                                                                    \
        do                                                                                                             \
        {                                                                                                              \
            (used)[(i) >> 3] &= ~(0x1 << (7 - ((i)&7)));                                                               \
        } while (0)

// the index have been allocated?
#    define xu_static_fixed_pool_used_bset(used, i) ((used)[(i) >> 3] & (0x1 << (7 - ((i)&7))))

// find the first free index
#    define xu_static_fixed_pool_find_free(v) xu_bits_fb0_be(v)

#else

// allocate the index
#    define xu_static_fixed_pool_used_set1(used, i)                                                                    \
        do                                                                                                             \
        {                                                                                                              \
            (used)[(i) >> 3] |= (0x1 << ((i)&7));                                                                      \
        } while (0)

// free the index
#    define xu_static_fixed_pool_used_set0(used, i)                                                                    \
        do                                                                                                             \
        {                                                                                                              \
            (used)[(i) >> 3] &= ~(0x1 << ((i)&7));                                                                     \
        } while (0)

// the index have been allocated?
#    define xu_static_fixed_pool_used_bset(used, i) ((used)[(i) >> 3] & (0x1 << ((i)&7)))

// find the first free index
#    define xu_static_fixed_pool_find_free(v) xu_bits_fb0_le(v)

#endif

// cache the predicted index
#define xu_static_fixed_pool_cache_pred(pool, i)                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        (pool)->pred_index = ((i) >> XU_CPU_SHIFT) + 1;                                                                \
    } while (0)

/* *******************************************************
 * types
 * *******************************************************
 */
// the static fixed pool type
typedef __xu_pool_data_aligned__ struct __xu_static_fixed_pool_t
{
    // the real data address after the used_info info
    xu_byte_t* data;

    // the data tail
    xu_byte_t* tail;

    // the used info
    xu_byte_t* used_info;

    // the used info size
    xu_size_t info_size;

    // the predict index
    xu_size_t pred_index;

    // the item size
    xu_size_t item_size;

    // the item space: head + item_size
    xu_size_t item_space;

    // the item count
    xu_size_t item_count;

    // the item maximum count
    xu_size_t item_maxn;

    // the data head size
    xu_uint16_t data_head_size;

    // for small allocator?
    xu_uint16_t for_small;

#ifdef __xu_debug__

    // the peak size
    xu_size_t peak_size;

    // the total size
    xu_size_t total_size;

    // the real size
    xu_size_t real_size;

    // the occupied size
    xu_size_t occupied_size;

    // the malloc count
    xu_size_t malloc_count;

    // the free count
    xu_size_t free_count;

    // the predict failed count
    xu_size_t pred_failed;

#endif

} __xu_pool_data_aligned__ xu_static_fixed_pool_t;

/* *******************************************************
 * private implementation
 * *******************************************************
 */

static xu_pool_data_empty_head_t* xu_static_fixed_pool_malloc_pred(xu_static_fixed_pool_t* pool)
{
    // check
    xu_assert_and_check_return_val(pool, xu_null);

    // done
    xu_pool_data_empty_head_t* data_head = xu_null;
    do
    {
        // exists the predict index?
        xu_check_break(pool->pred_index);

        // the predict index
        xu_size_t pred_index = pool->pred_index - 1;
        xu_assert((pred_index << XU_CPU_SHIFT) < pool->item_maxn);

        // the predict data
        xu_size_t* data = (xu_size_t*)pool->used_info + pred_index;

        // full?
        xu_check_break((*data) + 1);

        // the free bit index
        xu_size_t index = (pred_index << XU_CPU_SHIFT) + xu_static_fixed_pool_find_free(*data);

        // out of range?
        if (index >= pool->item_maxn)
        {
            // clear the pred index
            pool->pred_index = 0;
            break;
        }

        // check
        xu_assert(!xu_static_fixed_pool_used_bset(pool->used_info, index));

        // the data head
        data_head = (xu_pool_data_empty_head_t*)(pool->data + index * pool->item_space);

        // allocate it
        xu_static_fixed_pool_used_set1(pool->used_info, index);

        // the predict data is full
        if (!((*data) + 1))
        {
            // clear the predict index
            pool->pred_index = 0;

            // predict the next index
            if (index + 1 < pool->item_maxn && !xu_static_fixed_pool_used_bset(pool->used_info, index + 1))
                xu_static_fixed_pool_cache_pred(pool, index + 1);
        }

    } while (0);

#ifdef __xu_debug__
    // update the predict failed count
    if (!data_head) pool->pred_failed++;
#endif

    // ok?
    return data_head;
}

#if 1
static xu_pool_data_empty_head_t* xu_static_fixed_pool_malloc_find(xu_static_fixed_pool_t* pool)
{
    // check
    xu_assert_and_check_return_val(pool, xu_null);

    // init
    xu_size_t  i = 0;
    xu_size_t* p = (xu_size_t*)pool->used_info;
    xu_size_t* e = (xu_size_t*)(pool->used_info + pool->info_size);
    xu_byte_t* d = xu_null;

    // check align
    xu_assert_and_check_return_val(!(((xu_size_t)p) & (XU_CPU_BITBYTE - 1)), xu_null);

    // find the free chunk, item_space * 32|64 items
#    ifdef __xu_small__
    //  while (p < e && *p == 0xffffffff) p++;
    //  while (p < e && *p == 0xffffffffffffffffL) p++;
    while (p < e && !((*p) + 1))
        p++;
#    else
    while (p + 7 < e)
    {
        if (p[0] + 1)
        {
            p += 0;
            break;
        }
        if (p[1] + 1)
        {
            p += 1;
            break;
        }
        if (p[2] + 1)
        {
            p += 2;
            break;
        }
        if (p[3] + 1)
        {
            p += 3;
            break;
        }
        if (p[4] + 1)
        {
            p += 4;
            break;
        }
        if (p[5] + 1)
        {
            p += 5;
            break;
        }
        if (p[6] + 1)
        {
            p += 6;
            break;
        }
        if (p[7] + 1)
        {
            p += 7;
            break;
        }
        p += 8;
    }
    while (p < e && !(*p + 1))
        p++;
#    endif
    xu_check_return_val(p < e, xu_null);

    // find the free bit index
    xu_size_t m = pool->item_maxn;
    i           = (((xu_byte_t*)p - pool->used_info) << 3) + xu_static_fixed_pool_find_free(*p);
    xu_check_return_val(i < m, xu_null);

    // allocate it
    d = pool->data + i * pool->item_space;
    xu_static_fixed_pool_used_set1(pool->used_info, i);

    // predict this index if no full?
    if ((*p) + 1) xu_static_fixed_pool_cache_pred(pool, i);

    // ok?
    return (xu_pool_data_empty_head_t*)d;
}
#else
static xu_pool_data_empty_head_t* xu_static_fixed_pool_malloc_find(xu_static_fixed_pool_t* pool)
{
    // check
    xu_assert_and_check_return_val(pool, xu_null);

    // done
    xu_size_t  i = 0;
    xu_size_t  m = pool->item_maxn;
    xu_byte_t* p = pool->used_info;
    xu_byte_t  u = *p;
    xu_byte_t  b = 0;
    xu_byte_t* d = xu_null;
    for (i = 0; i < m; ++i)
    {
        // bit
        b = i & 0x07;

        // u++
        if (!b)
        {
            u = *p++;

            // skip the non-free byte
            // if (u == 0xff)
            if (!(u + 1))
            {
                i += 7;
                continue;
            }
        }

        // is free?
        // if (!xu_static_fixed_pool_used_bset(pool->used_info, i))
        if (!(u & (0x01 << b)))
        {
            // ok
            d = pool->data + i * pool->item_space;
            // xu_static_fixed_pool_used_set1(pool->used_info, i);
            *(p - 1) |= (0x01 << b);

            // predict the next block
            if (i + 1 < m && !xu_static_fixed_pool_used_bset(pool->used_info, i + 1))
                xu_static_fixed_pool_cache_pred(pool, i + 1);

            break;
        }
    }

    // ok?
    return (xu_pool_data_empty_head_t*)d;
}
#endif

#ifdef __xu_debug__
static xu_void_t xu_static_fixed_pool_check_data(xu_static_fixed_pool_t*          pool,
                                                 xu_pool_data_empty_head_t const* data_head)
{
    // check
    xu_assert_and_check_return(pool && data_head);

    // done
    xu_bool_t        ok   = xu_false;
    xu_byte_t const* data = (xu_byte_t const*)data_head + pool->data_head_size;
    do
    {
        // the index
        xu_size_t index = ((xu_byte_t*)data_head - pool->data) / pool->item_space;

        // check
        xu_assertf_pass_break(!(((xu_byte_t*)data_head - pool->data) % pool->item_space), "the invalid data: %p", data);
        xu_assertf_pass_break(xu_static_fixed_pool_used_bset(pool->used_info, index), "data have been freed: %p", data);
        xu_assertf_pass_break(data_head->debug.magic ==
                                  (pool->for_small ? XU_POOL_DATA_MAGIC : XU_POOL_DATA_EMPTY_MAGIC),
                              "the invalid data: %p", data);
        xu_assertf_pass_break(((xu_byte_t*)data)[pool->item_size] == XU_POOL_DATA_PATCH, "data underflow");

        // ok
        ok = xu_true;

    } while (0);

    // failed? dump it
    if (!ok)
    {
        // dump data
        xu_pool_data_dump(data, xu_true, "[static_fixed_pool]: [error]: ");

        // abort
        xu_abort();
    }
}
static xu_void_t xu_static_fixed_pool_check_next(xu_static_fixed_pool_t*          pool,
                                                 xu_pool_data_empty_head_t const* data_head)
{
    // check
    xu_assert_and_check_return(pool && data_head);

    // the index
    xu_size_t index = ((xu_byte_t*)data_head - pool->data) / pool->item_space;

    // check the next data
    if (index + 1 < pool->item_maxn && xu_static_fixed_pool_used_bset(pool->used_info, index + 1))
        xu_static_fixed_pool_check_data(pool, (xu_pool_data_empty_head_t*)((xu_byte_t*)data_head + pool->item_space));
}
static xu_void_t xu_static_fixed_pool_check_prev(xu_static_fixed_pool_t*          pool,
                                                 xu_pool_data_empty_head_t const* data_head)
{
    // check
    xu_assert_and_check_return(pool && data_head);

    // the index
    xu_size_t index = ((xu_byte_t*)data_head - pool->data) / pool->item_space;

    // check the prev data
    if (index && xu_static_fixed_pool_used_bset(pool->used_info, index - 1))
        xu_static_fixed_pool_check_data(pool, (xu_pool_data_empty_head_t*)((xu_byte_t*)data_head - pool->item_space));
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
xu_static_fixed_pool_ref_t xu_static_fixed_pool_init(xu_byte_t* data, xu_size_t size, xu_size_t item_size,
                                                     xu_bool_t for_small)
{
    // check
    xu_assert_and_check_return_val(data && size && item_size, xu_null);

    // align data and size
    xu_size_t diff = xu_align((xu_size_t)data, XU_POOL_DATA_ALIGN) - (xu_size_t)data;
    xu_assert_and_check_return_val(size > diff + sizeof(xu_static_fixed_pool_t), xu_null);
    size -= diff;
    data += diff;

    // init pool
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)data;
    xu_memset_(pool, 0, sizeof(xu_static_fixed_pool_t));

    // for small allocator?
    pool->for_small      = !!for_small;
    pool->data_head_size = for_small ? sizeof(xu_pool_data_head_t) : sizeof(xu_pool_data_empty_head_t);
#ifndef __xu_debug__
    // fix data alignment, because sizeof(xu_pool_data_empty_head_t) == 1 now.
    if (!for_small) pool->data_head_size = 0;
#endif
    xu_assert_and_check_return_val(!(pool->data_head_size & (XU_POOL_DATA_ALIGN - 1)), xu_null);

#ifdef __xu_debug__
    // init patch for checking underflow
    xu_size_t patch = 1;
#else
    xu_size_t patch = 0;
#endif

    // init the item space
    pool->item_space = pool->data_head_size + item_size + patch;
    pool->item_space = xu_align(pool->item_space, XU_POOL_DATA_ALIGN);
    xu_assert_and_check_return_val(pool->item_space > pool->data_head_size, xu_null);

    // init the used info
    pool->used_info = (xu_byte_t*)xu_align((xu_size_t)&pool[1], XU_POOL_DATA_ALIGN);
    xu_assert_and_check_return_val(data + size > pool->used_info, xu_null);

    /* init the item maxn
     *
     * used_info + maxn * item_space < left
     * align8(maxn) / 8 + maxn * item_space < left
     * (maxn + 7) / 8 + maxn * item_space < left
     * (maxn / 8) + (7 / 8) + maxn * item_space < left
     * maxn * (1 / 8 + item_space) < left - (7 / 8)
     * maxn < (left - (7 / 8)) / (1 / 8 + item_space)
     * maxn < (left * 8 - 7) / (1 + item_space * 8)
     */
    pool->item_maxn = (((data + size - pool->used_info) << 3) - 7) / (1 + (pool->item_space << 3));
    xu_assert_and_check_return_val(pool->item_maxn, xu_null);

    // init the used info size
    pool->info_size = xu_align(pool->item_maxn, XU_CPU_BITSIZE) >> 3;
    xu_assert_and_check_return_val(pool->info_size && !(pool->info_size & (XU_CPU_BITBYTE - 1)), xu_null);

    // clear the used info
    xu_memset_(pool->used_info, 0, pool->info_size);

    // init data
    pool->data = (xu_byte_t*)xu_align((xu_size_t)pool->used_info + pool->info_size, XU_POOL_DATA_ALIGN);
    xu_assert_and_check_return_val(data + size > pool->data, xu_null);
    xu_assert_and_check_return_val(pool->item_maxn * pool->item_space <= (xu_size_t)(data + size - pool->data + 1),
                                   xu_null);

    // init data tail
    pool->tail = pool->data + pool->item_maxn * pool->item_space;

    // init the item size
    pool->item_size = item_size;

    // init the item count
    pool->item_count = 0;

    // init the predict index
    pool->pred_index = 1;

    // trace
    xu_trace_d("init: data: %p, size: %lu, head_size: %lu, item_size: %lu, item_maxn: %lu, item_space: %lu", pool->data,
               size, pool->data - (xu_byte_t*)pool, item_size, pool->item_maxn, pool->item_space);

    // ok
    return (xu_static_fixed_pool_ref_t)pool;
}
xu_void_t xu_static_fixed_pool_exit(xu_static_fixed_pool_ref_t self)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return(pool);

    // clear it
    xu_static_fixed_pool_clear(self);

    // exit it
    xu_memset_(pool, 0, sizeof(xu_static_fixed_pool_t));
}
xu_size_t xu_static_fixed_pool_size(xu_static_fixed_pool_ref_t self)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, 0);

    // the item count
    return pool->item_count;
}
xu_size_t xu_static_fixed_pool_maxn(xu_static_fixed_pool_ref_t self)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, 0);

    // the item maximum count
    return pool->item_maxn;
}
xu_bool_t xu_static_fixed_pool_full(xu_static_fixed_pool_ref_t self)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, 0);

    // full?
    return pool->item_count == pool->item_maxn ? xu_true : xu_false;
}
xu_bool_t xu_static_fixed_pool_null(xu_static_fixed_pool_ref_t self)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, 0);

    // null?
    return !pool->item_count ? xu_true : xu_false;
}
xu_void_t xu_static_fixed_pool_clear(xu_static_fixed_pool_ref_t self)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return(pool);

    // clear used_info
    if (pool->used_info) xu_memset_(pool->used_info, 0, pool->info_size);

    // clear size
    pool->item_count = 0;

    // init the predict index
    pool->pred_index = 1;

    // clear info
#ifdef __xu_debug__
    pool->peak_size     = 0;
    pool->total_size    = 0;
    pool->real_size     = 0;
    pool->occupied_size = 0;
    pool->malloc_count  = 0;
    pool->free_count    = 0;
    pool->pred_failed   = 0;
#endif
}
xu_pointer_t xu_static_fixed_pool_malloc(xu_static_fixed_pool_ref_t self __xu_debug_decl__)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool && pool->item_space, xu_null);

    // done
    xu_pointer_t               data      = xu_null;
    xu_pool_data_empty_head_t* data_head = xu_null;
    do
    {
        // full?
        xu_check_break(pool->item_count < pool->item_maxn);

        // predict it?
        data_head = xu_static_fixed_pool_malloc_pred(pool);

        // find it
        if (!data_head) data_head = xu_static_fixed_pool_malloc_find(pool);
        xu_check_break(data_head);

        // the real data
        data = (xu_byte_t*)data_head + pool->data_head_size;

        // save the real size
        if (pool->for_small) ((xu_pool_data_head_t*)data_head)->size = pool->item_size;

        // count++
        pool->item_count++;

#ifdef __xu_debug__

        // init the debug info
        data_head->debug.magic = pool->for_small ? XU_POOL_DATA_MAGIC : XU_POOL_DATA_EMPTY_MAGIC;
        data_head->debug.file  = file_;
        data_head->debug.func  = func_;
        data_head->debug.line  = (xu_uint16_t)line_;

        // save backtrace
        xu_pool_data_save_backtrace(&data_head->debug, 6);

        // make the dirty data and patch 0xcc for checking underflow
        xu_memset_(data, XU_POOL_DATA_PATCH, pool->item_space - pool->data_head_size);

        // update the real size
        pool->real_size += pool->item_size;

        // update the occupied size
        pool->occupied_size += pool->item_space - XU_POOL_DATA_HEAD_DIFF_SIZE - 1;

        // update the total size
        pool->total_size += pool->item_size;

        // update the peak size
        if (pool->total_size > pool->peak_size) pool->peak_size = pool->total_size;

        // update the malloc count
        pool->malloc_count++;

        // check the prev data
        xu_static_fixed_pool_check_prev(pool, data_head);

        // check the next data
        xu_static_fixed_pool_check_next(pool, data_head);
#endif

    } while (0);

    // check
    xu_assertf(data, "malloc(%lu) failed!", pool->item_size);
    xu_assertf(!(((xu_size_t)data) & (XU_POOL_DATA_ALIGN - 1)), "malloc(%lu): unaligned data: %p", pool->item_size,
               data);

    // ok?
    return data;
}
xu_bool_t xu_static_fixed_pool_free(xu_static_fixed_pool_ref_t self, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool && pool->item_space, xu_false);

    // done
    xu_bool_t                  ok        = xu_false;
    xu_pool_data_empty_head_t* data_head = (xu_pool_data_empty_head_t*)((xu_byte_t*)data - pool->data_head_size);
    do
    {
        // the index
        xu_size_t index = ((xu_byte_t*)data_head - pool->data) / pool->item_space;

        // check
        xu_assertf_pass_and_check_break((xu_byte_t*)data_head >= pool->data &&
                                            (xu_byte_t*)data_head + pool->item_space <= pool->tail,
                                        "the data: %p not belong to pool: %p", data, pool);
        xu_assertf_pass_break(!(((xu_byte_t*)data_head - pool->data) % pool->item_space), "free the invalid data: %p",
                              data);
        xu_assertf_pass_and_check_break(pool->item_count, "double free data: %p", data);
        xu_assertf_pass_and_check_break(xu_static_fixed_pool_used_bset(pool->used_info, index), "double free data: %p",
                                        data);
        xu_assertf_pass_break(data_head->debug.magic ==
                                  (pool->for_small ? XU_POOL_DATA_MAGIC : XU_POOL_DATA_EMPTY_MAGIC),
                              "the invalid data: %p", data);
        xu_assertf_pass_break(((xu_byte_t*)data)[pool->item_size] == XU_POOL_DATA_PATCH, "data underflow");

#ifdef __xu_debug__
        // check the prev data
        xu_static_fixed_pool_check_prev(pool, data_head);

        // check the next data
        xu_static_fixed_pool_check_next(pool, data_head);

        // update the total size
        pool->total_size -= pool->item_size;

        // update the free count
        pool->free_count++;
#endif

        // free it
        xu_static_fixed_pool_used_set0(pool->used_info, index);

        // predict it if no cache
        if (!pool->pred_index) xu_static_fixed_pool_cache_pred(pool, index);

        // size--
        pool->item_count--;

        // ok
        ok = xu_true;

    } while (0);

    // failed? dump it
#ifdef __xu_debug__
    if (!ok)
    {
        // trace
        xu_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        xu_pool_data_dump((xu_byte_t const*)data, xu_true, "[static_fixed_pool]: [error]: ");

        // abort
        xu_abort();
    }
#endif

    // ok?
    return ok;
}
xu_void_t xu_static_fixed_pool_walk(xu_static_fixed_pool_ref_t self, xu_fixed_pool_item_walk_func_t func,
                                    xu_cpointer_t priv)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return(pool && pool->item_maxn && pool->item_space && func);

    // walk
    xu_size_t  i = 0;
    xu_size_t  m = pool->item_maxn;
    xu_byte_t* p = pool->used_info;
    xu_byte_t* d = pool->data + pool->data_head_size;
    xu_byte_t  u = *p;
    xu_byte_t  b = 0;
    for (i = 0; i < m; ++i)
    {
        // bit
        b = i & 0x07;

        // u++
        if (!b)
        {
            u = *p++;

            // this byte is all occupied?
            // if (u == 0xff)
            if (!(u + 1))
            {
                // done func
                func(d + (i + 0) * pool->item_space, priv);
                func(d + (i + 1) * pool->item_space, priv);
                func(d + (i + 2) * pool->item_space, priv);
                func(d + (i + 3) * pool->item_space, priv);
                func(d + (i + 4) * pool->item_space, priv);
                func(d + (i + 5) * pool->item_space, priv);
                func(d + (i + 6) * pool->item_space, priv);
                func(d + (i + 7) * pool->item_space, priv);

                // skip this byte and continue it
                i += 7;
                continue;
            }
        }

        // is occupied?
        // if (xu_static_fixed_pool_used_bset(pool->used_info, i))
        if ((u & (0x01 << b)))
        {
            // done func
            func(d + i * pool->item_space, priv);
        }
    }
}
#ifdef __xu_debug__
xu_void_t xu_static_fixed_pool_dump(xu_static_fixed_pool_ref_t self)
{
    // check
    xu_static_fixed_pool_t* pool = (xu_static_fixed_pool_t*)self;
    xu_assert_and_check_return(pool && pool->used_info);

    // dump
    xu_size_t index = 0;
    for (index = 0; index < pool->item_maxn; ++index)
    {
        // leak?
        if (xu_static_fixed_pool_used_bset(pool->used_info, index))
        {
            // the data head
            xu_pool_data_empty_head_t* data_head = (xu_pool_data_empty_head_t*)(pool->data + index * pool->item_space);

            // check it
            xu_static_fixed_pool_check_data(pool, data_head);

            // the data
            xu_byte_t const* data = (xu_byte_t const*)data_head + pool->data_head_size;

            // trace
            xu_trace_e("leak: %p", data);

            // dump data
            xu_pool_data_dump(data, xu_false, "[static_fixed_pool]: [error]: ");
        }
    }

    // trace debug info
    xu_trace_i("[%lu]: peak_size: %lu, wast_rate: %llu/10000, pred_failed: %lu, item_maxn: %lu, free_count: %lu, "
               "malloc_count: %lu",
               pool->item_size, pool->peak_size,
               pool->occupied_size
                   ? (((xu_hize_t)pool->occupied_size - pool->real_size) * 10000) / (xu_hize_t)pool->occupied_size
                   : 0,
               pool->pred_failed, pool->item_maxn, pool->free_count, pool->malloc_count);
}
#endif
