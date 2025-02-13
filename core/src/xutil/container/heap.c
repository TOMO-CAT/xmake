/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "heap"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xutil/container/heap.h"
#include "xutil/algorithm/for.h"
#include "xutil/libc/libc.h"
#include "xutil/memory/memory.h"
#include "xutil/stream/stream.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * macros
 */

// the self grow
#ifdef __xu_small__
#    define XU_HEAP_GROW (128)
#else
#    define XU_HEAP_GROW (256)
#endif

// the self maxn
#ifdef __xu_small__
#    define XU_HEAP_MAXN (1 << 16)
#else
#    define XU_HEAP_MAXN (1 << 30)
#endif

// enable check
#ifdef __xu_debug__
#    define XU_HEAP_CHECK_ENABLE (0)
#else
#    define XU_HEAP_CHECK_ENABLE (0)
#endif

/* *******************************************************
 * types
 */

// the self type
typedef struct __xu_heap_t
{
    // the itor
    xu_iterator_t itor;

    // the data
    xu_byte_t* data;

    // the size
    xu_size_t size;

    // the maxn
    xu_size_t maxn;

    // the grow
    xu_size_t grow;

    // the element
    xu_element_t element;

} xu_heap_t;

/* *******************************************************
 * private implementation
 */
#if XU_HEAP_CHECK_ENABLE
static xu_void_t xu_heap_check(xu_heap_t* heap)
{
    // init
    xu_byte_t* data   = heap->data;
    xu_size_t  tail   = heap->size;
    xu_size_t  step   = heap->element.size;
    xu_size_t  parent = 0;

    // done
    for (; parent < tail; parent++)
    {
        // the left child node
        xu_size_t lchild = (parent << 1) + 1;
        xu_check_break(lchild < tail);

        // the parent data
        xu_pointer_t parent_data = heap->element.data(&heap->element, data + parent * step);

        // check?
        if (heap->element.comp(&heap->element, heap->element.data(&heap->element, data + lchild * step), parent_data) <
            0)
        {
            // dump self
            xu_heap_dump((xu_heap_ref_t)heap);

            // abort
            xu_assertf(0, "lchild[%lu]: invalid, parent: %lu, tail: %lu", lchild, parent, tail);
        }

        // the right child node
        xu_size_t rchild = lchild + 1;
        xu_check_break(rchild < tail);

        // check?
        if (heap->element.comp(&heap->element, heap->element.data(&heap->element, data + rchild * step), parent_data) <
            0)
        {
            // dump self
            xu_heap_dump((xu_heap_ref_t)heap);

            // abort
            xu_assertf(0, "rchild[%lu]: invalid, parent: %lu, tail: %lu", rchild, parent, tail);
        }
    }
}
#endif
/*! shift up the self
 *
 * <pre>
 *
 * before:
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       6(parent)      9           7             8
 *                   ---------
 *                  |         |
 *                  10      5(hole) <------ data
 * after:
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *         data -------> 5(hole)        9           7             8
 *                   ---------
 *                  |         |
 *                  10        6
 * </pre>
 */
static xu_pointer_t xu_heap_shift_up(xu_heap_t* heap, xu_size_t hole, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return_val(heap && heap->data, xu_null);

    // the element function
    xu_element_comp_func_t func_comp = heap->element.comp;
    xu_element_data_func_t func_data = heap->element.data;
    xu_assert(func_comp && func_data);

    // (hole - 1) / 2: the parent node of the hole
    xu_size_t  parent = 0;
    xu_byte_t* head   = heap->data;
    xu_size_t  step   = heap->element.size;
    switch (step)
    {
    case sizeof(xu_size_t):
    {
        for (parent = (hole - 1) >> 1;
             hole && (func_comp(&heap->element, func_data(&heap->element, head + parent * step), data) > 0);
             parent = (hole - 1) >> 1)
        {
            // move item: parent => hole
            *((xu_size_t*)(head + hole * step)) = *((xu_size_t*)(head + parent * step));

            // move node: hole => parent
            hole = parent;
        }
    }
    break;
    default:
        for (parent = (hole - 1) >> 1;
             hole && (func_comp(&heap->element, func_data(&heap->element, head + parent * step), data) > 0);
             parent = (hole - 1) >> 1)
        {
            // move item: parent => hole
            xu_memcpy(head + hole * step, head + parent * step, step);

            // move node: hole => parent
            hole = parent;
        }
        break;
    }

    // ok?
    return head + hole * step;
}
/*! shift down the self
 *
 * <pre>
 *
 * before:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                           (hole)                       2
 *                        --------------             -------------
 *                       |              |           |             |
 *            lchild --> 6(smaller)     7           7             8
 *                   ---------     ------
 *                  |         |   |
 *                 11        16  10
 *
 *
 * move hole:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              6                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                     (hole)           7           7             8
 *                   ---------      -----
 *                  |         |    |
 *      lchild --> 11(smaller)16  10
 *
 * 11 >= data: 9? break it
 *
 * move data to hole:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              6                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *    data ------------> 9              7           7             8
 *                   ---------       ---
 *                  |         |     |
 *                 11        16    10
 *
 * </pre>
 */
