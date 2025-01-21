#include "xutil/memory/fixed_pool.h"
#include "xutil/algorithm/binary_find.h"
#include "xutil/algorithm/for_if.h"
#include "xutil/container/array_iterator.h"
#include "xutil/container/list_entry.h"
#include "xutil/memory/impl/static_fixed_pool.h"
#include "xutil/platform/ifaddrs.h"
#include "xutil/platform/page.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the item belong to this slot?
#define xu_fixed_pool_slot_exists(slot, item)                                                                          \
    (((xu_byte_t*)(item) > (xu_byte_t*)(slot)) && ((xu_byte_t*)(item) < (xu_byte_t*)slot + (slot)->size))

/* *******************************************************
 * types
 * *******************************************************
 */

// the fixed pool slot type
typedef struct __xu_fixed_pool_slot_t
{
    // the size: sizeof(slot) + data
    xu_size_t size;

    // the pool
    xu_static_fixed_pool_ref_t pool;

    // the list entry
    xu_list_entry_t entry;

} xu_fixed_pool_slot_t;

// the fixed pool type
typedef struct __xu_fixed_pool_impl_t
{
    // the large allocator
    xu_allocator_ref_t large_allocator;

    // the slot size
    xu_size_t slot_size;

    // the item size
    xu_size_t item_size;

    // the item count
    xu_size_t item_count;

    // the init func
    xu_fixed_pool_item_init_func_t func_init;

    // the exit func
    xu_fixed_pool_item_exit_func_t func_exit;

    // the private data
    xu_cpointer_t func_priv;

    // the current slot
    xu_fixed_pool_slot_t* current_slot;

    // the partial slot
    xu_list_entry_head_t partial_slots;

    // the full slot
    xu_list_entry_head_t full_slots;

    // the slot list
    xu_fixed_pool_slot_t** slot_list;

    // the slot count
    xu_size_t slot_count;

    // the slot space
    xu_size_t slot_space;

    // for small allocator
    xu_bool_t for_small;

} xu_fixed_pool_t;

/* *******************************************************
 * declaration
 * *******************************************************
 */

xu_fixed_pool_ref_t xu_fixed_pool_init_(xu_allocator_ref_t large_allocator, xu_size_t slot_size, xu_size_t item_size,
                                        xu_bool_t for_small, xu_fixed_pool_item_init_func_t item_init,
                                        xu_fixed_pool_item_exit_func_t item_exit, xu_cpointer_t priv);

/* *******************************************************
 * private implementation
 * *******************************************************
 */
