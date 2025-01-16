#include "xutil/algorithm/binary_find_if.h"

xu_size_t xu_binary_find_if(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_iterator_comp_t comp,
                            xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(comp && iterator && (xu_iterator_mode(iterator) & XU_ITERATOR_MODE_RACCESS),
                                   xu_iterator_tail(iterator));

    // null?
    xu_check_return_val(head != tail, xu_iterator_tail(iterator));

    // find
    xu_size_t l = head;
    xu_size_t r = tail;
    xu_size_t m = (l + r) >> 1;
    xu_long_t c = -1;
    while (l < r)
    {
        c = comp(iterator, xu_iterator_item(iterator, m), priv);
        if (c > 0)
            r = m;
        else if (c < 0)
            l = m + 1;
        else
            break;
        m = (l + r) >> 1;
    }

    // ok?
    return !c ? m : xu_iterator_tail(iterator);
}
xu_size_t xu_binary_find_all_if(xu_iterator_ref_t iterator, xu_iterator_comp_t comp, xu_cpointer_t priv)
{
    return xu_binary_find_if(iterator, xu_iterator_head(iterator), xu_iterator_tail(iterator), comp, priv);
}
