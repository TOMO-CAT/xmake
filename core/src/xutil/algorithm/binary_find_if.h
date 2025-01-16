#pragma once

#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/*! binary find item if !comp(item, priv)
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param comp      the comparer func
 * @param priv      the comparer data
 *
 * @return          the iterator itor, return xu_iterator_tail(iterator) if not found
 */
xu_size_t xu_binary_find_if(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_iterator_comp_t comp,
                            xu_cpointer_t priv);

/*! binary find item for all if !comp(item, priv)
 *
 * @param iterator  the iterator
 * @param comp      the comparer func
 * @param priv      the comparer data
 *
 * @return          the iterator itor, return xu_iterator_tail(iterator) if not found
 */
xu_size_t xu_binary_find_all_if(xu_iterator_ref_t iterator, xu_iterator_comp_t comp, xu_cpointer_t priv);