static xu_bool_t xu_fixed_pool_item_exit(xu_pointer_t data, xu_cpointer_t priv)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)priv;
    xu_assert(pool && pool->func_exit);

    // done exit
    pool->func_exit(data, pool->func_priv);

    // continue
    return xu_true;
}
static xu_void_t xu_fixed_pool_slot_exit(xu_fixed_pool_t* pool, xu_fixed_pool_slot_t* slot)
{
    // check
    xu_assert_and_check_return(pool && pool->large_allocator && slot);
    xu_assert_and_check_return(pool->slot_list && pool->slot_count);

    // trace
    xu_trace_d("slot[%lu]: exit: size: %lu", pool->item_size, slot->size);

    // make the iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator =
        xu_array_iterator_init_ptr(&array_iterator, (xu_pointer_t*)pool->slot_list, pool->slot_count);
    xu_assert(iterator);

    // find the slot from the slot list
    xu_size_t itor = xu_binary_find_all(iterator, (xu_cpointer_t)slot);
    xu_assert(itor != xu_iterator_tail(iterator) && itor < pool->slot_count && pool->slot_list[itor]);
    xu_check_return(itor != xu_iterator_tail(iterator) && itor < pool->slot_count && pool->slot_list[itor]);

    // remove the slot
    if (itor + 1 < pool->slot_count)
        xu_memmov_(pool->slot_list + itor, pool->slot_list + itor + 1,
                   (pool->slot_count - itor - 1) * sizeof(xu_fixed_pool_slot_t*));

    // update the slot count
    pool->slot_count--;

    // exit slot
    xu_allocator_large_free(pool->large_allocator, slot);
}
static xu_fixed_pool_slot_t* xu_fixed_pool_slot_init(xu_fixed_pool_t* pool)
{
    // check
    xu_assert_and_check_return_val(pool && pool->large_allocator && pool->slot_size && pool->item_size, xu_null);

    // done
    xu_bool_t             ok   = xu_false;
    xu_fixed_pool_slot_t* slot = xu_null;
    do
    {
#ifdef __xu_debug__
        // init patch for checking underflow
        xu_size_t patch = 1;
#else
        xu_size_t patch = 0;
#endif

        // the item space
        xu_size_t item_space = sizeof(xu_pool_data_head_t) + pool->item_size + patch;

        // the need space
        xu_size_t need_space = sizeof(xu_fixed_pool_slot_t) + pool->slot_size * item_space;

        // make slot
        xu_size_t real_space = 0;
        slot = (xu_fixed_pool_slot_t*)xu_allocator_large_malloc(pool->large_allocator, need_space, &real_space);
        xu_assert_and_check_break(slot);
        xu_assert_and_check_break(real_space > sizeof(xu_fixed_pool_slot_t) + item_space);

        // init slot
        slot->size = real_space;
        slot->pool = xu_static_fixed_pool_init((xu_byte_t*)&slot[1], real_space - sizeof(xu_fixed_pool_slot_t),
                                               pool->item_size, pool->for_small);
        xu_assert_and_check_break(slot->pool);

        // no list?
        if (!pool->slot_list)
        {
            // init the slot list
            xu_size_t size  = 0;
            pool->slot_list = (xu_fixed_pool_slot_t**)xu_allocator_large_nalloc(pool->large_allocator, 64,
                                                                                sizeof(xu_fixed_pool_slot_t*), &size);
            xu_assert_and_check_break(pool->slot_list && size);

            // init the slot count
            pool->slot_count = 0;

            // init the slot space
            pool->slot_space = size / sizeof(xu_fixed_pool_slot_t*);
            xu_assert_and_check_break(pool->slot_space);
        }
        // no enough space?
        else if (pool->slot_count == pool->slot_space)
        {
            // grow the slot list
            xu_size_t size  = 0;
            pool->slot_list = (xu_fixed_pool_slot_t**)xu_allocator_large_ralloc(
                pool->large_allocator, pool->slot_list, (pool->slot_space << 1) * sizeof(xu_fixed_pool_slot_t*), &size);
            xu_assert_and_check_break(pool->slot_list && size);

            // update the slot space
            pool->slot_space = size / sizeof(xu_fixed_pool_slot_t*);
            xu_assert_and_check_break(pool->slot_space);
        }

        // check
        xu_assert_and_check_break(pool->slot_count < pool->slot_space);

        // insert the slot to the slot list in the increasing order (TODO binary search)
        xu_size_t i = 0;
        xu_size_t n = pool->slot_count;
        for (i = 0; i < n; i++)
            if (slot <= pool->slot_list[i]) break;
        if (i < n) xu_memmov_(pool->slot_list + i + 1, pool->slot_list + i, (n - i) * sizeof(xu_fixed_pool_slot_t*));
        pool->slot_list[i] = slot;

        // update the slot count
        pool->slot_count++;

        // trace
        xu_trace_d("slot[%lu]: init: size: %lu => %lu, item: %lu => %lu", pool->item_size, need_space, real_space,
                   pool->slot_size, xu_static_fixed_pool_maxn(slot->pool));

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (slot) xu_fixed_pool_slot_exit(pool, slot);
        slot = xu_null;
    }

    // ok?
    return slot;
}
#if 0
static xu_fixed_pool_slot_t* xu_fixed_pool_slot_find(xu_fixed_pool_t* pool, xu_pointer_t data)
{
    // check
    xu_assert_and_check_return_val(pool && data, xu_null);

    // done
    xu_fixed_pool_slot_t* slot = xu_null;
    do
    {
        // belong to the current slot?
        if (pool->current_slot && xu_fixed_pool_slot_exists(pool->current_slot, data))
        {
            slot = pool->current_slot;
            break;
        }

        // find the slot from the partial slots
        xu_for_all_if(xu_fixed_pool_slot_t*, partial_slot, xu_list_entry_itor(&pool->partial_slots), partial_slot)
        {
            // is this?
            if (xu_fixed_pool_slot_exists(partial_slot, data))
            {
                slot = partial_slot;
                break;
            }
        }

        // no found?
        xu_check_break(!slot);

        // find the slot from the full slots
        xu_for_all_if(xu_fixed_pool_slot_t*, full_slot, xu_list_entry_itor(&pool->full_slots), full_slot)
        {
            // is this?
            if (xu_fixed_pool_slot_exists(full_slot, data))
            {
                slot = full_slot;
                break;
            }
        }

    } while (0);

    // ok?
    return slot;
}
#else
static xu_long_t xu_fixed_pool_slot_comp(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t data)
{
    // the slot
    xu_fixed_pool_slot_t* slot = (xu_fixed_pool_slot_t*)item;
    xu_assert(slot);

    // comp
    return (xu_byte_t*)data < (xu_byte_t*)slot ? 1 : ((xu_byte_t*)data >= (xu_byte_t*)slot + slot->size ? -1 : 0);
}
static xu_fixed_pool_slot_t* xu_fixed_pool_slot_find(xu_fixed_pool_t* pool, xu_pointer_t data)
{
    // check
    xu_assert_and_check_return_val(pool && data, xu_null);

    // make the iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t iterator =
        xu_array_iterator_init_ptr(&array_iterator, (xu_pointer_t*)pool->slot_list, pool->slot_count);
    xu_assert(iterator);

    // find it
    xu_size_t itor = xu_binary_find_all_if(iterator, xu_fixed_pool_slot_comp, data);
    xu_check_return_val(itor != xu_iterator_tail(iterator), xu_null);

    // the slot
    xu_fixed_pool_slot_t* slot = pool->slot_list[itor];
    xu_assert_and_check_return_val(slot, xu_null);

    // check
    xu_assert(xu_fixed_pool_slot_exists(slot, data));

    // ok?
    return slot;
}
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_fixed_pool_ref_t xu_fixed_pool_init_(xu_allocator_ref_t large_allocator, xu_size_t slot_size, xu_size_t item_size,
                                        xu_bool_t for_small, xu_fixed_pool_item_init_func_t item_init,
                                        xu_fixed_pool_item_exit_func_t item_exit, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(item_size, xu_null);

    // done
    xu_bool_t        ok   = xu_false;
    xu_fixed_pool_t* pool = xu_null;
    do
    {
        // no allocator? uses the global allocator
        if (!large_allocator) large_allocator = xu_allocator();
        xu_assert_and_check_break(large_allocator);

        // make pool
        pool = (xu_fixed_pool_t*)xu_allocator_large_malloc0(large_allocator, sizeof(xu_fixed_pool_t), xu_null);
        xu_assert_and_check_break(pool);

        // init pool
        pool->large_allocator = large_allocator;
        pool->slot_size       = slot_size ? slot_size : (xu_page_size() >> 4);
        pool->item_size       = item_size;
        pool->func_init       = item_init;
        pool->func_exit       = item_exit;
        pool->func_priv       = priv;
        pool->for_small       = for_small;
        xu_assert_and_check_break(pool->slot_size);

        // init partial slots
        xu_list_entry_init(&pool->partial_slots, xu_fixed_pool_slot_t, entry, xu_null);

        // init full slots
        xu_list_entry_init(&pool->full_slots, xu_fixed_pool_slot_t, entry, xu_null);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (pool) xu_fixed_pool_exit((xu_fixed_pool_ref_t)pool);
        pool = xu_null;
    }

    // ok?
    return (xu_fixed_pool_ref_t)pool;
}
xu_fixed_pool_ref_t xu_fixed_pool_init(xu_allocator_ref_t large_allocator, xu_size_t slot_size, xu_size_t item_size,
                                       xu_fixed_pool_item_init_func_t item_init,
                                       xu_fixed_pool_item_exit_func_t item_exit, xu_cpointer_t priv)
{
    return xu_fixed_pool_init_(large_allocator, slot_size, item_size, xu_false, item_init, item_exit, priv);
}
xu_void_t xu_fixed_pool_exit(xu_fixed_pool_ref_t self)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return(pool);

    // clear it
    xu_fixed_pool_clear(self);

    // exit the current slot
    if (pool->current_slot) xu_fixed_pool_slot_exit(pool, pool->current_slot);
    pool->current_slot = xu_null;

    // exit the slot list
    if (pool->slot_list) xu_allocator_large_free(pool->large_allocator, pool->slot_list);
    pool->slot_list  = xu_null;
    pool->slot_count = 0;
    pool->slot_space = 0;

    // exit it
    xu_allocator_large_free(pool->large_allocator, pool);
}
xu_size_t xu_fixed_pool_size(xu_fixed_pool_ref_t self)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, 0);

    // the item count
    return pool->item_count;
}
xu_size_t xu_fixed_pool_item_size(xu_fixed_pool_ref_t self)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, 0);

    // the item size
    return pool->item_size;
}
xu_void_t xu_fixed_pool_clear(xu_fixed_pool_ref_t self)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return(pool);

    // exit items
    if (pool->func_exit) xu_fixed_pool_walk(self, xu_fixed_pool_item_exit, (xu_pointer_t)pool);

    // exit the partial slots
    xu_iterator_ref_t partial_iterator = xu_list_entry_itor(&pool->partial_slots);
    if (partial_iterator)
    {
        // walk it
        xu_size_t itor = xu_iterator_head(partial_iterator);
        while (itor != xu_iterator_tail(partial_iterator))
        {
            // the slot
            xu_fixed_pool_slot_t* slot = (xu_fixed_pool_slot_t*)xu_iterator_item(partial_iterator, itor);
            xu_assert_and_check_break(slot);

            // check
            xu_assert(slot != pool->current_slot);

            // save next
            xu_size_t next = xu_iterator_next(partial_iterator, itor);

            // exit slot
            xu_fixed_pool_slot_exit(pool, slot);

            // next
            itor = next;
        }
    }

    // exit the full slots
    xu_iterator_ref_t full_iterator = xu_list_entry_itor(&pool->full_slots);
    if (full_iterator)
    {
        // walk it
        xu_size_t itor = xu_iterator_head(full_iterator);
        while (itor != xu_iterator_tail(full_iterator))
        {
            // the slot
            xu_fixed_pool_slot_t* slot = (xu_fixed_pool_slot_t*)xu_iterator_item(full_iterator, itor);
            xu_assert_and_check_break(slot);

            // check
            xu_assert(slot != pool->current_slot);

            // save next
            xu_size_t next = xu_iterator_next(full_iterator, itor);

            // exit slot
            xu_fixed_pool_slot_exit(pool, slot);

            // next
            itor = next;
        }
    }

    // clear current slot
    if (pool->current_slot && pool->current_slot->pool) xu_static_fixed_pool_clear(pool->current_slot->pool);

    // clear item count
    pool->item_count = 0;

    // clear partial slots
    xu_list_entry_clear(&pool->partial_slots);

    // clear full slots
    xu_list_entry_clear(&pool->full_slots);
}
xu_pointer_t xu_fixed_pool_malloc_(xu_fixed_pool_ref_t self __xu_debug_decl__)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, xu_null);

    // done
    xu_bool_t    ok   = xu_false;
    xu_pointer_t data = xu_null;
    do
    {
        // no current slot or the current slot is full? update the current slot
        if (!pool->current_slot || xu_static_fixed_pool_full(pool->current_slot->pool))
        {
            // move the current slot to the full slots if exists
            if (pool->current_slot) xu_list_entry_insert_tail(&pool->full_slots, &pool->current_slot->entry);

            // clear the current slot
            pool->current_slot = xu_null;

            // attempt to get a slot from the partial slots
            if (!xu_list_entry_is_null(&pool->partial_slots))
            {
                // the head entry
                xu_list_entry_ref_t entry = xu_list_entry_head(&pool->partial_slots);
                xu_assert_and_check_break(entry);

                // the head slot
                pool->current_slot = (xu_fixed_pool_slot_t*)xu_list_entry(&pool->partial_slots, entry);
                xu_assert_and_check_break(pool->current_slot);

                // remove this slot from the partial slots
                xu_list_entry_remove(&pool->partial_slots, entry);
            }
            // make a new slot
            else
                pool->current_slot = xu_fixed_pool_slot_init(pool);
        }

        // check
        xu_assert_and_check_break(pool->current_slot && pool->current_slot->pool);
        xu_assert_and_check_break(!xu_static_fixed_pool_full(pool->current_slot->pool));

        // make data from the current slot
        data = xu_static_fixed_pool_malloc(pool->current_slot->pool __xu_debug_args__);
        xu_assert_and_check_break(data);

        // done init
        if (pool->func_init && !pool->func_init(data, pool->func_priv)) break;

        // update the item count
        pool->item_count++;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit data
        if (data && pool->current_slot && pool->current_slot->pool)
            xu_static_fixed_pool_free(pool->current_slot->pool, data __xu_debug_args__);
        data = xu_null;
    }

    // check
    xu_assertf(data, "malloc(%lu) failed!", pool->item_size);

    // ok?
    return data;
}
xu_pointer_t xu_fixed_pool_malloc0_(xu_fixed_pool_ref_t self __xu_debug_decl__)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, xu_null);

    // done
    xu_pointer_t data = xu_fixed_pool_malloc_(self __xu_debug_args__);
    xu_assert_and_check_return_val(data, xu_null);

    // clear it
    xu_memset_(data, 0, pool->item_size);

    // ok
    return data;
}
xu_bool_t xu_fixed_pool_free_(xu_fixed_pool_ref_t self, xu_pointer_t data __xu_debug_decl__)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return_val(pool, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // check
        xu_assertf_pass_and_check_break(pool->item_count, "double free data: %p", data);

        // find the slot
        xu_fixed_pool_slot_t* slot = xu_fixed_pool_slot_find(pool, data);
        xu_assertf_pass_and_check_break(slot, "the data: %p not belong to pool: %p", data, self);
        xu_assert_pass_and_check_break(slot->pool);

        // the slot is full?
        xu_bool_t full = xu_static_fixed_pool_full(slot->pool);

        // done exit
        if (pool->func_exit) pool->func_exit(data, pool->func_priv);

        // free it
        if (!xu_static_fixed_pool_free(slot->pool, data __xu_debug_args__)) break;

        // not the current slot?
        if (slot != pool->current_slot)
        {
            // is full? move the slot to the partial slots
            if (full)
            {
                xu_list_entry_remove(&pool->full_slots, &slot->entry);
                xu_list_entry_insert_tail(&pool->partial_slots, &slot->entry);
            }
            // is null? exit the slot
            else if (xu_static_fixed_pool_null(slot->pool))
            {
                xu_list_entry_remove(&pool->partial_slots, &slot->entry);
                xu_fixed_pool_slot_exit(pool, slot);
            }
        }

        // update the item count
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
        xu_pool_data_dump((xu_byte_t const*)data, xu_true, "[fixed_pool]: [error]: ");

        // abort
        xu_abort();
    }
