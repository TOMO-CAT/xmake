#pragma once

#include "xutil/algorithm/predicate.h"
#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 */

/*! remove items if pred(item, value)
 *
 * @param iterator  the iterator
 * @param pred      the predicate
 * @param value     the value of the predicate
 */
xu_void_t xu_remove_if(xu_iterator_ref_t iterator, xu_predicate_ref_t pred, xu_cpointer_t value);

/*! remove items if pred(item, value, &is_break) until is_break == xu_true
 *
 * @param iterator  the iterator
 * @param pred      the predicate with break
 * @param value     the value of the predicate
 */
xu_void_t xu_remove_if_until(xu_iterator_ref_t iterator, xu_predicate_break_ref_t pred, xu_cpointer_t value);
