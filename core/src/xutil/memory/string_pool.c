/* *******************************************************
 * trace
 */

#define XU_TRACE_MODULE_NAME "string_pool"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/memory/string_pool.h"
#include "xutil/algorithm/for_if.h"
#include "xutil/container/hash_map.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"

// the string pool type
typedef struct __xu_string_pool_t
{
    // the cache
    xu_hash_map_ref_t cache;

} xu_string_pool_t;

xu_string_pool_ref_t xu_string_pool_init(xu_bool_t bcase)
{
    // done
    xu_bool_t         ok   = xu_false;
    xu_string_pool_t* pool = xu_null;
    do
    {
        // make pool
        pool = xu_malloc0_type(xu_string_pool_t);
        xu_assert_and_check_break(pool);

        // init hash
        pool->cache = xu_hash_map_init(0, xu_element_str(bcase), xu_element_size());
        xu_assert_and_check_break(pool->cache);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (pool) xu_string_pool_exit((xu_string_pool_ref_t)pool);
        pool = xu_null;
    }

    // ok?
    return (xu_string_pool_ref_t)pool;
}
xu_void_t xu_string_pool_exit(xu_string_pool_ref_t self)
{
    // check
    xu_string_pool_t* pool = (xu_string_pool_t*)self;
    xu_assert_and_check_return(pool);

    // exit cache
    if (pool->cache) xu_hash_map_exit(pool->cache);
    pool->cache = xu_null;

    // exit it
    xu_free(pool);
}
xu_void_t xu_string_pool_clear(xu_string_pool_ref_t self)
{
    // check
    xu_string_pool_t* pool = (xu_string_pool_t*)self;
    xu_assert_and_check_return(pool);

    // clear cache
    if (pool->cache) xu_hash_map_clear(pool->cache);
}
xu_char_t const* xu_string_pool_insert(xu_string_pool_ref_t self, xu_char_t const* data)
{
    // check
    xu_string_pool_t* pool = (xu_string_pool_t*)self;
    xu_assert_and_check_return_val(pool && data, xu_null);

    // done
    xu_char_t const* cstr = xu_null;
    if (pool->cache)
    {
        // exists?
        xu_size_t              itor;
        xu_hash_map_item_ref_t item = xu_null;
        if (((itor = xu_hash_map_find(pool->cache, data)) != xu_iterator_tail(pool->cache)) &&
            (item = (xu_hash_map_item_ref_t)xu_iterator_item(pool->cache, itor)))
        {
            // refn
            xu_size_t refn = (xu_size_t)item->data;

            // refn++
            if (refn) xu_iterator_copy(pool->cache, itor, (xu_pointer_t)(refn + 1));
            // no refn? remove it
            else
            {
                // assert
                xu_assert(0);

                // del it
                xu_iterator_remove(pool->cache, itor);
                item = xu_null;
            }
        }

        // no item? insert it
        if (!item)
        {
            // insert it
            if ((itor = xu_hash_map_insert(pool->cache, data, (xu_pointer_t)1)) != xu_iterator_tail(pool->cache))
                item = (xu_hash_map_item_ref_t)xu_iterator_item(pool->cache, itor);
        }

        // save the cstr
        if (item) cstr = (xu_char_t const*)item->name;
    }

    // ok?
    return cstr;
}
xu_void_t xu_string_pool_remove(xu_string_pool_ref_t self, xu_char_t const* data)
{
    // check
    xu_string_pool_t* pool = (xu_string_pool_t*)self;
    xu_assert_and_check_return(pool && data);

    // done
    xu_hash_map_item_ref_t item = xu_null;
    if (pool->cache)
    {
        // exists?
        xu_size_t itor;
        if (((itor = xu_hash_map_find(pool->cache, data)) != xu_iterator_tail(pool->cache)) &&
            (item = (xu_hash_map_item_ref_t)xu_iterator_item(pool->cache, itor)))
        {
            // refn
            xu_size_t refn = (xu_size_t)item->data;

            // refn--
            if (refn > 1) xu_iterator_copy(pool->cache, itor, (xu_pointer_t)(refn - 1));
            // del it
            else
                xu_iterator_remove(pool->cache, itor);
        }
    }
}
xu_bool_t xu_string_pool_has(xu_string_pool_ref_t self, xu_char_t const* data)
{
    // check
    xu_string_pool_t* pool = (xu_string_pool_t*)self;
    xu_assert_and_check_return_val(pool && data, xu_false);

    if (pool->cache)
    {
        xu_size_t itor = xu_hash_map_find(pool->cache, data);
        return itor != xu_iterator_tail(pool->cache);
    }
    return xu_false;
}
#ifdef __xu_debug__
xu_void_t xu_string_pool_dump(xu_string_pool_ref_t self)
{
    // check
    xu_string_pool_t* pool = (xu_string_pool_t*)self;
    xu_assert_and_check_return(pool && pool->cache);

    // dump cache
    xu_for_all_if(xu_hash_map_item_ref_t, item, pool->cache, item)
    {
        // trace
        xu_trace_i("item: refn: %lu, cstr: %s", (xu_size_t)item->data, item->name);
    }
}
#endif