#endif

    // ok?
    return ok;
}
xu_void_t xu_fixed_pool_walk(xu_fixed_pool_ref_t self, xu_fixed_pool_item_walk_func_t func, xu_cpointer_t priv)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return(pool && func);

    // walk the current slot first
    if (pool->current_slot && pool->current_slot->pool) xu_static_fixed_pool_walk(pool->current_slot->pool, func, priv);

    // walk the partial slots
    xu_for_all_if(xu_fixed_pool_slot_t*, partial_slot, xu_list_entry_itor(&pool->partial_slots),
                  partial_slot && partial_slot->pool)
    {
        // check
        xu_assert(!xu_static_fixed_pool_full(partial_slot->pool));

        // walk
        xu_static_fixed_pool_walk(partial_slot->pool, func, priv);
    }

    // walk the full slots
    xu_for_all_if(xu_fixed_pool_slot_t*, full_slot, xu_list_entry_itor(&pool->full_slots), full_slot && full_slot->pool)
    {
        // check
        xu_assert(xu_static_fixed_pool_full(full_slot->pool));

        // walk
        xu_static_fixed_pool_walk(full_slot->pool, func, priv);
    }
}
#ifdef __xu_debug__
xu_void_t xu_fixed_pool_dump(xu_fixed_pool_ref_t self)
{
    // check
    xu_fixed_pool_t* pool = (xu_fixed_pool_t*)self;
    xu_assert_and_check_return(pool);

    // dump the current slot first
    if (pool->current_slot && pool->current_slot->pool) xu_static_fixed_pool_dump(pool->current_slot->pool);

    // dump the partial slots
    xu_for_all_if(xu_fixed_pool_slot_t*, partial_slot, xu_list_entry_itor(&pool->partial_slots),
                  partial_slot && partial_slot->pool)
    {
        // check
        xu_assert(!xu_static_fixed_pool_full(partial_slot->pool));

        // dump
        xu_static_fixed_pool_dump(partial_slot->pool);
    }

    // dump the full slots
    xu_for_all_if(xu_fixed_pool_slot_t*, full_slot, xu_list_entry_itor(&pool->full_slots), full_slot && full_slot->pool)
    {
        // check
        xu_assert(xu_static_fixed_pool_full(full_slot->pool));

        // dump
        xu_static_fixed_pool_dump(full_slot->pool);
    }
}
#endif
