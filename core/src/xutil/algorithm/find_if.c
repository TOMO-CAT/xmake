#include "xutil/algorithm/find_if.h"

xu_size_t xu_find_if(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_predicate_ref_t pred,
                     xu_cpointer_t value)
{
    // check
    xu_assert_and_check_return_val(pred && iterator && (xu_iterator_mode(iterator) & XU_ITERATOR_MODE_FORWARD),
                                   xu_iterator_tail(iterator));

    // null?
    xu_check_return_val(head != tail, xu_iterator_tail(iterator));

    // find
    xu_size_t itor = head;
    xu_bool_t find = xu_false;
    for (; itor != tail; itor = xu_iterator_next(iterator, itor))
        if ((find = pred(iterator, xu_iterator_item(iterator, itor), value))) break;

    // ok?
    return find ? itor : xu_iterator_tail(iterator);
}
xu_size_t xu_find_all_if(xu_iterator_ref_t iterator, xu_predicate_ref_t pred, xu_cpointer_t value)
{
    return xu_find_if(iterator, xu_iterator_head(iterator), xu_iterator_tail(iterator), pred, value);
}
