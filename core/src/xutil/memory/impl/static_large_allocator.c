/* *******************************************************
 *  trace
 */
#define XU_TRACE_MODULE_NAME "static_large_allocator"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/memory/impl/static_large_allocator.h"
#include "xutil/libm/math.h"
#include "xutil/memory/impl/prefix.h"
#include "xutil/platform/page.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 *  macros
 */

// the static large allocator data size
#define xu_static_large_allocator_data_base(data_head)                                                                 \
    (&(((xu_pool_data_head_t*)((xu_static_large_data_head_t*)(data_head) + 1))[-1]))

/* *******************************************************
 *  types
 */

// the static large data head type
typedef __xu_pool_data_aligned__ struct __xu_static_large_data_head_t
{
    // the data space size: the allocated size + left size
    xu_uint32_t space : 31;

    // is free?
    xu_uint32_t bfree : 1;

    /* patch 4 bytes for align(8) for tinyc/x86_64
     *
     * __xu_aligned__(8) struct doesn't seem to work
     */
#if defined(XU_COMPILER_IS_TINYC) && defined(XU_CPU_BIT64)
    xu_uint32_t padding;
#endif

    // the data head base
    xu_byte_t base[sizeof(xu_pool_data_head_t)];

} __xu_pool_data_aligned__ xu_static_large_data_head_t;

// the static large data pred type
typedef struct __xu_static_large_data_pred_t
{
    // the data head
    xu_static_large_data_head_t* data_head;

#ifdef __xu_debug__
    // the total count
    xu_size_t total_count;

    // the failed count
    xu_size_t failed_count;
#endif

} xu_static_large_data_pred_t;

/*! the static large allocator type
 *
 * <pre>
 *
 * .e.g page_size == 4KB
 *
 *        --------------------------------------------------------------------------
 *       |                                     data                                 |
 *        --------------------------------------------------------------------------
 *                                              |
 *        --------------------------------------------------------------------------
 *       | head | 4KB | 16KB | 8KB | 128KB | ... | 32KB |       ...       |  4KB*N  |
 *        --------------------------------------------------------------------------
 *                       |                       |               |
 *                       |                       `---------------`
 *                       |                        merge free space when alloc or free
 *                       |
 *        ------------------------------------------
 *       | xu_static_large_data_head_t | data space |
 *        ------------------------------------------
 *
 *        --------------------------------------
 * pred: | >0KB :      4KB       | > 0*page     | 1
 *       |-----------------------|--------------
 *       | >4KB :      8KB       | > 1*page     | 2
 *       |-----------------------|--------------
 *       | >8KB :    12-16KB     | > 2*page     | 3-4
 *       |-----------------------|--------------
 *       | >16KB :   20-32KB     | > 4*page     | 5-8
 *       |-----------------------|--------------
 *       | >32KB :   36-64KB     | > 8*page     | 9-16
 *       |-----------------------|--------------
 *       | >64KB :   68-128KB    | > 16*page    | 17-32
 *       |-----------------------|--------------
 *       | >128KB :  132-256KB   | > 32*page    | 33-64
 *       |-----------------------|--------------
 *       | >256KB :  260-512KB   | > 64*page    | 65 - 128
 *       |-----------------------|--------------
 *       | >512KB :  516-1024KB  | > 128*page   | 129 - 256
 *       |-----------------------|--------------
 *       | >1024KB : 1028-...KB  | > 256*page   | 257 - ..
 *        --------------------------------------
 *
 * </pre>
 */
typedef __xu_pool_data_aligned__ struct __xu_static_large_allocator_t
{
    // the base
    xu_allocator_t base;

    // the page size
    xu_size_t page_size;

    // the data size
    xu_size_t data_size;

    // the data head
    xu_static_large_data_head_t* data_head;

    // the data tail
    xu_static_large_data_head_t* data_tail;

    // the data pred
#ifdef XU_CONFIG_MICRO_ENABLE
    xu_static_large_data_pred_t data_pred[1];
#else
    xu_static_large_data_pred_t data_pred[10];
#endif

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

    // the ralloc count
    xu_size_t ralloc_count;

    // the free count
    xu_size_t free_count;
#endif

} __xu_pool_data_aligned__ xu_static_large_allocator_t, *xu_static_large_allocator_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * checker implementation
 */
