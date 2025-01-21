#pragma once

#include "xutil/container/element.h"
#include "xutil/container/hash_map.h"
#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the micro hash bucket size
#define XU_HASH_SET_BUCKET_SIZE_MICRO XU_HASH_MAP_BUCKET_SIZE_MICRO

// the small hash bucket size
#define XU_HASH_SET_BUCKET_SIZE_SMALL XU_HASH_MAP_BUCKET_SIZE_SMALL

// the large hash bucket size
#define XU_HASH_SET_BUCKET_SIZE_LARGE XU_HASH_MAP_BUCKET_SIZE_LARGE

/* *******************************************************
 * types
 * *******************************************************
 */

/*! the hash set ref type
 *
 * @note the itor of the same item is mutable
 */
typedef xu_iterator_ref_t xu_hash_set_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init hash set
 *
 * @param bucket_size   the hash bucket size, using the default size if be zero
 * @param element       the element
 *
 * @return              the hash set
 */
xu_hash_set_ref_t xu_hash_set_init(xu_size_t bucket_size, xu_element_t element);

/*! exit hash set
 *
 * @param hash_set      the hash set
 */
xu_void_t xu_hash_set_exit(xu_hash_set_ref_t hash_set);

/*! clear hash set
 *
 * @param hash_set      the hash set
 */
xu_void_t xu_hash_set_clear(xu_hash_set_ref_t hash_set);

/*! get item?
 *
 * @code
 * if (xu_hash_set_get(hash_set, name))
 * {
 * }
 * @endcode
 *
 * @param hash_set      the hash set
 * @param data          the item data
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_hash_set_get(xu_hash_set_ref_t hash_set, xu_cpointer_t data);

/*! find item
 *
 * @code
 *
 * // find item
 * xu_size_t itor = xu_hash_set_find(hash_set, data);
 * if (itor != xu_iterator_tail(hash_set))
 * {
 *      // remove it
 *      xu_iterator_remove(hash_set, itor);
 * }
 * @endcode
 *
 * @param hash_set      the hash set
 * @param data          the item data
 *
 * @return              the item itor, @note: the itor of the same item is mutable
 */
xu_size_t xu_hash_set_find(xu_hash_set_ref_t hash_set, xu_cpointer_t data);

/*! insert item
 *
 * @note each item is unique
 *
 * @param hash_set      the hash set
 * @param data          the item data
 *
 * @return              the item itor, @note: the itor of the same item is mutable
 */
xu_size_t xu_hash_set_insert(xu_hash_set_ref_t hash_set, xu_cpointer_t data);

/*! remove item
 *
 * @param hash_set      the hash set
 * @param data          the item data
 */
xu_void_t xu_hash_set_remove(xu_hash_set_ref_t hash_set, xu_cpointer_t data);

/*! the hash set size
 *
 * @param hash_set      the hash set
 *
 * @return              the hash set size
 */
xu_size_t xu_hash_set_size(xu_hash_set_ref_t hash_set);

/*! the hash set maxn
 *
 * @param hash_set      the hash set
 *
 * @return              the hash set maxn
 */
xu_size_t xu_hash_set_maxn(xu_hash_set_ref_t hash_set);

#ifdef __xu_debug__
/*! dump hash
 *
 * @param hash_set      the hash set
 */
xu_void_t xu_hash_set_dump(xu_hash_set_ref_t hash_set);
#endif
