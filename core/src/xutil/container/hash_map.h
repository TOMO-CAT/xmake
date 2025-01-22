#pragma once

#include "xutil/container/element.h"
#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

// the micro hash bucket size
#define XU_HASH_MAP_BUCKET_SIZE_MICRO (64)

// the small hash bucket size
#define XU_HASH_MAP_BUCKET_SIZE_SMALL (256)

// the large hash bucket size
#define XU_HASH_MAP_BUCKET_SIZE_LARGE (65536)

/* *******************************************************
 * types
 */

// the hash map item type
typedef struct __xu_hash_map_item_t
{
    /// the item name
    xu_pointer_t name;

    /// the item data
    xu_pointer_t data;

} xu_hash_map_item_t, *xu_hash_map_item_ref_t;

/*! the hash map ref type
 *
 * <pre>
 *                 0        1        3       ...     ...                n       n + 1
 * hash_list: |--------|--------|--------|--------|--------|--------|--------|--------|
 *                         |
 *                       -----
 * item_list:           |     |       key:0
 *                       -----
 *                      |     |       key:1
 *                       -----               <= insert by binary search algorithm
 *                      |     |       key:2
 *                       -----
 *                      |     |       key:3
 *                       -----
 *                      |     |       key:4
 *                       -----
 *                      |     |
 *                       -----
 *                      |     |
 *                       -----
 *                      |     |
 *                       -----
 *
 * </pre>
 *
 * @note the itor of the same item is mutable
 */
typedef xu_iterator_ref_t xu_hash_map_ref_t;

/* *******************************************************
 * interfaces
 */

/*! init hash map
 *
 * @param bucket_size   the hash bucket size, using the default size if be zero
 * @param element_name  the item for name
 * @param element_data  the item for data
 *
 * @return              the hash map
 */
xu_hash_map_ref_t xu_hash_map_init(xu_size_t bucket_size, xu_element_t element_name, xu_element_t element_data);

/*! exit hash map
 *
 * @param hash_map      the hash map
 */
xu_void_t xu_hash_map_exit(xu_hash_map_ref_t hash_map);

/*! clear hash map
 *
 * @param hash_map      the hash map
 */
xu_void_t xu_hash_map_clear(xu_hash_map_ref_t hash_map);

/*! get item data from name
 *
 * @note
 * the return value may be zero if the item type is integer
 * so we need call xu_hash_map_find for judging whether to get value successfully
 *
 * @code
 *
 * // find item and get item data
 * xu_xxxx_ref_t data = (xu_xxxx_ref_t)xu_hash_map_get(hash_map, name);
 * if (data)
 * {
 *      // ...
 * }
 * @endcode
 *
 * @param hash_map      the hash map
 * @param name          the item name
 *
 * @return              the item data
 */
xu_pointer_t xu_hash_map_get(xu_hash_map_ref_t hash_map, xu_cpointer_t name);

/*! find item from name
 *
 * @code
 *
 * // find item
 * xu_size_t itor = xu_hash_map_find(hash_map, name);
 * if (itor != xu_iterator_tail(hash_map))
 * {
 *      // get data
 *      xu_xxxx_ref_t data = (xu_xxxx_ref_t)xu_iterator_item(hash_map, itor);
 *      xu_assert(data);
 *
 *      // remove it
 *      xu_iterator_remove(hash_map, itor);
 * }
 * @endcode
 *
 * @param hash_map      the hash map
 * @param name          the item name
 *
 * @return              the item itor, @note: the itor of the same item is mutable
 */
xu_size_t xu_hash_map_find(xu_hash_map_ref_t hash_map, xu_cpointer_t name);

/*! insert item data from name
 *
 * @note the pair (name => data) is unique
 *
 * @param hash_map      the hash map
 * @param name          the item name
 * @param data          the item data
 *
 * @return              the item itor, @note: the itor of the same item is mutable
 */
xu_size_t xu_hash_map_insert(xu_hash_map_ref_t hash_map, xu_cpointer_t name, xu_cpointer_t data);

/*! remove item from name
 *
 * @param hash_map      the hash map
 * @param name          the item name
 */
xu_void_t xu_hash_map_remove(xu_hash_map_ref_t hash_map, xu_cpointer_t name);

/*! the hash map size
 *
 * @param hash_map      the hash map
 *
 * @return              the hash map size
 */
xu_size_t xu_hash_map_size(xu_hash_map_ref_t hash_map);

/*! the hash map maxn
 *
 * @param hash_map      the hash map
 *
 * @return              the hash map maxn
 */
xu_size_t xu_hash_map_maxn(xu_hash_map_ref_t hash_map);

#ifdef __xu_debug__
/*! dump hash
 *
 * @param hash_map      the hash map
 */
xu_void_t xu_hash_map_dump(xu_hash_map_ref_t hash_map);
#endif
