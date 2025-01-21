#include "xutil/algorithm/quick_sort.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/malloc.h"

xu_void_t xu_quick_sort(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_iterator_comp_t comp)
{
    // check
    xu_assert_and_check_return(iterator && (xu_iterator_mode(iterator) & XU_ITERATOR_MODE_RACCESS));
    xu_check_return(head != tail);

    // get flag
    xu_size_t step = xu_iterator_step(iterator);
    xu_size_t flag = xu_iterator_flag(iterator);
    if (!flag && step > sizeof(xu_pointer_t)) flag |= XU_ITERATOR_FLAG_ITEM_REF;

    // init key item
    xu_pointer_t key = (flag & XU_ITERATOR_FLAG_ITEM_REF) ? xu_malloc(step) : xu_null;
    xu_assert_and_check_return(step <= sizeof(xu_pointer_t) || key);

    // the comparer
    if (!comp) comp = xu_iterator_comp;

    // hole => key
    if (flag & XU_ITERATOR_FLAG_ITEM_REF)
        xu_memcpy(key, xu_iterator_item(iterator, head), step);
    else
        key = xu_iterator_item(iterator, head);

    // quick_sort
    xu_size_t l = head;
    xu_size_t r = tail - 1;
    while (r > l)
    {
        // find: <=
        for (; r != l; r--)
            if (comp(iterator, xu_iterator_item(iterator, r), key) < 0) break;
        if (r != l)
        {
            xu_iterator_copy(iterator, l, xu_iterator_item(iterator, r));
            l++;
        }

        // find: =>
        for (; l != r; l++)
            if (comp(iterator, xu_iterator_item(iterator, l), key) > 0) break;
        if (l != r)
        {
            xu_iterator_copy(iterator, r, xu_iterator_item(iterator, l));
            r--;
        }
    }

    // key => hole
    xu_iterator_copy(iterator, l, key);

    // quick_sort [head, hole - 1]
    xu_quick_sort(iterator, head, l, comp);

    // quick_sort [hole + 1, tail]
    xu_quick_sort(iterator, ++l, tail, comp);

    // free key item
    if (key && (flag & XU_ITERATOR_FLAG_ITEM_REF)) xu_free(key);
}
xu_void_t xu_quick_sort_all(xu_iterator_ref_t iterator, xu_iterator_comp_t comp)
{
    xu_quick_sort(iterator, xu_iterator_head(iterator), xu_iterator_tail(iterator), comp);
}
