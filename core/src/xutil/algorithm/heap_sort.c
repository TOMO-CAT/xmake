#include "xutil/algorithm/heap_sort.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/malloc.h"

/* *******************************************************
 * head
 */

#ifdef __xu_debug__
static __xu_inline__ xu_bool_t xu_heap_check(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail,
                                             xu_iterator_comp_t comp)
{
    // the comparer
    if (!comp) comp = xu_iterator_comp;

    // walk
    if (head != tail)
    {
        xu_size_t root;
        for (root = head; ++head != tail; ++root)
        {
            // root < left?
            if (xu_iterator_comp(iterator, xu_iterator_item(iterator, root), xu_iterator_item(iterator, head)) < 0)
                return xu_false;
            // end?
            else if (++head == tail)
                break;
            // root < right?
            else if (xu_iterator_comp(iterator, xu_iterator_item(iterator, root), xu_iterator_item(iterator, head)) < 0)
                return xu_false;
        }
    }

    // ok
    return xu_true;
}
#endif

/*!push heap
 *
 * <pre>
 * hole: bottom => top
 * init:
 *                                          16(top)
 *                               -------------------------
 *                              |                         |
 *                              14                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8(parent)      7           9             3
 *                   ---------
 *                  |         |
 *                  2      (hole) <= 11(val)
 * after:
 *                                          16(top)
 *                               -------------------------
 *                              |                         |
 *                              14(parent)                10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       11(hole)       7           9             3
 *                   ---------
 *                  |         |
 *                  2         8
 * </pre>
 */
#if 0
static __xu_inline__ xu_void_t xu_heap_push(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t hole, xu_size_t top, xu_cpointer_t item, xu_iterator_comp_t comp)
{
    // check
    xu_assert_and_check_return(comp);

    // (hole - 1) / 2: the parent node of the hole
    // finds the final hole
    xu_size_t       parent = 0;
    xu_cpointer_t   parent_item = xu_null;
    for (parent = (hole - 1) >> 1; hole > top && (comp(iterator, (parent_item = xu_iterator_item(iterator, head + parent)), item) < 0); parent = (hole - 1) >> 1)
    {
        // move item: parent => hole
//      xu_iterator_copy(iterator, head + parent, item);
        xu_iterator_copy(iterator, head + hole, parent_item);

        // move node: hole => parent
        hole = parent;
    }

    // copy item
    xu_iterator_copy(iterator, head + hole, item);
}
#endif

/*! adjust heap
 *
 * <pre>
 * init:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                           (hole)                       10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8(larger)      7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         4     1(tail - 1)
 *
 * after:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                      (hole)          7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2 (larger)4     1(tail - 1)
 *
 * after:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       4              7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2      (hole)   1(tail - 1)
 *
 * </pre>
 */
static __xu_inline__ xu_void_t xu_heap_adjust(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t hole,
                                              xu_size_t tail, xu_cpointer_t item, xu_iterator_comp_t comp)
{
    // the comparer
    if (!comp) comp = xu_iterator_comp;

#if 0
    // save top position
    xu_size_t top = hole;

    // 2 * hole + 2: the right child node of hole
    xu_size_t child = (hole << 1) + 2;
    for (; child < tail; child = (child << 1) + 2)
    {
        // the larger child node
        if (comp(iterator, xu_iterator_item(iterator, head + child), xu_iterator_item(iterator, head + child - 1)) < 0) child--;

        // the larger child node => hole
        xu_iterator_copy(iterator, head + hole, xu_iterator_item(iterator, head + child));

        // move the hole down to it's larger child node
        hole = child;
    }

    // no right child node?
    if (child == tail)
    {
        // the last child => hole
        xu_iterator_copy(iterator, head + hole, xu_iterator_item(iterator, head + tail - 1));

        // move hole down to tail
        hole = tail - 1;
    }

    // push item into the hole
    xu_heap_push(iterator, head, hole, top, item, comp);
#else

    // walk, 2 * hole + 1: the left child node of hole
    xu_size_t     child        = (hole << 1) + 1;
    xu_cpointer_t child_item   = xu_null;
    xu_cpointer_t child_item_r = xu_null;
    for (; child < tail; child = (child << 1) + 1)
    {
        // the larger child node
        child_item = xu_iterator_item(iterator, head + child);
        if (child + 1 < tail &&
            comp(iterator, child_item, (child_item_r = xu_iterator_item(iterator, head + child + 1))) < 0)
        {
            child++;
            child_item = child_item_r;
        }

        // end?
        if (comp(iterator, child_item, item) < 0) break;

        // the larger child node => hole
        xu_iterator_copy(iterator, head + hole, child_item);

        // move the hole down to it's larger child node
        hole = child;
    }

    // copy item
    xu_iterator_copy(iterator, head + hole, item);

#endif
}
/*!make heap
 *
 * <pre>
 * heap:    16      14      10      8       7       9       3       2       4       1
 *
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              14                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8       (tail / 2 - 1)7    9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         4     1(tail - 1)
 * </pre>
 */
