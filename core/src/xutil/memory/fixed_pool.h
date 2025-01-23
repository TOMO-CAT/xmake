#pragma once

#include "xutil/memory/allocator.h"
#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

#define xu_fixed_pool_malloc(pool) xu_fixed_pool_malloc_(pool __xu_debug_vals__)
#define xu_fixed_pool_malloc0(pool) xu_fixed_pool_malloc0_(pool __xu_debug_vals__)
#define xu_fixed_pool_free(pool, item) xu_fixed_pool_free_(pool, (xu_pointer_t)(item)__xu_debug_vals__)

/* *******************************************************
 * types
 */

/*! the fixed pool ref type
 *
 * <pre>
 *
 * current:
 *         -----------
 *        |           |
 *  --------------    |
 * |     slot     |<--
 * |--------------|
 * ||||||||||||||||
 * |--------------|
 * |              |
 * |--------------|
 * |              |
 * |--------------|
 * ||||||||||||||||
 * |--------------|
 * ||||||||||||||||
 * |--------------|
 * |              |
 *  --------------
 *
 * partial:
 *
 *  --------------       --------------               --------------
 * |     slot     | <=> |     slot     | <=> ... <=> |     slot     |
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     |              |             |              |
 * |--------------|     |--------------|             |--------------|
 * |              |     ||||||||||||||||             |              |
 * |--------------|     |--------------|             |--------------|
 * |              |     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             |              |
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     |              |             |              |
 * |--------------|     |--------------|             |--------------|
 * |              |     |              |             ||||||||||||||||
 *  --------------       --------------               --------------
 *
 * full:
 *
 *  --------------       --------------               --------------
 * |     slot     | <=> |     slot     | <=> ... <=> |     slot     |
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 *  --------------       --------------               --------------
 *
 * slot:
 *
 *  -------------- ------------------------>|
 * |     head     |                         |
 * |--------------|                         |
 * |||   item     |                         |
 * |--------------|                         |
 * |||   item     |                         |
 * |--------------|                         | data
 * |||   item     |                         |
 * |--------------|                         |
 * |      ...     |                         |
 * |--------------|                         |
 * |||   item     |                         |
 *  -------------- ------------------------>|
 *
 * </pre>
 */
typedef __xu_typeref__(fixed_pool);

// the item init func type
typedef xu_bool_t (*xu_fixed_pool_item_init_func_t)(xu_pointer_t data, xu_cpointer_t priv);

// the item exit func type
typedef xu_void_t (*xu_fixed_pool_item_exit_func_t)(xu_pointer_t data, xu_cpointer_t priv);

// the item walk func type
typedef xu_bool_t (*xu_fixed_pool_item_walk_func_t)(xu_pointer_t data, xu_cpointer_t priv);

/* *******************************************************
 * interfaces
 */

/*! init fixed pool
 *
 * @param large_allocator   the large allocator, uses the global allocator if be null
 * @param slot_size         the item count per-slot, using the default size if be zero
 * @param item_size         the item size
 * @param item_init         the item init func
 * @param item_exit         the item exit func
 * @param priv              the private data
 *
 * @return                  the pool
 */
xu_fixed_pool_ref_t xu_fixed_pool_init(xu_allocator_ref_t large_allocator, xu_size_t slot_size, xu_size_t item_size,
                                       xu_fixed_pool_item_init_func_t item_init,
                                       xu_fixed_pool_item_exit_func_t item_exit, xu_cpointer_t priv);

/*! exit pool
 *
 * @param pool              the pool
 */
xu_void_t xu_fixed_pool_exit(xu_fixed_pool_ref_t pool);

/*! the item count
 *
 * @param pool              the pool
 *
 * @return                  the item count
 */
xu_size_t xu_fixed_pool_size(xu_fixed_pool_ref_t pool);

/*! the item size
 *
 * @param pool              the pool
 *
 * @return                  the item size
 */
xu_size_t xu_fixed_pool_item_size(xu_fixed_pool_ref_t pool);

/*! clear pool
 *
 * @param pool              the pool
 */
xu_void_t xu_fixed_pool_clear(xu_fixed_pool_ref_t pool);

/*! malloc data
 *
 * @param pool              the pool
 *
 * @return                  the data
 */
xu_pointer_t xu_fixed_pool_malloc_(xu_fixed_pool_ref_t pool __xu_debug_decl__);

/*! malloc data and clear it
 *
 * @param pool              the pool
 *
 * @return                  the data
 */
xu_pointer_t xu_fixed_pool_malloc0_(xu_fixed_pool_ref_t pool __xu_debug_decl__);

/*! free data
 *
 * @param pool              the pool
 * @param data              the data
 *
 * @return                  xu_true or xu_false
 */
xu_bool_t xu_fixed_pool_free_(xu_fixed_pool_ref_t pool, xu_pointer_t data __xu_debug_decl__);

/*! walk item
 *
 * @code
    xu_bool_t xu_fixed_pool_item_func(xu_pointer_t data, xu_cpointer_t priv)
    {
        // ok or break
        return xu_true;
    }
 * @endcode
 *
 * @param pool              the pool
 * @param func              the walk func
 * @param priv              the private data
 */
xu_void_t xu_fixed_pool_walk(xu_fixed_pool_ref_t pool, xu_fixed_pool_item_walk_func_t func, xu_cpointer_t priv);

#ifdef __xu_debug__
/*! dump pool
 *
 * @param pool              the pool
 */
xu_void_t xu_fixed_pool_dump(xu_fixed_pool_ref_t pool);
#endif