static xu_pointer_t xu_heap_shift_down(xu_heap_t* heap, xu_size_t hole, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return_val(heap && heap->data, xu_null);

    // init element
    xu_element_comp_func_t func_comp = heap->element.comp;
    xu_element_data_func_t func_data = heap->element.data;
    xu_assert(func_comp && func_data);

    // 2 * hole + 1: the left child node of hole
    xu_size_t    step        = heap->element.size;
    xu_byte_t*   head        = heap->data;
    xu_byte_t*   tail        = head + heap->size * step;
    xu_byte_t*   phole       = head + hole * step;
    xu_byte_t*   lchild      = head + ((hole << 1) + 1) * step;
    xu_pointer_t data_lchild = xu_null;
    xu_pointer_t data_rchild = xu_null;
    switch (step)
    {
    case sizeof(xu_size_t):
    {
        for (; lchild < tail; lchild = head + (((lchild - head) << 1) + step))
        {
            // the smaller child node
            data_lchild = func_data(&heap->element, lchild);
            if (lchild + step < tail &&
                func_comp(&heap->element, data_lchild, (data_rchild = func_data(&heap->element, lchild + step))) > 0)
            {
                lchild += step;
                data_lchild = data_rchild;
            }

            // end?
            if (func_comp(&heap->element, data_lchild, data) >= 0) break;

            // the smaller child node => hole
            *((xu_size_t*)phole) = *((xu_size_t*)lchild);

            // move the hole down to it's smaller child node
            phole = lchild;
        }
    }
    break;
    default:
    {
        for (; lchild < tail; lchild = head + (((lchild - head) << 1) + step))
        {
            // the smaller child node
            data_lchild = func_data(&heap->element, lchild);
            if (lchild + step < tail &&
                func_comp(&heap->element, data_lchild, (data_rchild = func_data(&heap->element, lchild + step))) > 0)
            {
                lchild += step;
                data_lchild = data_rchild;
            }

            // end?
            if (func_comp(&heap->element, data_lchild, data) >= 0) break;

            // the smaller child node => hole
            xu_memcpy(phole, lchild, step);

            // move the hole down to it's smaller child node
            phole = lchild;
        }
    }
    break;
    }

    // ok?
    return phole;
}
static xu_size_t xu_heap_itor_size(xu_iterator_ref_t iterator)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return_val(heap, 0);

    // size
    return heap->size;
}
static xu_size_t xu_heap_itor_head(xu_iterator_ref_t iterator)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return_val(heap, 0);

    // head
    return 0;
}
static xu_size_t xu_heap_itor_last(xu_iterator_ref_t iterator)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return_val(heap, 0);

    // last
    return heap->size ? heap->size - 1 : 0;
}
static xu_size_t xu_heap_itor_tail(xu_iterator_ref_t iterator)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return_val(heap, 0);

    // tail
    return heap->size;
}
static xu_size_t xu_heap_itor_next(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return_val(heap, 0);
    xu_assert_and_check_return_val(itor < heap->size, heap->size);

    // next
    return itor + 1;
}
static xu_size_t xu_heap_itor_prev(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return_val(heap, 0);
    xu_assert_and_check_return_val(itor && itor < heap->size, 0);

    // prev
    return itor - 1;
}
static xu_pointer_t xu_heap_itor_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return_val(heap && itor < heap->size, xu_null);

    // data
    return heap->element.data(&heap->element, heap->data + itor * iterator->step);
}
static xu_void_t xu_heap_itor_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return(heap);

    // copy
    heap->element.copy(&heap->element, heap->data + itor * iterator->step, item);
}
static xu_long_t xu_heap_itor_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return_val(heap && heap->element.comp, 0);

    // comp
    return heap->element.comp(&heap->element, litem, ritem);
}
static xu_void_t xu_heap_itor_remove(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)iterator;
    xu_assert_and_check_return(heap && heap->data && heap->size && itor < heap->size);

    // check the element function
    xu_assert(heap->element.comp && heap->element.data);

    // the step
    xu_size_t step = heap->element.size;
    xu_assert(step);

    // free the item first
    if (heap->element.free) heap->element.free(&heap->element, heap->data + itor * step);

    // the removed item is not the last item?
    if (itor != heap->size - 1)
    {
        // the last and parent
        xu_pointer_t last   = heap->data + (heap->size - 1) * step;
        xu_pointer_t parent = heap->data + ((itor - 1) >> 1) * step;

        // the last and parent data
        xu_pointer_t data_last   = heap->element.data(&heap->element, last);
        xu_pointer_t data_parent = heap->element.data(&heap->element, parent);

        /* we might need to shift it upward if it is less than its parent,
         * or downward if it is greater than one or both its children.
         *
         * since the children are known to be less than the parent,
         * it can't need to shift both up and down.
         */
        xu_pointer_t hole = xu_null;
        if (itor && heap->element.comp(&heap->element, data_parent, data_last) > 0)
        {
            // shift up the self from the given hole
            hole = xu_heap_shift_up(heap, itor, data_last);
        }
        // shift down the self from the given hole
        else
            hole = xu_heap_shift_down(heap, itor, data_last);
        xu_assert(hole);

        // copy the last data to the hole
        if (hole != last) xu_memcpy(hole, last, step);
    }

    // size--
    heap->size--;

    // check
