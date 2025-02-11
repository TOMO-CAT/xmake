#pragma once

#include "xutil/prefix.h"

#include "xutil/container/element.h"
#include "xutil/container/iterator.h"

/* *******************************************************
 * types
 */

/*! the head ref type
 *
 * <pre>
 * heap:    1      4      2      6       9       7       8       10       14       16
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       6       (last / 2 - 1)9    7             8
 *                   ---------       ----
 *                  |         |     |
 *                  10        14    16(last - 1)
 *
 * performance:
 *
 * put: O(lgn)
 * pop: O(1)
 * top: O(1)
 * del: O(lgn) + find: O(n)
 *
 * iterator:
 *
 * next: fast
 * prev: fast
 *
 * </pre>
 *
 * @note the itor of the same item is mutable
 */
typedef xu_iterator_ref_t xu_heap_ref_t;

/* *******************************************************
 * interfaces
 */

/*! init heap, default: minheap
 *
 * @param grow      the item grow, using the default grow if be zero
 * @param element   the element
 *
 * @return          the heap
 */
xu_heap_ref_t xu_heap_init(xu_size_t grow, xu_element_t element);

/*! exit heap
 *
 * @param heap      the heap
 */
xu_void_t xu_heap_exit(xu_heap_ref_t heap);

/*! clear the heap
 *
 * @param heap      the heap
 */
xu_void_t xu_heap_clear(xu_heap_ref_t heap);

/*! the heap size
 *
 * @param heap      the heap
 *
 * @return          the heap size
 */
xu_size_t xu_heap_size(xu_heap_ref_t heap);

/*! the heap maxn
 *
 * @param heap      the heap
 *
 * @return          the heap maxn
 */
xu_size_t xu_heap_maxn(xu_heap_ref_t heap);

/*! the heap top item
 *
 * @param heap      the heap
 *
 * @return          the heap top item
 */
xu_pointer_t xu_heap_top(xu_heap_ref_t heap);

/*! put the heap item
 *
 * @param heap      the heap
 * @param data      the item data
 */
xu_void_t xu_heap_put(xu_heap_ref_t heap, xu_cpointer_t data);

/*! pop the heap item
 *
 * @param heap      the heap
 */
xu_void_t xu_heap_pop(xu_heap_ref_t heap);

/*! remove the heap item using iterator only for algorithm(find, ...)
 *
 * @param heap      the heap
 * @param itor      the itor
 */
xu_void_t xu_heap_remove(xu_heap_ref_t heap, xu_size_t itor);

#ifdef __xu_debug__
/*! dump heap
 *
 * @param heap      the heap
 */
xu_void_t xu_heap_dump(xu_heap_ref_t heap);
#endif
