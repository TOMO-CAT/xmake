#include "xutil/algorithm/insert_sort.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/malloc.h"

/*!the insertion sort
 *
 * <pre>
 * old:     5       2       6       2       8       6       1
 *
 *        (hole)
 * step1: ((5))     2       6       2       8       6       1
 *        (next) <=
 *
 *        (hole)
 * step2: ((2))    (5)      6       2       8       6       1
 *                (next) <=
 *
 *                        (hole)
 * step3:   2       5     ((6))     2       8       6       1
 *                        (next) <=
 *
 *                 (hole)
 * step4:   2      ((2))   (5)     (6)      8       6       1
 *                                (next) <=
 *
 *                                        (hole)
 * step5:   2       2       5       6     ((8))     6       1
 *                                        (next) <=
 *
 *                                        (hole)
 * step6:   2       2       5       6     ((6))    (8)       1
 *                                                (next) <=
 *
 *        (hole)
 * step7: ((1))    (2)     (2)     (5)     (6)     (6)      (8)
 *                                                        (next)
 * </pre>
 */
xu_void_t xu_insert_sort(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_iterator_comp_t comp)
{
    // check
    xu_assert_and_check_return(iterator);
    xu_assert_and_check_return((xu_iterator_mode(iterator) & XU_ITERATOR_MODE_FORWARD));
    xu_assert_and_check_return((xu_iterator_mode(iterator) & XU_ITERATOR_MODE_REVERSE));
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

    // sort
    xu_size_t last, next;
    for (next = xu_iterator_next(iterator, head); next != tail; next = xu_iterator_next(iterator, next))
    {
        // save next
        if (flag & XU_ITERATOR_FLAG_ITEM_REF)
            xu_memcpy(temp, xu_iterator_item(iterator, next), step);
        else
            temp = xu_iterator_item(iterator, next);

        // look for hole and move elements[hole, next - 1] => [hole + 1, next]
        for (last = next; last != head && (last = xu_iterator_prev(iterator, last),
                                          comp(iterator, temp, xu_iterator_item(iterator, last)) < 0);
             next = last)
            xu_iterator_copy(iterator, next, xu_iterator_item(iterator, last));

        // item => hole
        xu_iterator_copy(iterator, next, temp);
    }

    // free temp item
    if (temp && (flag & XU_ITERATOR_FLAG_ITEM_REF)) xu_free(temp);
}
xu_void_t xu_insert_sort_all(xu_iterator_ref_t iterator, xu_iterator_comp_t comp)
{
    xu_insert_sort(iterator, xu_iterator_head(iterator), xu_iterator_tail(iterator), comp);
}
