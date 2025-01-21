#pragma once

#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/*! compute distance from head to tail
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 *
 * @return          the distance
 */
xu_size_t xu_distance(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail);
