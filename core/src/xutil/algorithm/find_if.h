#pragma once

#include "xutil/algorithm/predicate.h"
#include "xutil/prefix.h"

/*! find item if pred(item, value)
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param pred      the predicate
 * @param value     the value of the predicate
 *
 * @return          the iterator itor, return xu_iterator_tail(iterator) if not found
 */
xu_size_t xu_find_if(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_predicate_ref_t pred,
                     xu_cpointer_t value);

/*! find item for all if pred(item, value)
 *
 * @param iterator  the iterator
 * @param pred      the predicate
 * @param value     the value of the predicate
 *
 * @return          the iterator itor, return xu_iterator_tail(iterator) if not found
 */
xu_size_t xu_find_all_if(xu_iterator_ref_t iterator, xu_predicate_ref_t pred, xu_cpointer_t value);
