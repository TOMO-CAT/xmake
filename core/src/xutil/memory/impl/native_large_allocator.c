/* *******************************************************
 *  trace
 */
#define XU_TRACE_MODULE_NAME "native_large_allocator"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/memory/impl/native_large_allocator.h"
#include "xutil/algorithm/for_if.h"
#include "xutil/container/list_entry.h"
#include "xutil/memory/impl/prefix.h"
#include "xutil/platform/native_memory.h"
#include "xutil/platform/virtual_memory.h"

/* *******************************************************
 *  macros
 */

// the native large allocator data size
#define xu_native_large_allocator_data_base(data_head)                                                                 \
    (&(((xu_pool_data_head_t*)((xu_native_large_data_head_t*)(data_head) + 1))[-1]))

/* *******************************************************
 *  types
 */

// the native large data head type
typedef __xu_pool_data_aligned__ struct __xu_native_large_data_head_t
{
#ifdef __xu_debug__
    // the allocator reference
    xu_pointer_t allocator;
#endif

    // the entry
    xu_list_entry_t entry;

    // the data head base
    xu_byte_t base[sizeof(xu_pool_data_head_t)];

} __xu_pool_data_aligned__ xu_native_large_data_head_t;

/*! the native large allocator type
 *
 * <pre>
 *        -----------       -----------               -----------
 *       |||  data   | <=> |||  data   | <=> ... <=> |||  data   | <=> |
 *        -----------       -----------               -----------      |
 *              |                                                      |
 *              `------------------------------------------------------`
 * </pre>
 */
typedef struct __xu_native_large_allocator_t
{
    // the base
    xu_allocator_t base;

    // the data list
    xu_list_entry_head_t data_list;

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

} xu_native_large_allocator_t, *xu_native_large_allocator_ref_t;

/* *******************************************************
 *  private implementation
 */

