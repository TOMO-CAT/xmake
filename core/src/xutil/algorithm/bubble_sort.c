#include "xutil/algorithm/bubble_sort.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/malloc.h"

xu_void_t xu_bubble_sort(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_iterator_comp_t comp)
{
    // check
    xu_assert_and_check_return(iterator && (xu_iterator_mode(iterator) & XU_ITERATOR_MODE_FORWARD));
    xu_check_return(head != tail);

    // get flag
    xu_size_t step = xu_iterator_step(iterator);
    xu_size_t flag = xu_iterator_flag(iterator);
    if (!flag && step > sizeof(xu_pointer_t)) flag |= XU_ITERATOR_FLAG_ITEM_REF;

    // init temp item
    xu_pointer_t temp = flag & XU_ITERATOR_FLAG_ITEM_REF ? xu_malloc(step) : xu_null;
    xu_assert_and_check_return(step <= sizeof(xu_pointer_t) || temp);

    // the comparer
    if (!comp) comp = xu_iterator_comp;

    // bubble_sort
    xu_size_t itor1, itor2;
    for (itor1 = head; itor1 != tail; itor1 = xu_iterator_next(iterator, itor1))
    {
        for (itor2 = itor1, itor2 = xu_iterator_next(iterator, itor2); itor2 != tail;
             itor2 = xu_iterator_next(iterator, itor2))
        {
            if (comp(iterator, xu_iterator_item(iterator, itor2), xu_iterator_item(iterator, itor1)) < 0)
            {
                if (flag & XU_ITERATOR_FLAG_ITEM_REF)
                    xu_memcpy(temp, xu_iterator_item(iterator, itor1), step);
                else
                    temp = xu_iterator_item(iterator, itor1);
                xu_iterator_copy(iterator, itor1, xu_iterator_item(iterator, itor2));
                xu_iterator_copy(iterator, itor2, temp);
            }
        }
    }

    // free temp item
    if (temp && (flag & XU_ITERATOR_FLAG_ITEM_REF)) xu_free(temp);
}
xu_void_t xu_bubble_sort_all(xu_iterator_ref_t iterator, xu_iterator_comp_t comp)
{
    xu_bubble_sort(iterator, xu_iterator_head(iterator), xu_iterator_tail(iterator), comp);
}
