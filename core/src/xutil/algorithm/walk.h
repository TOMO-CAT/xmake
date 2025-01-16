#pragma once

#include "xutil/algorithm/for.h"
#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

// the walk func type
typedef xu_bool_t (*xu_walk_func_t)(xu_iterator_ref_t iterator, xu_pointer_t item, xu_cpointer_t priv);

/*! the walker
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param func      the walker func
 * @param priv      the func private data
 *
 * @return          the item count
 */
xu_size_t xu_walk(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_walk_func_t func, xu_cpointer_t priv);

/*! the walker for all
 *
 * @param iterator  the iterator
 * @param func      the walker func
 * @param priv      the func private data
 *
 * @return          the item count
 */
xu_size_t xu_walk_all(xu_iterator_ref_t iterator, xu_walk_func_t func, xu_cpointer_t priv);
