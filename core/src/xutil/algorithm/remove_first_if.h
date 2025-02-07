#include "xutil/prefix.h"
#include "xutil/algorithm/predicate.h"

/* *******************************************************
 * interfaces
 */

/*! remove the first item if pred(item, value)
 *
 * @param iterator  the iterator
 * @param pred      the predicate
 * @param value     the value of the predicate
 */
xu_void_t           xu_remove_first_if(xu_iterator_ref_t iterator, xu_predicate_ref_t pred, xu_cpointer_t value);