#ifdef __xu_debug__
static xu_void_t xu_static_large_allocator_check_data(xu_static_large_allocator_ref_t    allocator,
                                                      xu_static_large_data_head_t const* data_head)
{
    // check
    xu_assert_and_check_return(allocator && data_head);

    // done
    xu_bool_t        ok   = xu_false;
    xu_byte_t const* data = (xu_byte_t const*)&(data_head[1]);
    do
    {
        // the base head
        xu_pool_data_head_t* base_head = xu_static_large_allocator_data_base(data_head);

        // check
        xu_assertf_pass_break(!data_head->bfree, "data have been freed: %p", data);
        xu_assertf_pass_break(base_head->debug.magic == XU_POOL_DATA_MAGIC, "the invalid data: %p", data);
        xu_assertf_pass_break(((xu_byte_t*)data)[base_head->size] == XU_POOL_DATA_PATCH, "data underflow");

        // ok
        ok = xu_true;

    } while (0);

    // failed? dump it
    if (!ok)
    {
        // dump data
        xu_pool_data_dump(data, xu_true, "[static_large_allocator]: [error]: ");

        // abort
        xu_abort();
    }
}
static xu_void_t xu_static_large_allocator_check_next(xu_static_large_allocator_ref_t    allocator,
                                                      xu_static_large_data_head_t const* data_head)
{
    // check
    xu_assert_and_check_return(allocator && data_head);

    // check the next data
    xu_static_large_data_head_t* next_head =
        (xu_static_large_data_head_t*)((xu_byte_t*)&(data_head[1]) + data_head->space);
    if (next_head < allocator->data_tail && !next_head->bfree)
        xu_static_large_allocator_check_data(allocator, next_head);
}
#endif

/* *******************************************************
 *  malloc implementation
 */

