#pragma once

#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 */

/*! the predicate ref type
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          xu_true or xu_false
 */
typedef xu_bool_t (*xu_predicate_ref_t)(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value);

/*! the predicate break ref type
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 * @param is_break  is break now?
 *
 * @return          xu_true or xu_false
 */
typedef xu_bool_t (*xu_predicate_break_ref_t)(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value,
                                              xu_bool_t* is_break);

/* *******************************************************
 * interfaces
 */

/*! the predicate: if (item == value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_predicate_eq(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value);

/*! the predicate: if (item < value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_predicate_le(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value);

/*! the predicate: if (item > value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_predicate_be(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value);

/*! the predicate: if (item <= value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_predicate_leq(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value);

/*! the predicate: if (item >= value)?
 *
 * @param iterator  the iterator
 * @param item      the inner item of the container
 * @param value     the outer value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_predicate_beq(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value);