static __xu_inline__ xu_void_t xu_heap_make(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail,
                                            xu_iterator_comp_t comp)
{
    // get flag
    xu_size_t step = xu_iterator_step(iterator);
    xu_size_t flag = xu_iterator_flag(iterator);
    if (!flag && step > sizeof(xu_pointer_t)) flag |= XU_ITERATOR_FLAG_ITEM_REF;

    // init
    xu_pointer_t temp = flag & XU_ITERATOR_FLAG_ITEM_REF ? xu_malloc(step) : xu_null;
    xu_assert_and_check_return(step <= sizeof(xu_pointer_t) || temp);

    // make
    xu_size_t hole;
    xu_size_t bottom = tail - head;
    for (hole = (bottom >> 1); hole > 0;)
    {
        --hole;

        // save hole
        if (flag & XU_ITERATOR_FLAG_ITEM_REF)
            xu_memcpy(temp, xu_iterator_item(iterator, head + hole), step);
        else
            temp = xu_iterator_item(iterator, head + hole);

        // reheap top half, bottom to top
        xu_heap_adjust(iterator, head, hole, bottom, temp, comp);
    }

    // free temp item
    if (temp && (flag & XU_ITERATOR_FLAG_ITEM_REF)) xu_free(temp);

    // check
    xu_assert(xu_heap_check(iterator, head, tail, comp));
}
/*!pop the top of heap to last and reheap
 *
 * <pre>
 *                                          16(head)
 *                               ----------------|--------
 *                              |                |        |
 *                              14               |        10
 *                        --------------         |   -------------
 *                       |              |        |  |             |
 *                       8              7        |  9             3
 *                   ---------       ----        |
 *                  |         |     |            |
 *                  2         4     1(last)<-----
 *                                (hole)
 * </pre>
 */
static __xu_inline__ xu_void_t xu_heap_pop0(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail,
                                            xu_cpointer_t item, xu_iterator_comp_t comp)
{
    // top => last
    xu_iterator_copy(iterator, tail - 1, xu_iterator_item(iterator, head));

    // reheap it
    xu_heap_adjust(iterator, head, 0, tail - head - 1, item, comp);

    // check
    //  xu_assert(xu_heap_check(iterator, head, tail - head - 1, comp));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/*!the heap sort
 *
 * <pre>
 * init:
 *
 *                                           16(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       14             7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         8     1(last - 1)
 *
 * make_heap:
 *
 *                                           16(head)
 *                               -------------------------
 *                              |                         |
 *                              14                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8              7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         4     1(last - 1)
 * pop_heap:
 *
 *                                          16(head)--------------------------
 *                               -------------------------                     |
 *                              |                         |                    |
 *                              4                         10                   |
 *                        --------------             -------------             |
 *                       |              |           |             |            |
 *                       14             7           9             3            |
 *                   ---------       ----                                      |
 *                  |         |     |                                          |
 *                  2         8     1(last - 1) <------------------------------
 *
 *                                          (hole)(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         10
 *                        --------------             -------------
 *                       |              |           |             |          (val = 1)
 *                       14             7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         8     16(last - 1)
 *
 * adjust_heap:
 *                                          14(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |           (val = 1)
 *                       4              7           9             3
 *                   ---------
 *                  |         |
 *                  2      (hole)(last - 1)   16
 *
 *
 * push_heap:
 *                                          14(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |           (val = 1)
 *                       4              7           9             3              |
 *                   ---------                                                   |
 *                  |         | /-----------------------------------------------
 *                  2      (hole)(last - 1)   16
 *
 *                                          14(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |           (val = 1)
 *                       4              7           9             3
 *                   ---------
 *                  |         |
 *                  2       1(last - 1)   16
 *
 * pop_heap adjust_heap push_heap ...
 *
 * final_heap:
 *                                           1(head)
 *
 *
 *                              2                         3
 *
 *
 *                       4              7           8             9
 *
 *
 *                  10       14      16
 *
 * result: 1 2 3 4 7 8 9 10 14 16
 * </pre>
 */
xu_void_t xu_heap_sort(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_iterator_comp_t comp)
{
    // check
    xu_assert_and_check_return(iterator && (xu_iterator_mode(iterator) & XU_ITERATOR_MODE_RACCESS));
    xu_check_return(head != tail);

    // make
    xu_heap_make(iterator, head, tail, comp);

    // get flag
    xu_size_t step = xu_iterator_step(iterator);
    xu_size_t flag = xu_iterator_flag(iterator);
    if (!flag && step > sizeof(xu_pointer_t)) flag |= XU_ITERATOR_FLAG_ITEM_REF;

    // init last item
    xu_pointer_t last = flag & XU_ITERATOR_FLAG_ITEM_REF ? xu_malloc(step) : xu_null;
    xu_assert_and_check_return(step <= sizeof(xu_pointer_t) || last);

    // pop0 ...
    for (; tail > head + 1; tail--)
    {
        // save last
        if (flag & XU_ITERATOR_FLAG_ITEM_REF)
            xu_memcpy(last, xu_iterator_item(iterator, tail - 1), step);
        else
            last = xu_iterator_item(iterator, tail - 1);

        // pop0
        xu_heap_pop0(iterator, head, tail, last, comp);
    }

    // free
    if (last && (flag & XU_ITERATOR_FLAG_ITEM_REF)) xu_free(last);
}
xu_void_t xu_heap_sort_all(xu_iterator_ref_t iterator, xu_iterator_comp_t comp)
{
    xu_heap_sort(iterator, xu_iterator_head(iterator), xu_iterator_tail(iterator), comp);
}