#if XU_HEAP_CHECK_ENABLE
    xu_heap_check(heap);
#endif
}

/* *******************************************************
 * implementation
 */
xu_heap_ref_t xu_heap_init(xu_size_t grow, xu_element_t element)
{
    // check
    xu_assert_and_check_return_val(element.size && element.data && element.dupl && element.repl, xu_null);

    // done
    xu_bool_t  ok   = xu_false;
    xu_heap_t* heap = xu_null;
    do
    {
        // using the default grow
        if (!grow) grow = XU_HEAP_GROW;

        // make heap
        heap = xu_malloc0_type(xu_heap_t);
        xu_assert_and_check_break(heap);

        // init heap
        heap->size    = 0;
        heap->grow    = grow;
        heap->maxn    = grow;
        heap->element = element;
        xu_assert_and_check_break(heap->maxn < XU_HEAP_MAXN);

        // init operation
        static xu_iterator_op_t op = {xu_heap_itor_size,
                                      xu_heap_itor_head,
                                      xu_heap_itor_last,
                                      xu_heap_itor_tail,
                                      xu_heap_itor_prev,
                                      xu_heap_itor_next,
                                      xu_heap_itor_item,
                                      xu_heap_itor_comp,
                                      xu_heap_itor_copy,
                                      xu_heap_itor_remove,
                                      xu_null};

        // init iterator
        heap->itor.priv = xu_null;
        heap->itor.step = element.size;
        heap->itor.mode =
            XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE | XU_ITERATOR_MODE_RACCESS | XU_ITERATOR_MODE_MUTABLE;
        heap->itor.op = &op;
        if (element.type == XU_ELEMENT_TYPE_MEM) heap->itor.flag = XU_ITERATOR_FLAG_ITEM_REF;

        // make data
        heap->data = (xu_byte_t*)xu_nalloc0(heap->maxn, element.size);
        xu_assert_and_check_break(heap->data);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (heap) xu_heap_exit((xu_heap_ref_t)heap);
        heap = xu_null;
    }

    // ok?
    return (xu_heap_ref_t)heap;
}
xu_void_t xu_heap_exit(xu_heap_ref_t self)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)self;
    xu_assert_and_check_return(heap);

    // clear data
    xu_heap_clear(self);

    // free data
    if (heap->data) xu_free(heap->data);
    heap->data = xu_null;

    // free it
    xu_free(heap);
}
xu_void_t xu_heap_clear(xu_heap_ref_t self)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)self;
    xu_assert_and_check_return(heap);

    // free data
    if (heap->element.nfree) heap->element.nfree(&heap->element, heap->data, heap->size);

    // reset size
    heap->size = 0;
}
xu_size_t xu_heap_size(xu_heap_ref_t self)
{
    // check
    xu_heap_t const* heap = (xu_heap_t const*)self;
    xu_assert_and_check_return_val(heap, 0);

    // size
    return heap->size;
}
xu_size_t xu_heap_maxn(xu_heap_ref_t self)
{
    // check
    xu_heap_t const* heap = (xu_heap_t const*)self;
    xu_assert_and_check_return_val(heap, 0);

    // maxn
    return heap->maxn;
}
xu_pointer_t xu_heap_top(xu_heap_ref_t self) { return xu_iterator_item(self, xu_iterator_head(self)); }
xu_void_t    xu_heap_put(xu_heap_ref_t self, xu_cpointer_t data)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)self;
    xu_assert_and_check_return(heap && heap->element.dupl && heap->data);

    // no enough? grow it
    if (heap->size == heap->maxn)
    {
        // the maxn
        xu_size_t maxn = xu_align4(heap->maxn + heap->grow);
        xu_assert_and_check_return(maxn < XU_HEAP_MAXN);

        // realloc data
        heap->data = (xu_byte_t*)xu_ralloc(heap->data, maxn * heap->element.size);
        xu_assert_and_check_return(heap->data);

        // must be align by 4-bytes
        xu_assert_and_check_return(!(((xu_size_t)(heap->data)) & 3));

        // clear the grow data
        xu_memset(heap->data + heap->size * heap->element.size, 0, (maxn - heap->maxn) * heap->element.size);

        // save maxn
        heap->maxn = maxn;
    }

    // check
    xu_assert_and_check_return(heap->size < heap->maxn);

    // shift up the self from the tail hole
    xu_pointer_t hole = xu_heap_shift_up(heap, heap->size, data);
    xu_assert(hole);

    // save data to the hole
    if (hole) heap->element.dupl(&heap->element, hole, data);

    // update the size
    heap->size++;

    // check
