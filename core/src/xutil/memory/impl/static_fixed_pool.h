#pragma once

#include "xutil/memory/fixed_pool.h"
#include "xutil/memory/impl/prefix.h"
#include "xutil/prefix.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * types
 * *******************************************************
 */

/*! the static fixed pool ref type
 *
 * <pre>
 *  ---------------------------------------------------------------------------
 * |  head   |      used       |                    data                       |
 *  ---------------------------------------------------------------------------
 *               |
 *              pred
 * </pre>
 */
typedef __xu_typeref__(static_fixed_pool);

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! init pool
 *
 * @param data              the data address
 * @param size              the data size
 * @param item_size         the item size
 * @param for_small         add data size field at head for the small allocator
 *
 * @return                  the pool
 */
xu_static_fixed_pool_ref_t xu_static_fixed_pool_init(xu_byte_t* data, xu_size_t size, xu_size_t item_size,
                                                     xu_bool_t for_small);

/*! exit pool
 *
 * @param pool              the pool
 */
xu_void_t xu_static_fixed_pool_exit(xu_static_fixed_pool_ref_t pool);

/*! the item count
 *
 * @param pool              the pool
 *
 * @return                  the item count
 */
xu_size_t xu_static_fixed_pool_size(xu_static_fixed_pool_ref_t pool);

/*! the item maximum count
 *
 * @param pool              the pool
 *
 * @return                  the item maximum count
 */
xu_size_t xu_static_fixed_pool_maxn(xu_static_fixed_pool_ref_t pool);

/*! is full?
 *
 * @param pool              the pool
 *
 * @return                  xu_true or xu_false
 */
xu_bool_t xu_static_fixed_pool_full(xu_static_fixed_pool_ref_t pool);

/*! is null?
 *
 * @param pool              the pool
 *
 * @return                  xu_true or xu_false
 */
xu_bool_t xu_static_fixed_pool_null(xu_static_fixed_pool_ref_t pool);

/*! clear pool
 *
 * @param pool              the pool
 */
xu_void_t xu_static_fixed_pool_clear(xu_static_fixed_pool_ref_t pool);

/*! malloc data
 *
 * @param pool              the pool
 *
 * @return                  the data
 */
xu_pointer_t xu_static_fixed_pool_malloc(xu_static_fixed_pool_ref_t pool __xu_debug_decl__);

/*! free data
 *
 * @param pool              the pool
 * @param data              the data
 *
 * @return                  xu_true or xu_false
 */
xu_bool_t xu_static_fixed_pool_free(xu_static_fixed_pool_ref_t pool, xu_pointer_t data __xu_debug_decl__);

/*! walk data
 *
 * @code
 * xu_bool_t xu_static_fixed_pool_item_func(xu_pointer_t data, xu_cpointer_t priv)
 * {
 *      // ok or break
 *      return xu_true;
 * }
 * @endcode
 *
 * @param pool              the pool
 * @param func              the walk func
 * @param priv              the walk data
 *
 */
xu_void_t xu_static_fixed_pool_walk(xu_static_fixed_pool_ref_t pool, xu_fixed_pool_item_walk_func_t func,
                                    xu_cpointer_t priv);

#ifdef __xu_debug__
/*! dump pool
 *
 * @param pool              the pool
 */
xu_void_t xu_static_fixed_pool_dump(xu_static_fixed_pool_ref_t pool);
#endif
