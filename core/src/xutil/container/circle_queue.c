#include "xutil/container/circle_queue.h"
#include "xutil/algorithm/for.h"
#include "xutil/libc/libc.h"
#include "xutil/memory/memory.h"
#include "xutil/stream/stream.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * macros
 */
#ifdef __xu_small__
#    define XU_CIRCLE_QUEUE_SIZE_DEFAULT (256)
#else
#    define XU_CIRCLE_QUEUE_SIZE_DEFAULT (65536)
#endif

/* *******************************************************
 * types
 */

// the circle queue type
typedef struct __xu_circle_queue_t
{
    // the itor
    xu_iterator_t itor;

    // the data
    xu_byte_t* data;

    // the head
    xu_size_t head;

    // the tail
    xu_size_t tail;

    // the maxn
    xu_size_t maxn;

    // the size
    xu_size_t size;

    // the element
    xu_element_t element;

} xu_circle_queue_t;

/* *******************************************************
 * private implementation
 */
static xu_size_t xu_circle_queue_itor_size(xu_iterator_ref_t iterator)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert_and_check_return_val(queue, 0);

    // the size
    return queue->size;
}
static xu_size_t xu_circle_queue_itor_head(xu_iterator_ref_t iterator)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert_and_check_return_val(queue, 0);

    // head
    return queue->head;
}
static xu_size_t xu_circle_queue_itor_last(xu_iterator_ref_t iterator)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert_and_check_return_val(queue, 0);

    // last
    return (queue->tail + queue->maxn - 1) % queue->maxn;
}
static xu_size_t xu_circle_queue_itor_tail(xu_iterator_ref_t iterator)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert_and_check_return_val(queue, 0);

    // tail
    return queue->tail;
}
static xu_size_t xu_circle_queue_itor_next(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert_and_check_return_val(queue, 0);

    // next
    return (itor + 1) % queue->maxn;
}
static xu_size_t xu_circle_queue_itor_prev(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert_and_check_return_val(queue, 0);

    // prev
    return (itor + queue->maxn - 1) % queue->maxn;
}
static xu_pointer_t xu_circle_queue_itor_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert_and_check_return_val(queue && itor < queue->maxn, xu_null);

    // item
    return queue->element.data(&queue->element, queue->data + itor * iterator->step);
}
static xu_void_t xu_circle_queue_itor_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert(queue);

    // copy
    queue->element.copy(&queue->element, queue->data + itor * iterator->step, item);
}
static xu_long_t xu_circle_queue_itor_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)iterator;
    xu_assert_and_check_return_val(queue && queue->element.comp, 0);

    // comp
    return queue->element.comp(&queue->element, litem, ritem);
}

/* *******************************************************
 * implementation
 */
