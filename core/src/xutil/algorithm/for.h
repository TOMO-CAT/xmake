#pragma once

#include "xutil/prefix.h"

/*! for items using iterator
 *
 * @code
 * xu_for(xu_char_t*, item, xu_iterator_head(iterator), xu_iterator_tail(iterator), iterator)
 * {
 *      xu_trace_d("item: %s", item);
 * }
 *
 * xu_for(xu_size_t, item, xu_iterator_head(iterator), xu_iterator_tail(iterator), iterator)
 * {
 *      xu_trace_d("item: %lu", item);
 * }
 *
 * xu_for(xu_hash_map_item_ref_t, item, xu_iterator_head(iterator), xu_iterator_tail(iterator), iterator)
 * {
 *      if (item) xu_trace_d("item: %p => %p", item->name, item->data);
 * }
 * @endcode
 */
#define xu_for(type, item, head, tail, iterator)                                                                       \
    /* iterator */                                                                                                     \
    xu_iterator_ref_t item##_iterator = (xu_iterator_ref_t)iterator;                                                   \
    xu_assert(!item##_iterator ||                                                                                      \
              (xu_iterator_mode(item##_iterator) & (XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_RACCESS)));            \
    /* init */                                                                                                         \
    type      item;                                                                                                    \
    xu_size_t item##_itor = head;                                                                                      \
    xu_size_t item##_head = head;                                                                                      \
    xu_size_t item##_tail = tail;                                                                                      \
    /* walk */                                                                                                         \
    if (item##_iterator && item##_head != item##_tail)                                                                 \
        for (; item##_itor != item##_tail && ((item = (type)xu_iterator_item(item##_iterator, item##_itor)), 1);       \
             item##_itor = xu_iterator_next(item##_iterator, item##_itor))

/*! for all items using iterator
 *
 * @code
 *
 * xu_for_all(xu_char_t*, item, iterator)
 * {
 *      xu_trace_d("item: %s", item);
 * }
 *
 * xu_for_all(xu_size_t, item, iterator)
 * {
 *      xu_trace_d("item: %lu", item);
 * }
 *
 * xu_for_all(xu_hash_map_item_ref_t, item, iterator)
 * {
 *      if (item) xu_trace_d("item: %p => %p", item->name, item->data);
 * }
 * @endcode
 */
#define xu_for_all(type, item, iterator)                                                                               \
    xu_iterator_ref_t item##_iterator_all = (xu_iterator_ref_t)iterator;                                               \
    xu_for(type, item, xu_iterator_head(item##_iterator_all), xu_iterator_tail(item##_iterator_all),                   \
           item##_iterator_all)
