#pragma once

#include "xutil/prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/*! for items using iterator if the condition ok
 *
 * @code
 * xu_for_if(xu_char_t*, item, xu_iterator_head(iterator), xu_iterator_tail(iterator), iterator, item)
 * {
 *      xu_trace_d("item: %s", item);
 * }
 *
 * xu_for_if(xu_size_t, item, xu_iterator_head(iterator), xu_iterator_tail(iterator), iterator, item > 10)
 * {
 *      xu_trace_d("item: %lu", item);
 * }
 *
 * xu_for_if(xu_hash_map_item_ref_t, item, xu_iterator_head(iterator), xu_iterator_tail(iterator), iterator, item !=
 * xu_null)
 * {
 *      xu_trace_d("item: %p => %p", item->name, item->data);
 * }
 * @endcode
 */
#define xu_for_if(type, item, head, tail, iterator, cond)                                                              \
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
             item##_itor = xu_iterator_next(item##_iterator, item##_itor))                                             \
            if ((cond))

/*! for all items using iterator if the condition ok
 *
 * @code
 *
 * xu_for_all_if(xu_char_t*, item, iterator, item)
 * {
 *      xu_trace_d("item: %s", item);
 * }
 *
 * xu_for_all_if(xu_size_t, item, iterator, item > 10)
 * {
 *      xu_trace_d("item: %lu", item);
 * }
 *
 * xu_for_all_if(xu_hash_map_item_ref_t, item, iterator, item != xu_null)
 * {
 *      if (item) xu_trace_d("item: %p => %p", item->name, item->data);
 * }
 * @endcode
 */
#define xu_for_all_if(type, item, iterator, cond)                                                                      \
    xu_iterator_ref_t item##_iterator_all = (xu_iterator_ref_t)iterator;                                               \
    xu_for_if(type, item, xu_iterator_head(item##_iterator_all), xu_iterator_tail(item##_iterator_all),                \
              item##_iterator_all, cond)
