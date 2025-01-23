#include "xutil/algorithm/predicate.h"

/* *******************************************************
 * implementation
 */
xu_bool_t xu_predicate_eq(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // check
    xu_assert(iterator);

    // item == value?
    return !xu_iterator_comp(iterator, item, value);
}
xu_bool_t xu_predicate_le(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // check
    xu_assert(iterator);

    // item < value?
    return xu_iterator_comp(iterator, item, value) < 0;
}
xu_bool_t xu_predicate_be(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // check
    xu_assert(iterator);

    // item > value?
    return xu_iterator_comp(iterator, item, value) > 0;
}
xu_bool_t xu_predicate_leq(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // check
    xu_assert(iterator);

    // item <= value?
    return xu_iterator_comp(iterator, item, value) <= 0;
}
xu_bool_t xu_predicate_beq(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // check
    xu_assert(iterator);

    // item >= value?
    return xu_iterator_comp(iterator, item, value) >= 0;
}
