#include "xutil/container/prefix.h"
#include "xutil/container/element.h"
#include "xutil/container/iterator.h"

/* *******************************************************
 * types
 */

/*! the double list ref type
 *
 *
 * <pre>
 * list: |-----| => |-------------------------------------------------=> |------| => |------| => tail
 *        head                                                                         last
 *
 * performance:
 *
 * insert:
 * insert midd: slow
 * insert head: fast
 * insert tail: fast
 * insert next: fast
 *
 * remove:
 * remove midd: slow
 * remove head: fast
 * remove last: fast
 * remove next: fast
 *
 * iterator:
 * next: fast
 * </pre>
 *
 */
typedef xu_iterator_ref_t   xu_single_list_ref_t;

/* *******************************************************
 * interfaces
 */

/*! init list
 *
 * @param grow          the grow size
 * @param element       the element
 *
 * @return              the list
 */
xu_single_list_ref_t    xu_single_list_init(xu_size_t grow, xu_element_t element);

/*! exit list
 *
 * @param list          the list
 */
xu_void_t               xu_single_list_exit(xu_single_list_ref_t list);

/*! clear list
 *
 * @param list          the list
 */
xu_void_t               xu_single_list_clear(xu_single_list_ref_t list);

/*! the list head item
 *
 * @param list          the list
 *
 * @return              the head item
 */
xu_pointer_t            xu_single_list_head(xu_single_list_ref_t list);

/*! the list last item
 *
 * @param list          the list
 *
 * @return              the last item
 */
xu_pointer_t            xu_single_list_last(xu_single_list_ref_t list);

/*! insert the next item
 *
 * @param list          the list
 * @param itor          the item itor
 * @param data          the item data
 *
 * @return              the item itor
 */
xu_size_t               xu_single_list_insert_next(xu_single_list_ref_t list, xu_size_t itor, xu_cpointer_t data);

/*! insert the head item
 *
 * @param list          the list
 * @param data          the item data
 *
 * @return              the item itor
 */
xu_size_t               xu_single_list_insert_head(xu_single_list_ref_t list, xu_cpointer_t data);

/*! insert the tail item
 *
 * @param list          the list
 * @param data          the item data
 *
 * @return              the item itor
 */
xu_size_t               xu_single_list_insert_tail(xu_single_list_ref_t list, xu_cpointer_t data);

/*! replace the item
 *
 * @param list          the list
 * @param itor          the item itor
 * @param data          the item data
 */
xu_void_t               xu_single_list_replace(xu_single_list_ref_t list, xu_size_t itor, xu_cpointer_t data);

/*! replace the head item
 *
 * @param list          the list
 * @param data          the item data
 */
xu_void_t               xu_single_list_replace_head(xu_single_list_ref_t list, xu_cpointer_t data);

/*! replace the tail item
 *
 * @param list          the list
 * @param data          the item data
 */
xu_void_t               xu_single_list_replace_last(xu_single_list_ref_t list, xu_cpointer_t data);

/*! remove the next item
 *
 * @param list          the list
 * @param itor          the item itor
 */
xu_void_t               xu_single_list_remove_next(xu_single_list_ref_t list, xu_size_t itor);

/*! remove the head item
 *
 * @param list          the list
 */
xu_void_t               xu_single_list_remove_head(xu_single_list_ref_t list);

/*! the item count
 *
 * @param list          the list
 *
 * @return              the item count
 */
xu_size_t               xu_single_list_size(xu_single_list_ref_t list);

/*! the item max count
 *
 * @param list          the list
 *
 * @return              the item max count
 */
xu_size_t               xu_single_list_maxn(xu_single_list_ref_t list);

#ifdef __xu_debug__
/*! dump list
 *
 * @param list          the list
 */
xu_void_t               xu_single_list_dump(xu_single_list_ref_t list);
#endif
