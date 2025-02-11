#pragma once

#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 */

/*! the finder
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param value     the value of the predicate
 *
 * @return          the iterator itor, return xu_iterator_tail(iterator) if not found
 */
xu_size_t xu_find(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_cpointer_t value);

/*! the finder for all
 *
 * @param iterator  the iterator
 * @param value     the value of the predicate
 *
 * @return          the iterator itor, return xu_iterator_tail(iterator) if not found
 */
xu_size_t xu_find_all(xu_iterator_ref_t iterator, xu_cpointer_t value);
