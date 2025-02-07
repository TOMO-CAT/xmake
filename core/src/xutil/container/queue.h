#pragma once

#include "xutil/container/element.h"
#include "xutil/container/iterator.h"
#include "xutil/container/prefix.h"
#include "xutil/container/single_list.h"

/* *******************************************************
 * types
 */

/*! the queue ref type
 *
 * <pre>
 * queue: |-----| => |-----| => ...                                 => |-----| => tail
 *         head                                                          last
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
 */
typedef xu_single_list_ref_t xu_queue_ref_t;

/* *******************************************************
 * interfaces
 */

/*! init queue
 *
 * @param grow      the grow size, using the default grow size if be zero
 * @param element   the element
 *
 * @return          the queue
 */
xu_queue_ref_t xu_queue_init(xu_size_t grow, xu_element_t element);

/*! exit queue
 *
 * @param queue     the queue
 */
xu_void_t xu_queue_exit(xu_queue_ref_t queue);

/*! the queue head item
 *
 * @param queue     the queue
 *
 * @return          the head item
 */
xu_pointer_t xu_queue_head(xu_queue_ref_t queue);

/*! the queue last item
 *
 * @param queue     the queue
 *
 * @return          the last item
 */
xu_pointer_t xu_queue_last(xu_queue_ref_t queue);

/*! clear the queue
 *
 * @param queue     the queue
 */
xu_void_t xu_queue_clear(xu_queue_ref_t queue);

/*! put the queue item
 *
 * @param queue     the queue
 * @param data      the item data
 */
xu_void_t xu_queue_put(xu_queue_ref_t queue, xu_cpointer_t data);

/*! pop the queue item
 *
 * @param queue     the queue
 */
xu_void_t xu_queue_pop(xu_queue_ref_t queue);

/*! get the queue item
 *
 * @param queue     the queue
 *
 * @return          the queue item
 */
xu_pointer_t xu_queue_get(xu_queue_ref_t queue);

/*! the queue size
 *
 * @param queue     the queue
 *
 * @return          the queue size
 */
xu_size_t xu_queue_size(xu_queue_ref_t queue);

/*! the queue maxn
 *
 * @param queue     the queue
 *
 * @return          the queue maxn
 */
xu_size_t xu_queue_maxn(xu_queue_ref_t queue);

/*! the queue full?
 *
 * @param queue     the queue
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_queue_full(xu_queue_ref_t queue);

/*! the queue null?
 *
 * @param queue     the queue
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_queue_null(xu_queue_ref_t queue);

#ifdef __xu_debug__
/*! dump queue
 *
 * @param queue     the queue
 */
xu_void_t xu_queue_dump(xu_queue_ref_t queue);
#endif