xu_circle_queue_ref_t xu_circle_queue_init(xu_size_t maxn, xu_element_t element)
{
    // check
    xu_assert_and_check_return_val(element.size && element.dupl && element.data, xu_null);

    // done
    xu_bool_t          ok    = xu_false;
    xu_circle_queue_t* queue = xu_null;
    do
    {
        // make queue
        queue = xu_malloc0_type(xu_circle_queue_t);
        xu_assert_and_check_break(queue);

        // using the default maxn
        if (!maxn) maxn = XU_CIRCLE_QUEUE_SIZE_DEFAULT;

        // init queue, + tail
        queue->maxn    = maxn + 1;
        queue->element = element;

        // init operation
        static xu_iterator_op_t op = {xu_circle_queue_itor_size,
                                      xu_circle_queue_itor_head,
                                      xu_circle_queue_itor_last,
                                      xu_circle_queue_itor_tail,
                                      xu_circle_queue_itor_prev,
                                      xu_circle_queue_itor_next,
                                      xu_circle_queue_itor_item,
                                      xu_circle_queue_itor_comp,
                                      xu_circle_queue_itor_copy,
                                      xu_null,
                                      xu_null};

        // init iterator
        queue->itor.priv = xu_null;
        queue->itor.step = element.size;
        queue->itor.mode = XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE | XU_ITERATOR_MODE_MUTABLE;
        queue->itor.op   = &op;
        if (element.type == XU_ELEMENT_TYPE_MEM) queue->itor.flag = XU_ITERATOR_FLAG_ITEM_REF;

        // make data
        queue->data = (xu_byte_t*)xu_nalloc0(queue->maxn, element.size);
        xu_assert_and_check_break(queue->data);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (queue) xu_circle_queue_exit((xu_circle_queue_ref_t)queue);
        queue = xu_null;
    }

    // ok?
    return (xu_circle_queue_ref_t)queue;
}
xu_void_t xu_circle_queue_exit(xu_circle_queue_ref_t self)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return(queue);

    // clear data
    xu_circle_queue_clear(self);

    // free data
    if (queue->data) xu_free(queue->data);

    // free it
    xu_free(queue);
}
xu_void_t xu_circle_queue_clear(xu_circle_queue_ref_t self)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return(queue);

    // clear it
    while (!xu_circle_queue_null(self))
        xu_circle_queue_pop(self);
    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;
}
xu_void_t xu_circle_queue_put(xu_circle_queue_ref_t self, xu_cpointer_t data)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return(queue && queue->size < queue->maxn);

    // put it
    queue->element.dupl(&queue->element, queue->data + queue->tail * queue->element.size, data);
    queue->tail = (queue->tail + 1) % queue->maxn;
    queue->size++;
}
xu_void_t xu_circle_queue_pop(xu_circle_queue_ref_t self)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return(queue && queue->size);

    // pop it
    if (queue->element.free) queue->element.free(&queue->element, queue->data + queue->head * queue->element.size);
    queue->head = (queue->head + 1) % queue->maxn;
    queue->size--;
}
xu_pointer_t xu_circle_queue_get(xu_circle_queue_ref_t self)
{
    // get the head item
    return xu_circle_queue_head(self);
}
xu_pointer_t xu_circle_queue_head(xu_circle_queue_ref_t self)
{
    // the head item
    return xu_iterator_item((xu_iterator_ref_t)self, xu_iterator_head((xu_iterator_ref_t)self));
}
xu_pointer_t xu_circle_queue_last(xu_circle_queue_ref_t self)
{
    // the last item
    return xu_iterator_item((xu_iterator_ref_t)self, xu_iterator_last((xu_iterator_ref_t)self));
}
xu_size_t xu_circle_queue_size(xu_circle_queue_ref_t self)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return_val(queue, 0);

    // the size
    return queue->size;
}
xu_size_t xu_circle_queue_maxn(xu_circle_queue_ref_t self)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return_val(queue, 0);

    // the maxn
    return queue->maxn;
}
xu_bool_t xu_circle_queue_full(xu_circle_queue_ref_t self)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return_val(queue, xu_true);

    // is full?
    return (queue->size + 1) == queue->maxn;
}
xu_bool_t xu_circle_queue_null(xu_circle_queue_ref_t self)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return_val(queue, xu_true);

    // is null?
    return !queue->size;
}
#ifdef __xu_debug__
xu_void_t xu_circle_queue_dump(xu_circle_queue_ref_t self)
{
    // check
    xu_circle_queue_t* queue = (xu_circle_queue_t*)self;
    xu_assert_and_check_return(queue);

    // trace
    xu_trace_i("self: size: %lu", xu_circle_queue_size(self));

    // done
    xu_char_t cstr[4096];
    xu_for_all(xu_pointer_t, data, self)
    {
        // trace
        if (queue->element.cstr)
        {
            xu_trace_i("    %s", queue->element.cstr(&queue->element, data, cstr, sizeof(cstr)));
        }
        else
        {
            xu_trace_i("    %p", data);
        }
    }
}
#endif
