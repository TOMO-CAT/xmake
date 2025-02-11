#pragma once

#include "xutil/prefix.h"

#include "xutil/container/element.h"
#include "xutil/container/iterator.h"

/* *******************************************************
 * types
 */

/*! the circle queue ref type
 *
 * <pre>
 * queue: |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||------|
 *       head                                                           last    tail
 *
 * queue: ||||||||||||||-----|--------------------------||||||||||||||||||||||||||
 *                   last  tail                       head
 *
 * performance:
 *
 * put: O(1)
 * pop: O(1)
 *
 * iterator:
 *
 * next: fast
 * prev: fast
 *
 * </pre>
 *
 * @note the index of the same item is mutable
 *
 */
typedef xu_iterator_ref_t xu_circle_queue_ref_t;

/* *******************************************************
 * interfaces
 */

/*! init queue
 *
 * @param maxn          the item maxn, using the default maxn if be zero
 * @param element       the element
 *
 * @return              the queue
 */
xu_circle_queue_ref_t xu_circle_queue_init(xu_size_t maxn, xu_element_t element);

/*! exit queue
 *
 * @param queue         the queue
 */
xu_void_t xu_circle_queue_exit(xu_circle_queue_ref_t queue);

/*! the queue head item
 *
 * @param queue         the queue
 *
 * @return              the head item
 */
xu_pointer_t xu_circle_queue_head(xu_circle_queue_ref_t queue);

/*! the queue last item
 *
 * @param queue         the queue
 *
 * @return              the last item
 */
xu_pointer_t xu_circle_queue_last(xu_circle_queue_ref_t queue);

/*! clear the queue
 *
 * @param queue         the queue
 */
xu_void_t xu_circle_queue_clear(xu_circle_queue_ref_t queue);

/*! put the queue item
 *
 * @param queue         the queue
 * @param data          the item data
 */
xu_void_t xu_circle_queue_put(xu_circle_queue_ref_t queue, xu_cpointer_t data);

/*! pop the queue item
 *
 * @param queue         the queue
 */
xu_void_t xu_circle_queue_pop(xu_circle_queue_ref_t queue);

/*! get the queue item
 *
 * @param queue         the queue
 *
 * @return              the queue item
 */
xu_pointer_t xu_circle_queue_get(xu_circle_queue_ref_t queue);

/*! the queue size
 *
 * @param queue         the queue
 *
 * @return              the queue size
 */
xu_size_t xu_circle_queue_size(xu_circle_queue_ref_t queue);

/*! the queue maxn
 *
 * @param queue         the queue
 *
 * @return              the queue maxn
 */
xu_size_t xu_circle_queue_maxn(xu_circle_queue_ref_t queue);

/*! the queue full?
 *
 * @param queue         the queue
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_circle_queue_full(xu_circle_queue_ref_t queue);

/*! the queue null?
 *
 * @param queue         the queue
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_circle_queue_null(xu_circle_queue_ref_t queue);

#ifdef __xu_debug__
/*! dump queue
 *
 * @param queue         the queue
 */
xu_void_t xu_circle_queue_dump(xu_circle_queue_ref_t circle_queue);
#endif