#if XU_HEAP_CHECK_ENABLE
    xu_heap_check(heap);
#endif
}
xu_void_t xu_heap_pop(xu_heap_ref_t self)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)self;
    xu_assert_and_check_return(heap && heap->data && heap->size);

    // free the top item first
    if (heap->element.free) heap->element.free(&heap->element, heap->data);

    // the last item is not in top
    if (heap->size > 1)
    {
        // check the element function
        xu_assert(heap->element.data);

        // the step
        xu_size_t step = heap->element.size;
        xu_assert(step);

        // the last
        xu_pointer_t last = heap->data + (heap->size - 1) * step;

        // shift down the self from the top hole
        xu_pointer_t hole = xu_heap_shift_down(heap, 0, heap->element.data(&heap->element, last));
        xu_assert(hole);

        // copy the last data to the hole
        if (hole != last) xu_memcpy(hole, last, step);
    }

    // update the size
    heap->size--;

    // check
#if XU_HEAP_CHECK_ENABLE
    xu_heap_check(heap);
#endif
}
xu_void_t xu_heap_remove(xu_heap_ref_t self, xu_size_t itor) { xu_heap_itor_remove(self, itor); }
#ifdef __xu_debug__
xu_void_t xu_heap_dump(xu_heap_ref_t self)
{
    // check
    xu_heap_t* heap = (xu_heap_t*)self;
    xu_assert_and_check_return(heap);

    // trace
    xu_trace_i("self: size: %lu", xu_heap_size(self));

    // done
    xu_char_t cstr[4096];
    xu_for_all(xu_pointer_t, data, self)
    {
        // trace
        if (heap->element.cstr)
        {
#    if XU_HEAP_CHECK_ENABLE
            xu_trace_i("    [%lu]: %s", data_itor, heap->element.cstr(&heap->element, data, cstr, sizeof(cstr)));
#    else
            xu_trace_i("    %s", heap->element.cstr(&heap->element, data, cstr, sizeof(cstr)));
#    endif
        }
        else
        {
            xu_trace_i("    %p", data);
        }
    }
}
#endif
