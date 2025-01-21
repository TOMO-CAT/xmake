#pragma once

#include "xutil/algorithm/binary_find_if.h"
#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/*! binary find item
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param item      the found item
 *
 * @return          the iterator itor, return xu_iterator_tail(iterator) if not found
 */
xu_size_t xu_binary_find(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_cpointer_t item);

/*!binary find item for all
 *
 * @param iterator  the iterator
 * @param item      the found item
 *
 * @return          the iterator itor, return xu_iterator_tail(iterator) if not found
 */
xu_size_t xu_binary_find_all(xu_iterator_ref_t iterator, xu_cpointer_t item);