#ifdef __xu_debug__
static xu_void_t xu_native_large_allocator_check_data(xu_native_large_allocator_ref_t    allocator,
                                                      xu_native_large_data_head_t const* data_head)
{
    // check
    xu_assert_and_check_return(allocator && data_head);

    // done
    xu_bool_t        ok   = xu_false;
    xu_byte_t const* data = (xu_byte_t const*)&(data_head[1]);
    do
    {
        // the base head
        xu_pool_data_head_t* base_head = xu_native_large_allocator_data_base(data_head);

        // check
        xu_assertf_pass_break(base_head->debug.magic != (xu_uint16_t)~XU_POOL_DATA_MAGIC, "data have been freed: %p",
                              data);
        xu_assertf_pass_break(base_head->debug.magic == XU_POOL_DATA_MAGIC, "the invalid data: %p", data);
        xu_assertf_pass_break(((xu_byte_t*)data)[base_head->size] == XU_POOL_DATA_PATCH, "data underflow");

        // ok
        ok = xu_true;

    } while (0);

    // failed? dump it
#    ifdef __xu_debug__
    if (!ok)
    {
        // dump data
        xu_pool_data_dump(data, xu_true, "[native_large_allocator]: [error]: ");

        // abort
        xu_abort();
    }
#    endif
}
static xu_void_t xu_native_large_allocator_check_last(xu_native_large_allocator_ref_t allocator)
{
    // check
    xu_assert_and_check_return(allocator);

    // non-empty?
    if (!xu_list_entry_is_null(&allocator->data_list))
    {
        // the last entry
        xu_list_entry_ref_t data_last = xu_list_entry_last(&allocator->data_list);
        xu_assert_and_check_return(data_last);

        // check it
        xu_native_large_allocator_check_data(
            allocator, (xu_native_large_data_head_t*)xu_list_entry(&allocator->data_list, data_last));
    }
}
static xu_void_t xu_native_large_allocator_check_prev(xu_native_large_allocator_ref_t    allocator,
                                                      xu_native_large_data_head_t const* data_head)
{
    // check
    xu_assert_and_check_return(allocator && data_head);

    // non-empty?
    if (!xu_list_entry_is_null(&allocator->data_list))
    {
        // the prev entry
        xu_list_entry_ref_t data_prev = xu_list_entry_prev((xu_list_entry_ref_t)&data_head->entry);
        xu_assert_and_check_return(data_prev);

        // not tail entry
        xu_check_return(data_prev != xu_list_entry_tail(&allocator->data_list));

        // check it
        xu_native_large_allocator_check_data(
            allocator, (xu_native_large_data_head_t*)xu_list_entry(&allocator->data_list, data_prev));
    }
}
static xu_void_t xu_native_large_allocator_check_next(xu_native_large_allocator_ref_t    allocator,
                                                      xu_native_large_data_head_t const* data_head)
{
    // check
    xu_assert_and_check_return(allocator && data_head);

    // non-empty?
    if (!xu_list_entry_is_null(&allocator->data_list))
    {
        // the next entry
        xu_list_entry_ref_t data_next = xu_list_entry_next((xu_list_entry_ref_t)&data_head->entry);
        xu_assert_and_check_return(data_next);

        // not tail entry
        xu_check_return(data_next != xu_list_entry_tail(&allocator->data_list));

        // check it
        xu_native_large_allocator_check_data(
            allocator, (xu_native_large_data_head_t*)xu_list_entry(&allocator->data_list, data_next));
    }
}
#endif
static xu_pointer_t xu_native_large_allocator_malloc(xu_allocator_ref_t self, xu_size_t size,
                                                     xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_native_large_allocator_ref_t allocator = (xu_native_large_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator, xu_null);

    // done
#ifdef __xu_debug__
    xu_size_t patch = 1; // patch 0xcc
#else
    xu_size_t patch = 0;
#endif
    xu_bool_t                    ok        = xu_false;
    xu_size_t                    need      = sizeof(xu_native_large_data_head_t) + size + patch;
    xu_byte_t*                   data      = xu_null;
    xu_byte_t*                   data_real = xu_null;
    xu_native_large_data_head_t* data_head = xu_null;
    do
    {
#ifdef __xu_debug__
        // check the last data
        xu_native_large_allocator_check_last(allocator);
#endif

        // make data
        data = (xu_byte_t*)(size >= XU_VIRTUAL_MEMORY_DATA_MINN ? xu_virtual_memory_malloc(need)
                                                                : xu_native_memory_malloc(need));
        xu_assert_and_check_break(data);
        xu_assert_and_check_break(!(((xu_size_t)data) & 0x1));

        // make the real data
        data_real = data + sizeof(xu_native_large_data_head_t);

        // init the data head
        data_head = (xu_native_large_data_head_t*)data;

        // the base head
        xu_pool_data_head_t* base_head = xu_native_large_allocator_data_base(data_head);

        // save the real size
        base_head->size = (xu_uint32_t)size;

#ifdef __xu_debug__
        base_head->debug.magic = XU_POOL_DATA_MAGIC;
        base_head->debug.file  = file_;
        base_head->debug.func  = func_;
        base_head->debug.line  = (xu_uint16_t)line_;

        // save backtrace
        xu_pool_data_save_backtrace(&base_head->debug, 5);

        // make the dirty data and patch 0xcc for checking underflow
        xu_memset_(data_real, XU_POOL_DATA_PATCH, size + patch);

        // save allocator reference for checking data range
        data_head->allocator = (xu_pointer_t)allocator;
#endif

        // save the data to the data_list
        xu_list_entry_insert_tail(&allocator->data_list, &data_head->entry);

        // save the real size
        if (real) *real = size;

#ifdef __xu_debug__
        // update the real size
        allocator->real_size += size;

        // update the occupied size
        allocator->occupied_size += need - XU_POOL_DATA_HEAD_DIFF_SIZE - patch;

        // update the total size
        allocator->total_size += size;

        // update the peak size
        if (allocator->total_size > allocator->peak_size) allocator->peak_size = allocator->total_size;

        // update the malloc count
        allocator->malloc_count++;
#endif

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit the data
        if (data) xu_native_memory_free(data);
        data      = xu_null;
        data_real = xu_null;
    }

    // ok?
    return (xu_pointer_t)data_real;
}
static xu_pointer_t xu_native_large_allocator_ralloc(xu_allocator_ref_t self, xu_pointer_t data, xu_size_t size,
                                                     xu_size_t* real __xu_debug_decl__)
{
    // check
    xu_native_large_allocator_ref_t allocator = (xu_native_large_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator, xu_null);

    // done
#ifdef __xu_debug__
    xu_size_t patch = 1; // patch 0xcc
#else
    xu_size_t patch = 0;
#endif
    xu_bool_t                    ok        = xu_false;
    xu_bool_t                    removed   = xu_false;
    xu_size_t                    need      = sizeof(xu_native_large_data_head_t) + size + patch;
    xu_byte_t*                   data_real = xu_null;
    xu_native_large_data_head_t* data_head = xu_null;
    do
    {
        // the data head
        data_head = &(((xu_native_large_data_head_t*)data)[-1]);

        // the base head
        xu_pool_data_head_t* base_head = xu_native_large_allocator_data_base(data_head);

        // check
        xu_assertf(base_head->debug.magic != (xu_uint16_t)~XU_POOL_DATA_MAGIC, "ralloc freed data: %p", data);
        xu_assertf(base_head->debug.magic == XU_POOL_DATA_MAGIC, "ralloc invalid data: %p", data);
        xu_assertf(data_head->allocator == (xu_pointer_t)allocator, "the data: %p not belong to allocator: %p", data,
                   allocator);
        xu_assertf(((xu_byte_t*)data)[base_head->size] == XU_POOL_DATA_PATCH, "data underflow");

#ifdef __xu_debug__
        // check the last data
        xu_native_large_allocator_check_last(allocator);

        // check the prev data
        xu_native_large_allocator_check_prev(allocator, data_head);

        // check the next data
        xu_native_large_allocator_check_next(allocator, data_head);

        // update the real size
        allocator->real_size -= base_head->size;

        // update the occupied size
        allocator->occupied_size -= base_head->size;

        // update the total size
        allocator->total_size -= base_head->size;

        // the previous size
        xu_size_t prev_size = base_head->size;
#endif

        // remove the data from the data_list
        xu_list_entry_remove(&allocator->data_list, &data_head->entry);
        removed = xu_true;

        // ralloc data
        if (size >= XU_VIRTUAL_MEMORY_DATA_MINN)
        {
            if (base_head->size >= XU_VIRTUAL_MEMORY_DATA_MINN)
                data = (xu_byte_t*)xu_virtual_memory_ralloc(data_head, need);
            else
            {
                data = (xu_byte_t*)xu_virtual_memory_malloc(need);
                if (data)
                {
                    xu_memcpy_(data, data_head, base_head->size);
                    xu_native_memory_free(data_head);
                }
            }
        }
        else
        {
            if (base_head->size < XU_VIRTUAL_MEMORY_DATA_MINN)
                data = (xu_byte_t*)xu_native_memory_ralloc(data_head, need);
            else
            {
                data = (xu_byte_t*)xu_native_memory_malloc(need);
                if (data)
                {
                    xu_memcpy_(data, data_head, base_head->size);
                    xu_virtual_memory_free(data_head);
                }
            }
        }
        xu_assert_and_check_break(data);
        xu_assert_and_check_break(!(((xu_size_t)data) & 0x1));

        // update the real data
        data_real = (xu_byte_t*)data + sizeof(xu_native_large_data_head_t);

        // update the data head
        data_head = (xu_native_large_data_head_t*)data;

        // update the base head
        base_head = xu_native_large_allocator_data_base(data_head);

        // save the real size
        base_head->size = (xu_uint32_t)size;

#ifdef __xu_debug__
        base_head->debug.file = file_;
        base_head->debug.func = func_;
        base_head->debug.line = (xu_uint16_t)line_;

        // check
        xu_assertf(base_head->debug.magic == XU_POOL_DATA_MAGIC, "ralloc data have been changed: %p", data);

        // update backtrace
        xu_pool_data_save_backtrace(&base_head->debug, 5);

        // make the dirty data
        if (size > prev_size) xu_memset_(data_real + prev_size, XU_POOL_DATA_PATCH, size - prev_size);

        // patch 0xcc for checking underflow
        data_real[size] = XU_POOL_DATA_PATCH;
#endif

        // save the data to the data_list
        xu_list_entry_insert_tail(&allocator->data_list, &data_head->entry);
        removed = xu_false;

        // save the real size
        if (real) *real = size;

#ifdef __xu_debug__
        // update the real size
        allocator->real_size += size;

        // update the occupied size
        allocator->occupied_size += size;

        // update the total size
        allocator->total_size += size;

        // update the peak size
        if (allocator->total_size > allocator->peak_size) allocator->peak_size = allocator->total_size;

        // update the ralloc count
        allocator->ralloc_count++;
#endif

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // restore data to data_list
        if (data_head && removed) xu_list_entry_insert_tail(&allocator->data_list, &data_head->entry);

        // clear it
        data      = xu_null;
        data_real = xu_null;
    }

    // ok?
    return (xu_pointer_t)data_real;
}
static xu_bool_t xu_native_large_allocator_free(xu_allocator_ref_t self, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_native_large_allocator_ref_t allocator = (xu_native_large_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator && data, xu_false);

    // done
    xu_bool_t                    ok        = xu_false;
    xu_native_large_data_head_t* data_head = xu_null;
    do
    {
        // the data head
        data_head = &(((xu_native_large_data_head_t*)data)[-1]);

        // the base head
        xu_pool_data_head_t* base_head = xu_native_large_allocator_data_base(data_head);

        // check
        xu_assertf(base_head->debug.magic != (xu_uint16_t)~XU_POOL_DATA_MAGIC, "double free data: %p", data);
        xu_assertf(base_head->debug.magic == XU_POOL_DATA_MAGIC, "free invalid data: %p", data);
        xu_assertf(data_head->allocator == (xu_pointer_t)allocator, "the data: %p not belong to allocator: %p", data,
                   allocator);
        xu_assertf(((xu_byte_t*)data)[base_head->size] == XU_POOL_DATA_PATCH, "data underflow");

#ifdef __xu_debug__
        // check the last data
        xu_native_large_allocator_check_last(allocator);

        // check the prev data
        xu_native_large_allocator_check_prev(allocator, data_head);

        // check the next data
        xu_native_large_allocator_check_next(allocator, data_head);

        // for checking double-free
        base_head->debug.magic = (xu_uint16_t)~XU_POOL_DATA_MAGIC;

        // update the total size
        allocator->total_size -= base_head->size;

        // update the free count
        allocator->free_count++;
#endif

        // remove the data from the data_list
        xu_list_entry_remove(&allocator->data_list, &data_head->entry);

        // free it
        if (base_head->size >= XU_VIRTUAL_MEMORY_DATA_MINN)
            xu_virtual_memory_free(data_head);
        else
            xu_native_memory_free(data_head);

        // ok
        ok = xu_true;

    } while (0);

    // ok?
    return ok;
}
static xu_void_t xu_native_large_allocator_clear(xu_allocator_ref_t self)
{
    // check
    xu_native_large_allocator_ref_t allocator = (xu_native_large_allocator_ref_t)self;
    xu_assert_and_check_return(allocator);

    // done
    do
    {
        // the iterator
        xu_iterator_ref_t iterator = xu_list_entry_itor(&allocator->data_list);
        xu_assert_and_check_break(iterator);

        // walk it
        xu_size_t itor = xu_iterator_head(iterator);
        while (itor != xu_iterator_tail(iterator))
        {
            // the data head
            xu_native_large_data_head_t* data_head = (xu_native_large_data_head_t*)xu_iterator_item(iterator, itor);
            xu_assert_and_check_break(data_head);

            // save next
            xu_size_t next = xu_iterator_next(iterator, itor);

            // exit data
            xu_native_large_allocator_free(self, (xu_pointer_t)&data_head[1] __xu_debug_vals__);

            // next
            itor = next;
        }

    } while (0);

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
static xu_void_t xu_native_large_allocator_exit(xu_allocator_ref_t self)
{
    // check
    xu_native_large_allocator_ref_t allocator = (xu_native_large_allocator_ref_t)self;
    xu_assert_and_check_return(allocator);

    // exit lock
    xu_spinlock_exit(&allocator->base.lock);

    // exit it
    xu_native_memory_free(allocator);
}
#ifdef __xu_debug__
static xu_void_t xu_native_large_allocator_dump(xu_allocator_ref_t self)
{
    // check
    xu_native_large_allocator_ref_t allocator = (xu_native_large_allocator_ref_t)self;
    xu_assert_and_check_return(allocator);

    // trace
    xu_trace_i("");

    // exit all data_list
    xu_for_all_if(xu_native_large_data_head_t*, data_head, xu_list_entry_itor(&allocator->data_list), data_head)
    {
        // check it
        xu_native_large_allocator_check_data(allocator, data_head);

        // trace
        xu_trace_e("leak: %p", &data_head[1]);

        // dump data
        xu_pool_data_dump((xu_byte_t const*)&data_head[1], xu_false, "[native_large_allocator]: [error]: ");
    }

    // trace debug info
    xu_trace_i("peak_size: %lu", allocator->peak_size);
    xu_trace_i("wast_rate: %llu/10000", allocator->occupied_size
                                            ? (((xu_hize_t)allocator->occupied_size - allocator->real_size) * 10000) /
                                                  (xu_hize_t)allocator->occupied_size
                                            : 0);
    xu_trace_i("free_count: %lu", allocator->free_count);
    xu_trace_i("malloc_count: %lu", allocator->malloc_count);
    xu_trace_i("ralloc_count: %lu", allocator->ralloc_count);
}
static xu_bool_t xu_native_large_allocator_have(xu_allocator_ref_t self, xu_cpointer_t data)
{
    // check
    xu_native_large_allocator_ref_t allocator = (xu_native_large_allocator_ref_t)self;
    xu_assert_and_check_return_val(allocator, xu_false);

    /* always ok for checking memory
     *
     * TODO: need better implementation for distinguishing it
     */
    return xu_true;
}
#endif

/* *******************************************************
 *  implementation
 */

xu_allocator_ref_t xu_native_large_allocator_init()
{
    // done
    xu_bool_t                       ok        = xu_false;
    xu_native_large_allocator_ref_t allocator = xu_null;
    do
    {
        // check
        xu_assert_static(!(sizeof(xu_native_large_data_head_t) & (XU_POOL_DATA_ALIGN - 1)));

        // make allocator
        allocator = (xu_native_large_allocator_ref_t)xu_native_memory_malloc0(sizeof(xu_native_large_allocator_t));
        xu_assert_and_check_break(allocator);

        // init base
        allocator->base.type         = XU_ALLOCATOR_TYPE_LARGE;
        allocator->base.flag         = XU_ALLOCATOR_FLAG_NONE;
        allocator->base.large_malloc = xu_native_large_allocator_malloc;
        allocator->base.large_ralloc = xu_native_large_allocator_ralloc;
        allocator->base.large_free   = xu_native_large_allocator_free;
        allocator->base.clear        = xu_native_large_allocator_clear;
        allocator->base.exit         = xu_native_large_allocator_exit;
#ifdef __xu_debug__
        allocator->base.dump = xu_native_large_allocator_dump;
        allocator->base.have = xu_native_large_allocator_have;
#endif

        // init lock
        if (!xu_spinlock_init(&allocator->base.lock)) break;

        // init data_list
        xu_list_entry_init(&allocator->data_list, xu_native_large_data_head_t, entry, xu_null);

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
        // exit it
        if (allocator) xu_native_large_allocator_exit((xu_allocator_ref_t)allocator);
        allocator = xu_null;
    }

    // ok?
    return (xu_allocator_ref_t)allocator;
}
