#pragma once

#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/*! the sorter
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param comp      the comparer
 */
xu_void_t xu_sort(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_iterator_comp_t comp);

/*! the sorter for all
 *
 * @param iterator  the iterator
 * @param comp      the comparer
 */
xu_void_t xu_sort_all(xu_iterator_ref_t iterator, xu_iterator_comp_t comp);