static __xu_inline__ xu_size_t xu_static_large_allocator_pred_index(xu_static_large_allocator_ref_t allocator,
                                                                    xu_size_t                       space)
{
#ifndef XU_CONFIG_MICRO_ENABLE
    // the size
    xu_size_t size = sizeof(xu_static_large_data_head_t) + space;
    xu_assert(!(size & (allocator->page_size - 1)));

    // the page count
    size /= allocator->page_size;

    // the pred index
#    if 0
    xu_size_t indx = xu_ilog2i(xu_align_pow2(size));
#    else
    // faster
    xu_size_t indx = size > 1 ? (xu_ilog2i((xu_uint32_t)(size - 1)) + 1) : 0;
#    endif
    if (indx >= xu_arrayn(allocator->data_pred)) indx = xu_arrayn(allocator->data_pred) - 1;
    return indx;
#else
    return 0;
#endif
}
static __xu_inline__ xu_void_t xu_static_large_allocator_pred_update(xu_static_large_allocator_ref_t allocator,
                                                                     xu_static_large_data_head_t*    data_head)
{
    // check
    xu_assert(allocator && data_head && data_head->bfree);

    // cannot be tail
    xu_check_return(data_head != allocator->data_tail);

    // the pred index
    xu_size_t indx = xu_static_large_allocator_pred_index(allocator, data_head->space);

    // the pred head
    xu_static_large_data_head_t* pred_head = allocator->data_pred[indx].data_head;

    // cache this data head
    if (!pred_head || data_head->space > pred_head->space) allocator->data_pred[indx].data_head = data_head;
}
static __xu_inline__ xu_void_t xu_static_large_allocator_pred_remove(xu_static_large_allocator_ref_t allocator,
                                                                     xu_static_large_data_head_t*    data_head)
{
    // check
    xu_assert(allocator && data_head);

    // the pred index
    xu_size_t indx = xu_static_large_allocator_pred_index(allocator, data_head->space);

    // clear this data head
    if (allocator->data_pred[indx].data_head == data_head) allocator->data_pred[indx].data_head = xu_null;
}
static xu_static_large_data_head_t* xu_static_large_allocator_malloc_find(xu_static_large_allocator_ref_t allocator,
                                                                          xu_static_large_data_head_t*    data_head,
                                                                          xu_size_t walk_size, xu_size_t space)
{
    // check
    xu_assert_and_check_return_val(allocator && data_head && space, xu_null);

    // the data tail
    xu_static_large_data_head_t* data_tail = allocator->data_tail;
    xu_check_return_val(data_head < data_tail, xu_null);

    // find the free data
    while ((data_head + 1) <= data_tail && walk_size)
    {
        // the data space size
        xu_size_t data_space = data_head->space;

        // check the space size
        xu_assert(!((sizeof(xu_static_large_data_head_t) + data_space) & (allocator->page_size - 1)));

#ifdef __xu_debug__
        // check the data
        if (!data_head->bfree) xu_static_large_allocator_check_data(allocator, data_head);
#endif

        // allocate if the data is free
        if (data_head->bfree)
        {
            // is enough?
            if (data_space >= space)
            {
                // remove this free data from the pred cache
                xu_static_large_allocator_pred_remove(allocator, data_head);

                // split it if this free data is too large
                if (data_space > sizeof(xu_static_large_data_head_t) + space)
                {
                    // split this free data
                    xu_static_large_data_head_t* next_head =
                        (xu_static_large_data_head_t*)((xu_byte_t*)(data_head + 1) + space);
                    next_head->space = data_space - space - sizeof(xu_static_large_data_head_t);
                    next_head->bfree = 1;
                    data_head->space = space;

                    // add next free data to the pred cache
                    xu_static_large_allocator_pred_update(allocator, next_head);
                }
                else
                {
                    // the next data head
                    xu_static_large_data_head_t* next_head =
                        (xu_static_large_data_head_t*)((xu_byte_t*)(data_head + 1) + data_space);

                    // the next data is free?
                    if (next_head + 1 < data_tail && next_head->bfree)
                    {
                        // add next free data to the pred cache
                        xu_static_large_allocator_pred_update(allocator, next_head);
                    }
                }

                // allocate the data
                data_head->bfree = 0;

                // return the data head
                return data_head;
            }
            else // attempt to merge next free data if this free data is too small
            {
                // the next data head
                xu_static_large_data_head_t* next_head =
                    (xu_static_large_data_head_t*)((xu_byte_t*)(data_head + 1) + data_space);

                // break if doesn't exist next data
                xu_check_break(next_head + 1 < data_tail);

                // the next data is free?
                if (next_head->bfree)
                {
                    // remove next free data from the pred cache
                    xu_static_large_allocator_pred_remove(allocator, next_head);

                    // remove this free data from the pred cache
                    xu_static_large_allocator_pred_remove(allocator, data_head);

                    // trace
                    xu_trace_d("malloc: find: merge: %lu", next_head->space);

                    // merge next data
                    data_head->space += sizeof(xu_static_large_data_head_t) + next_head->space;

                    // add this free data to the pred cache
                    xu_static_large_allocator_pred_update(allocator, data_head);

                    // continue handle this data
                    continue;
                }
            }
        }

        // walk_size--
        walk_size--;

        // skip it if the data is non-free or too small
        data_head = (xu_static_large_data_head_t*)((xu_byte_t*)(data_head + 1) + data_space);
    }

    // failed
    return xu_null;
}
static xu_static_large_data_head_t* xu_static_large_allocator_malloc_pred(xu_static_large_allocator_ref_t allocator,
                                                                          xu_size_t                       space)
{
    // check
    xu_assert_and_check_return_val(allocator && allocator->data_head, xu_null);

    // walk the pred cache
    xu_size_t                    indx      = xu_static_large_allocator_pred_index(allocator, space);
    xu_size_t                    size      = xu_arrayn(allocator->data_pred);
    xu_static_large_data_pred_t* pred      = allocator->data_pred;
    xu_static_large_data_head_t* data_head = xu_null;
    xu_static_large_data_head_t* pred_head = xu_null;
    for (; indx < size && !data_head; indx++)
    {
        // the pred data head
        pred_head = pred[indx].data_head;
        if (pred_head)
        {
            // find the free data from the pred data head
            data_head = xu_static_large_allocator_malloc_find(allocator, pred_head, 1, space);

#ifdef __xu_debug__
            // update the total count
            pred[indx].total_count++;

            // update the failed count
            if (!data_head) pred[indx].failed_count++;
#endif
        }
    }

    // trace
    xu_trace_d("malloc: pred: %lu: %s", space, data_head ? "ok" : "no");

    // ok?
    return data_head;
}
static xu_static_large_data_head_t* xu_static_large_allocator_malloc_done(xu_static_large_allocator_ref_t allocator,
                                                                          xu_size_t                       size,
                                                                          xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator && allocator->data_head, xu_null);

    // done
    xu_bool_t                    ok        = xu_false;
    xu_static_large_data_head_t* data_head = xu_null;
    do
    {
#ifdef __xu_debug__
        // patch 0xcc
        xu_size_t patch = 1;
#else
        xu_size_t patch = 0;
#endif

        // compile the need space for the page alignment
        xu_size_t need_space = xu_align(size + patch, allocator->page_size) - sizeof(xu_static_large_data_head_t);
        if (size + patch > need_space)
            need_space = xu_align(size + patch + allocator->page_size, allocator->page_size) -
                         sizeof(xu_static_large_data_head_t);

        // attempt to predict the free data first
        data_head = xu_static_large_allocator_malloc_pred(allocator, need_space);
        if (!data_head)
        {
            // find the free data from the first data head
            data_head = xu_static_large_allocator_malloc_find(allocator, allocator->data_head, -1, need_space);
            xu_check_break(data_head);
        }
        xu_assert(data_head->space >= size + patch);

        // the base head
        xu_pool_data_head_t* base_head = xu_static_large_allocator_data_base(data_head);

        // the real size
        xu_size_t size_real = real ? (data_head->space - patch) : size;

        // save the real size
        if (real) *real = size_real;
        base_head->size = (xu_uint32_t)size_real;

#ifdef __xu_debug__
        // init the debug info
        base_head->debug.magic = XU_POOL_DATA_MAGIC;
        base_head->debug.file  = file_;
        base_head->debug.func  = func_;
        base_head->debug.line  = (xu_uint16_t)line_;

        // calculate the skip frames
        xu_size_t skip_nframe =
            (xu_allocator() && xu_allocator_type(xu_allocator()) == XU_ALLOCATOR_TYPE_DEFAULT) ? 6 : 3;

        // save backtrace
        xu_pool_data_save_backtrace(&base_head->debug, skip_nframe);

        // make the dirty data and patch 0xcc for checking underflow
        xu_memset_((xu_pointer_t) & (data_head[1]), XU_POOL_DATA_PATCH, size_real + patch);

        // update the real size
        allocator->real_size += base_head->size;

        // update the occupied size
        allocator->occupied_size +=
            sizeof(xu_static_large_data_head_t) + data_head->space - 1 - XU_POOL_DATA_HEAD_DIFF_SIZE;

        // update the total size
        allocator->total_size += base_head->size;

        // update the peak size
        if (allocator->total_size > allocator->peak_size) allocator->peak_size = allocator->total_size;

        // update the malloc count
        allocator->malloc_count++;
#endif

        // ok
        ok = xu_true;

    } while (0);

    // trace
    xu_trace_d("malloc: %lu: %s", size, ok ? "ok" : "no");

    // failed? clear it
    if (!ok) data_head = xu_null;

    // ok?
    return data_head;
}
static xu_static_large_data_head_t* xu_static_large_allocator_ralloc_fast(xu_static_large_allocator_ref_t allocator,
                                                                          xu_static_large_data_head_t*    data_head,
                                                                          xu_size_t                       size,
                                                                          xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_assert_and_check_return_val(allocator && data_head && size, xu_null);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // the base head
        xu_pool_data_head_t* base_head = xu_static_large_allocator_data_base(data_head);

#ifdef __xu_debug__
        // patch 0xcc
        xu_size_t patch = 1;

        // the prev size
        xu_size_t prev_size = base_head->size;

        // the prev space
        xu_size_t prev_space = data_head->space;

#else
        // no patch
        xu_size_t patch = 0;
#endif

        // compile the need space for the page alignment
        xu_size_t need_space = xu_align(size + patch, allocator->page_size) - sizeof(xu_static_large_data_head_t);
        if (size + patch > need_space)
            need_space = xu_align(size + patch + allocator->page_size, allocator->page_size) -
                         sizeof(xu_static_large_data_head_t);

        // this data space is not enough?
        if (need_space > data_head->space)
        {
            // attempt to merge the next free data
            xu_static_large_data_head_t* data_tail = allocator->data_tail;
            xu_static_large_data_head_t* next_head =
                (xu_static_large_data_head_t*)((xu_byte_t*)&(data_head[1]) + data_head->space);
            while (next_head < data_tail && next_head->bfree)
            {
                // remove next free data from the pred cache
                xu_static_large_allocator_pred_remove(allocator, next_head);

                // trace
                xu_trace_d("ralloc: fast: merge: %lu", next_head->space);

                // merge it
                data_head->space += sizeof(xu_static_large_data_head_t) + next_head->space;

                // the next data head
                next_head = (xu_static_large_data_head_t*)((xu_byte_t*)&(data_head[1]) + data_head->space);
            }
        }

        // enough?
        xu_check_break(need_space <= data_head->space);

        // split it if this data is too large after merging
        if (data_head->space > sizeof(xu_static_large_data_head_t) + need_space)
        {
            // split this free data
            xu_static_large_data_head_t* next_head =
                (xu_static_large_data_head_t*)((xu_byte_t*)(data_head + 1) + need_space);
            next_head->space = data_head->space - need_space - sizeof(xu_static_large_data_head_t);
            next_head->bfree = 1;
            data_head->space = need_space;

            // add next free data to the pred cache
            xu_static_large_allocator_pred_update(allocator, next_head);
        }

        // the real size
        xu_size_t size_real = real ? (data_head->space - patch) : size;

        // save the real size
        if (real) *real = size_real;
        base_head->size = (xu_uint32_t)size_real;

#ifdef __xu_debug__
        // init the debug info
        base_head->debug.magic = XU_POOL_DATA_MAGIC;
        base_head->debug.file  = file_;
        base_head->debug.func  = func_;
        base_head->debug.line  = (xu_uint16_t)line_;

        // calculate the skip frames
        xu_size_t skip_nframe =
            (xu_allocator() && xu_allocator_type(xu_allocator()) == XU_ALLOCATOR_TYPE_DEFAULT) ? 6 : 3;

        // save backtrace
        xu_pool_data_save_backtrace(&base_head->debug, skip_nframe);

        // make the dirty data
        if (size_real > prev_size)
            xu_memset_((xu_byte_t*)&(data_head[1]) + prev_size, XU_POOL_DATA_PATCH, size_real - prev_size);

        // patch 0xcc for checking underflow
        ((xu_byte_t*)&(data_head[1]))[size_real] = XU_POOL_DATA_PATCH;

        // update the real size
        allocator->real_size += size_real;
        allocator->real_size -= prev_size;

        // update the occupied size
        allocator->occupied_size += data_head->space;
        allocator->occupied_size -= prev_space;

        // update the total size
        allocator->total_size += size_real;
        allocator->total_size -= prev_size;

        // update the peak size
        if (allocator->total_size > allocator->peak_size) allocator->peak_size = allocator->total_size;
#endif

        // ok
        ok = xu_true;

    } while (0);

    // failed? clear it
    if (!ok) data_head = xu_null;

    // trace
    xu_trace_d("ralloc: fast: %lu: %s", size, ok ? "ok" : "no");

    // ok?
    return data_head;
}
static xu_bool_t xu_static_large_allocator_free(xu_allocator_ref_t self, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_static_large_allocator_ref_t allocator = (xu_static_large_allocator_t*)self;
    xu_assert_and_check_return_val(allocator && data, xu_false);

    // done
    xu_bool_t                    ok        = xu_false;
    xu_static_large_data_head_t* data_head = xu_null;
    do
    {
        // the data head
        data_head = &(((xu_static_large_data_head_t*)data)[-1]);

#ifdef __xu_debug__
        // the base head
        xu_pool_data_head_t* base_head = xu_static_large_allocator_data_base(data_head);
#endif

        // check
        xu_assertf_and_check_break(!data_head->bfree, "double free data: %p", data);
        xu_assertf(base_head->debug.magic == XU_POOL_DATA_MAGIC, "free invalid data: %p", data);
        xu_assertf_and_check_break(data_head >= allocator->data_head && data_head < allocator->data_tail,
                                   "the data: %p not belong to allocator: %p", data, allocator);
        xu_assertf(((xu_byte_t*)data)[base_head->size] == XU_POOL_DATA_PATCH, "data underflow");

#ifdef __xu_debug__
        // check the next data
        xu_static_large_allocator_check_next(allocator, data_head);

        // update the total size
        allocator->total_size -= base_head->size;

        // update the free count
        allocator->free_count++;
#endif

        // trace
        xu_trace_d("free: %lu: %s", base_head->size, ok ? "ok" : "no");

        // attempt merge the next free data
        xu_static_large_data_head_t* next_head =
            (xu_static_large_data_head_t*)((xu_byte_t*)&(data_head[1]) + data_head->space);
        if (next_head < allocator->data_tail && next_head->bfree)
        {
            // remove next free data from the pred cache
            xu_static_large_allocator_pred_remove(allocator, next_head);

            // trace
            xu_trace_d("free: merge: %lu", next_head->space);

            // merge it
            data_head->space += sizeof(xu_static_large_data_head_t) + next_head->space;
        }

        // free it
        data_head->bfree = 1;

        // add this free data to the pred cache
        xu_static_large_allocator_pred_update(allocator, data_head);

        // ok
        ok = xu_true;

    } while (0);

    // ok?
    return ok;
}
static xu_pointer_t xu_static_large_allocator_malloc(xu_allocator_ref_t self, xu_size_t size,
                                                     xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_static_large_allocator_ref_t allocator = (xu_static_large_allocator_t*)self;
    xu_assert_and_check_return_val(allocator && size, xu_null);

    // done
    xu_static_large_data_head_t* data_head =
        xu_static_large_allocator_malloc_done(allocator, size, real __xu_debug_args__);

    // ok
    return data_head ? (xu_pointer_t) & (data_head[1]) : xu_null;
}
static xu_pointer_t xu_static_large_allocator_ralloc(xu_allocator_ref_t self, xu_pointer_t data, xu_size_t size,
                                                     xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_static_large_allocator_ref_t allocator = (xu_static_large_allocator_t*)self;
    xu_assert_and_check_return_val(allocator && data && size, xu_null);

    // done
    xu_bool_t                    ok        = xu_false;
    xu_byte_t*                   data_real = xu_null;
    xu_static_large_data_head_t* data_head = xu_null;
    xu_static_large_data_head_t* aloc_head = xu_null;
    do
    {
        // the data head
        data_head = &(((xu_static_large_data_head_t*)data)[-1]);

#ifdef __xu_debug__
        // the base head
        xu_pool_data_head_t* base_head = xu_static_large_allocator_data_base(data_head);
#endif

        // check
        xu_assertf_and_check_break(!data_head->bfree, "ralloc freed data: %p", data);
        xu_assertf(base_head->debug.magic == XU_POOL_DATA_MAGIC, "ralloc invalid data: %p", data);
        xu_assertf_and_check_break(data_head >= allocator->data_head && data_head < allocator->data_tail,
                                   "the data: %p not belong to allocator: %p", data, allocator);
        xu_assertf(((xu_byte_t*)data)[base_head->size] == XU_POOL_DATA_PATCH, "data underflow");

#ifdef __xu_debug__
        // check the next data
        xu_static_large_allocator_check_next(allocator, data_head);
#endif

        // attempt to allocate it fast if enough
        aloc_head = xu_static_large_allocator_ralloc_fast(allocator, data_head, size, real __xu_debug_args__);
        if (!aloc_head)
        {
            // allocate it
            aloc_head = xu_static_large_allocator_malloc_done(allocator, size, real __xu_debug_args__);
            xu_check_break(aloc_head);

            // not same?
            if (aloc_head != data_head)
            {
                // copy the real data
                xu_memcpy_((xu_pointer_t)&aloc_head[1], data,
                           xu_min(size, (((xu_pool_data_head_t*)(data_head + 1))[-1]).size));

                // free the previous data
                xu_static_large_allocator_free(self, data __xu_debug_args__);
            }
        }

        // the real data
        data_real = (xu_byte_t*)&aloc_head[1];

#ifdef __xu_debug__
        // update the ralloc count
        allocator->ralloc_count++;
#endif

        // ok
        ok = xu_true;

    } while (0);

    // trace
    xu_trace_d("ralloc: %lu: %s", size, ok ? "ok" : "no");

    // failed? clear it
    if (!ok) data_real = xu_null;

    // ok?
    return (xu_pointer_t)data_real;
}
static xu_void_t xu_static_large_allocator_clear(xu_allocator_ref_t self)
{
    // check
    xu_static_large_allocator_ref_t allocator = (xu_static_large_allocator_t*)self;
    xu_assert_and_check_return(allocator && allocator->data_head &&
                               allocator->data_size > sizeof(xu_static_large_data_head_t));

    // clear it
    allocator->data_head->bfree = 1;
    allocator->data_head->space = allocator->data_size - sizeof(xu_static_large_data_head_t);

    // clear the pred cache
    xu_memset_(allocator->data_pred, 0, sizeof(allocator->data_pred));

    // add this free data to the pred cache
    xu_static_large_allocator_pred_update(allocator, allocator->data_head);

    // clear info
#ifdef __xu_debug__
    allocator->peak_size     = 0;
    allocator->total_size    = 0;
    allocator->real_size     = 0;
    allocator->occupied_size = 0;
    allocator->malloc_count  = 0;
    allocator->ralloc_count  = 0;
    allocator->free_count    = 0;
#endif
}
static xu_void_t xu_static_large_allocator_exit(xu_allocator_ref_t self)
{
    // check
    xu_static_large_allocator_ref_t allocator = (xu_static_large_allocator_t*)self;
    xu_assert_and_check_return(allocator);

    // exit lock
    xu_spinlock_exit(&allocator->base.lock);
}
#ifdef __xu_debug__
static xu_void_t xu_static_large_allocator_dump(xu_allocator_ref_t self)
{
    // check
    xu_static_large_allocator_ref_t allocator = (xu_static_large_allocator_t*)self;
    xu_assert_and_check_return(allocator);

    // trace
    xu_trace_i("");

    // the data head
    xu_static_large_data_head_t* data_head = allocator->data_head;
    xu_assert_and_check_return(data_head);

    // the data tail
    xu_static_large_data_head_t* data_tail = allocator->data_tail;
    xu_assert_and_check_return(data_tail);

    // done
    xu_size_t frag_count = 0;
    while ((data_head + 1) <= data_tail)
    {
        // non-free?
        if (!data_head->bfree)
        {
            // check it
            xu_static_large_allocator_check_data(allocator, data_head);

            // trace
            xu_trace_e("leak: %p", &data_head[1]);

            // dump data
            xu_pool_data_dump((xu_byte_t const*)&data_head[1], xu_false, "[static_large_allocator]: [error]: ");
        }

        // fragment++
        frag_count++;

        // the next head
        data_head = (xu_static_large_data_head_t*)((xu_byte_t*)(data_head + 1) + data_head->space);
    }

    // trace
    xu_trace_i("");

    // trace pred info
    xu_size_t i         = 0;
    xu_size_t pred_size = xu_arrayn(allocator->data_pred);
    for (i = 0; i < pred_size; i++)
    {
        // the pred info
        xu_static_large_data_pred_t const* pred = &allocator->data_pred[i];
        xu_assert_and_check_break(pred);

        // trace
        xu_trace_i("pred[>%04luKB]: data: %p, space: %lu, total_count: %lu, failed_count: %lu",
                   ((allocator->page_size << (i - 1)) >> 10), pred->data_head ? &pred->data_head[1] : xu_null,
                   pred->data_head ? pred->data_head->space : 0, pred->total_count, pred->failed_count);
    }

    // trace
    xu_trace_i("");

    // trace debug info
    xu_trace_i("peak_size: %lu", allocator->peak_size);
    xu_trace_i("wast_rate: %llu/10000", allocator->occupied_size
                                            ? (((xu_hize_t)allocator->occupied_size - allocator->real_size) * 10000) /
                                                  (xu_hize_t)allocator->occupied_size
                                            : 0);
    xu_trace_i("frag_count: %lu", frag_count);
    xu_trace_i("free_count: %lu", allocator->free_count);
    xu_trace_i("malloc_count: %lu", allocator->malloc_count);
    xu_trace_i("ralloc_count: %lu", allocator->ralloc_count);
}
static xu_bool_t xu_static_large_allocator_have(xu_allocator_ref_t self, xu_cpointer_t data)
{
    // check
    xu_static_large_allocator_ref_t allocator = (xu_static_large_allocator_t*)self;
    xu_assert_and_check_return_val(allocator, xu_false);

    // have it?
    return ((xu_byte_t const*)data > (xu_byte_t const*)allocator->data_head &&
            (xu_byte_t const*)data < (xu_byte_t const*)allocator->data_head + allocator->data_size)
               ? xu_true
               : xu_false;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
xu_allocator_ref_t xu_static_large_allocator_init(xu_byte_t* data, xu_size_t size, xu_size_t pagesize)
{
    // check
    xu_assert_and_check_return_val(data && size, xu_null);
    xu_assert_static(!(sizeof(xu_static_large_data_head_t) & (XU_POOL_DATA_ALIGN - 1)));
    xu_assert_static(!(sizeof(xu_static_large_allocator_t) & (XU_POOL_DATA_ALIGN - 1)));

    // align data and size
    xu_size_t diff = xu_align((xu_size_t)data, XU_POOL_DATA_ALIGN) - (xu_size_t)data;
    xu_assert_and_check_return_val(size > diff + sizeof(xu_static_large_allocator_t), xu_null);
    size -= diff;
    data += diff;

    // init allocator
    xu_static_large_allocator_ref_t allocator = (xu_static_large_allocator_t*)data;
    xu_memset_(allocator, 0, sizeof(xu_static_large_allocator_t));

    // init base
    allocator->base.type         = XU_ALLOCATOR_TYPE_LARGE;
    allocator->base.flag         = XU_ALLOCATOR_FLAG_NONE;
    allocator->base.large_malloc = xu_static_large_allocator_malloc;
    allocator->base.large_ralloc = xu_static_large_allocator_ralloc;
    allocator->base.large_free   = xu_static_large_allocator_free;
    allocator->base.clear        = xu_static_large_allocator_clear;
    allocator->base.exit         = xu_static_large_allocator_exit;
#ifdef __xu_debug__
    allocator->base.dump = xu_static_large_allocator_dump;
    allocator->base.have = xu_static_large_allocator_have;
#endif

    // init lock
    if (!xu_spinlock_init(&allocator->base.lock)) return xu_null;

    // init page_size
    allocator->page_size = pagesize ? pagesize : xu_page_size();

    // page_size must be larger than sizeof(xu_static_large_data_head_t)
    if (allocator->page_size < sizeof(xu_static_large_data_head_t))
        allocator->page_size += sizeof(xu_static_large_data_head_t);

    // page_size must be aligned
    allocator->page_size = xu_align_pow2(allocator->page_size);
    xu_assert_and_check_return_val(allocator->page_size, xu_null);

    // init data size
    allocator->data_size = size - sizeof(xu_static_large_allocator_t);
    xu_assert_and_check_return_val(allocator->data_size > allocator->page_size, xu_null);

    // align data size
    allocator->data_size = xu_align(allocator->data_size - allocator->page_size, allocator->page_size);
    xu_assert_and_check_return_val(allocator->data_size > sizeof(xu_static_large_data_head_t), xu_null);

    // init data head
    allocator->data_head        = (xu_static_large_data_head_t*)&allocator[1];
    allocator->data_head->bfree = 1;
    allocator->data_head->space = allocator->data_size - sizeof(xu_static_large_data_head_t);
    xu_assert_and_check_return_val(!((xu_size_t)allocator->data_head & (XU_POOL_DATA_ALIGN - 1)), xu_null);

    // add this free data to the pred cache
    xu_static_large_allocator_pred_update(allocator, allocator->data_head);

    // init data tail
    allocator->data_tail =
        (xu_static_large_data_head_t*)((xu_byte_t*)&allocator->data_head[1] + allocator->data_head->space);

    // register lock profiler
#ifdef XU_LOCK_PROFILER_ENABLE
    xu_lock_profiler_register(xu_lock_profiler(), (xu_pointer_t)&allocator->base.lock, XU_TRACE_MODULE_NAME);
#endif

    // ok
    return (xu_allocator_ref_t)allocator;
}
