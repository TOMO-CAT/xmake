#include "xutil/algorithm/walk.h"

xu_size_t xu_walk(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_walk_func_t func, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(iterator && (xu_iterator_mode(iterator) & XU_ITERATOR_MODE_FORWARD) && func, 0);

    // null?
    xu_check_return_val(head != tail, 0);

    // walk
    xu_size_t count = 0;
    xu_for(xu_pointer_t, item, head, tail, iterator)
    {
        // done
        if (!func(iterator, item, priv)) break;

        // count++
        count++;
    }

    // ok?
    return count;
}
xu_size_t xu_walk_all(xu_iterator_ref_t iterator, xu_walk_func_t func, xu_cpointer_t priv)
{
    return xu_walk(iterator, xu_iterator_head(iterator), xu_iterator_tail(iterator), func, priv);
}
