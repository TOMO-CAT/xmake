#include "xutil/algorithm/remove_if.h"

/* *******************************************************
 * private implementation
 */

static xu_bool_t xu_remove_if_pred(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value,
                                   xu_bool_t* pbreak)
{
    // check
    xu_value_ref_t tuple = (xu_value_ref_t)value;
    xu_assert(tuple && tuple[0].cptr);

    // the pred
    return ((xu_predicate_ref_t)tuple[0].cptr)(iterator, item, tuple[1].cptr);
}

/* *******************************************************
 * implementation
 */

xu_void_t xu_remove_if(xu_iterator_ref_t iterator, xu_predicate_ref_t pred, xu_cpointer_t value)
{
    // init tuple
    xu_value_t tuple[2];
    tuple[0].cptr = pred;
    tuple[1].cptr = value;

    // remove it
    xu_remove_if_until(iterator, xu_remove_if_pred, tuple);
}
xu_void_t xu_remove_if_until(xu_iterator_ref_t iterator, xu_predicate_break_ref_t pred, xu_cpointer_t value)
{
    // check
    xu_assert_and_check_return(iterator && pred);

    // the iterator mode
    xu_size_t mode = xu_iterator_mode(iterator);
    xu_assert_and_check_return((mode & XU_ITERATOR_MODE_FORWARD));
    xu_assert_and_check_return(!(mode & XU_ITERATOR_MODE_READONLY));

    // done
    xu_size_t next;
    xu_size_t size     = 0;
    xu_bool_t ok       = xu_false;
    xu_bool_t need     = xu_false;
    xu_bool_t is_break = xu_false;
    xu_size_t prev     = xu_iterator_tail(iterator);
    xu_size_t itor     = xu_iterator_head(iterator);
    xu_size_t base     = xu_iterator_tail(iterator);
    xu_bool_t bmutable = (mode & XU_ITERATOR_MODE_MUTABLE) ? xu_true : xu_false;
    while (itor != xu_iterator_tail(iterator))
    {
        // save next
        next = xu_iterator_next(iterator, itor);

        // done predicate
        ok = pred(iterator, xu_iterator_item(iterator, itor), value, &is_break);

        // remove it?
        if (ok)
        {
            // is the first removed item?
            if (!need)
            {
                // save the removed range base
                base = prev;

                // need remove items
                need = xu_true;
            }

            // update size
            size++;
        }

        // the removed range have been passed or stop or end?
        if (!ok || next == xu_iterator_tail(iterator))
        {
            // need remove items?
            if (need)
            {
                // check
                xu_assert(size);

                // the previous tail
                xu_size_t prev_tail = xu_iterator_tail(iterator);

                // remove items
                xu_iterator_nremove(iterator, base, ok ? next : itor, size);

                // reset state
                need = xu_false;
                size = 0;

                // is the mutable iterator?
                if (bmutable)
                {
                    // update itor
                    prev = base;

                    // the body items are removed?
                    if (base != prev_tail) itor = xu_iterator_next(iterator, base);
                    // the head items are removed?
                    else
                        itor = xu_iterator_head(iterator);

                    // the last item be not removed? skip the last walked item
                    if (!ok)
                    {
                        prev = itor;
                        itor = xu_iterator_next(iterator, itor);
                    }

                    // break?
                    xu_check_break(!is_break);

                    // continue?
                    continue;
                }
            }

            // break?
            xu_check_break(!is_break);
        }

        // next
        prev = itor;
        itor = next;
    }
}
