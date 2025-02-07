#include "xutil/algorithm/remove_first_if.h"

/* *******************************************************
 * implementation
 */
xu_void_t xu_remove_first_if(xu_iterator_ref_t iterator, xu_predicate_ref_t pred, xu_cpointer_t value)
{
    // check
    xu_assert_and_check_return(iterator && pred);

    // the iterator mode
    xu_size_t mode = xu_iterator_mode(iterator);
    xu_assert_and_check_return((mode & XU_ITERATOR_MODE_FORWARD));
    xu_assert_and_check_return(!(mode & XU_ITERATOR_MODE_READONLY));

    // done
    xu_size_t itor = xu_iterator_head(iterator);
    while (itor != xu_iterator_tail(iterator))
    {
        // done predicate
        if (pred(iterator, xu_iterator_item(iterator, itor), value))
        {
            // remove it
            xu_iterator_remove(iterator, itor);
            break;
        }

        // next
        itor = xu_iterator_next(iterator, itor);
    }
}
