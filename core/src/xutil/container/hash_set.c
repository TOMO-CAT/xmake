/* *******************************************************
 * trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "hash_set"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/container/hash_set.h"
#include "xutil/container/hash_map.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * types
 * *******************************************************
 */

// the hash map itor item func type
typedef xu_pointer_t (*gb_hash_map_item_func_t)(xu_iterator_ref_t, xu_size_t);

/* *******************************************************
 * private implementation
 * *******************************************************
 */

static xu_pointer_t xu_hash_set_itor_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(iterator && iterator->priv);

    // the item func for the hash map
    gb_hash_map_item_func_t func = (gb_hash_map_item_func_t)iterator->priv;

    // get the item of the hash map
    xu_hash_map_item_ref_t item = (xu_hash_map_item_ref_t)func(iterator, itor);

    // get the item of the hash set
    return item ? item->name : xu_null;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_hash_set_ref_t xu_hash_set_init(xu_size_t bucket_size, xu_element_t element)
{
    // init hash set
    xu_iterator_ref_t hash_set = (xu_iterator_ref_t)xu_hash_map_init(bucket_size, element, xu_element_true());
    xu_assert_and_check_return_val(hash_set, xu_null);

    // @note the private data of the hash map iterator cannot be used
    xu_assert(!hash_set->priv);

    // init operation
    static xu_iterator_op_t op = {0};
    if (op.item != xu_hash_set_itor_item)
    {
        op      = *hash_set->op;
        op.item = xu_hash_set_itor_item;
    }

    // hacking hash_map and hook the item
    hash_set->priv = (xu_pointer_t)hash_set->op->item;
    hash_set->op   = &op;

    // ok?
    return (xu_hash_set_ref_t)hash_set;
}
xu_void_t xu_hash_set_exit(xu_hash_set_ref_t self) { xu_hash_map_exit((xu_hash_map_ref_t)self); }
xu_void_t xu_hash_set_clear(xu_hash_set_ref_t self) { xu_hash_map_clear((xu_hash_map_ref_t)self); }
xu_bool_t xu_hash_set_get(xu_hash_set_ref_t self, xu_cpointer_t data)
{
    return xu_p2b(xu_hash_map_get((xu_hash_map_ref_t)self, data));
}
xu_size_t xu_hash_set_find(xu_hash_set_ref_t self, xu_cpointer_t data)
{
    return xu_hash_map_find((xu_hash_map_ref_t)self, data);
}
xu_size_t xu_hash_set_insert(xu_hash_set_ref_t self, xu_cpointer_t data)
{
    return xu_hash_map_insert((xu_hash_map_ref_t)self, data, xu_b2p(xu_true));
}
xu_void_t xu_hash_set_remove(xu_hash_set_ref_t self, xu_cpointer_t data)
{
    xu_hash_map_remove((xu_hash_map_ref_t)self, data);
}
xu_size_t xu_hash_set_size(xu_hash_set_ref_t self) { return xu_hash_map_size((xu_hash_map_ref_t)self); }
xu_size_t xu_hash_set_maxn(xu_hash_set_ref_t self) { return xu_hash_map_maxn((xu_hash_map_ref_t)self); }
#ifdef __xu_debug__
xu_void_t xu_hash_set_dump(xu_hash_set_ref_t self) { xu_hash_map_dump((xu_hash_map_ref_t)self); }
#endif
