#pragma once

#include "xutil/container/element.h"
#include "xutil/container/iterator.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/malloc.h"
#include "xutil/prefix.h"
#include "xutil/utils/trace.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * types
 * *******************************************************
 */

/*! the vector ref type
 *
 * <pre>
 * vector: |-----|--------------------------------------------------------|------|
 *       head                                                           last    tail
 *
 * performance:
 *
 * insert:
 * insert midd: slow
 * insert head: slow
 * insert tail: fast
 *
 * ninsert:
 * ninsert midd: fast
 * ninsert head: fast
 * ninsert tail: fast
 *
 * remove:
 * remove midd: slow
 * remove head: slow
 * remove last: fast
 *
 * nremove:
 * nremove midd: fast
 * nremove head: fast
 * nremove last: fast
 *
 * iterator:
 * next: fast
 * prev: fast
 * </pre>
 *
 * @note the itor of the same item is mutable
 *
 */
typedef xu_iterator_ref_t xu_vector_ref_t;

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! init vector
 *
 * @code
 *
    // init vector
    xu_vector_ref_t vector = xu_vector_init(0, xu_element_str(xu_true));
    if (vector)
    {
        // insert elements into head
        xu_vector_insert_head(vector, "hi!");

        // insert elements into tail
        xu_vector_insert_tail(vector, "how");
        xu_vector_insert_tail(vector, "are");
        xu_vector_insert_tail(vector, "you");

        // dump elements
        xu_for_all (xu_char_t const*, cstr, vector)
        {
            // trace
            xu_trace_d("%s", cstr);
        }

        // exit vector
        xu_vector_exit(vector);
    }
 * @endcode
 *
 * @param grow      the item grow
 * @param element   the element
 *
 * @return          the vector
 */
xu_vector_ref_t xu_vector_init(xu_size_t grow, xu_element_t element);

/*! exist vector
 *
 * @param vector    the vector
 */
xu_void_t xu_vector_exit(xu_vector_ref_t vector);

/*! the vector data
 *
 * @param vector    the vector
 *
 * @return          the vector data
 */
xu_pointer_t xu_vector_data(xu_vector_ref_t vector);

/*! the vector head item
 *
 * @param vector    the vector
 *
 * @return          the vector head item
 */
xu_pointer_t xu_vector_head(xu_vector_ref_t vector);

/*! the vector last item
 *
 * @param vector    the vector
 *
 * @return          the vector last item
 */
xu_pointer_t xu_vector_last(xu_vector_ref_t vector);

/*! resize the vector
 *
 * @param vector    the vector
 * @param size      the vector size
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_vector_resize(xu_vector_ref_t vector, xu_size_t size);

/*! clear the vector
 *
 * @param vector    the vector
 */
xu_void_t xu_vector_clear(xu_vector_ref_t vector);

/*! copy the vector
 *
 * @param vector    the vector
 * @param copy      the copied vector
 */
xu_void_t xu_vector_copy(xu_vector_ref_t vector, xu_vector_ref_t copy);

/*! insert the vector prev item
 *
 * @param vector    the vector
 * @param itor      the item itor
 * @param data      the item data
 */
xu_void_t xu_vector_insert_prev(xu_vector_ref_t vector, xu_size_t itor, xu_cpointer_t data);

/*! insert the vector next item
 *
 * @param vector    the vector
 * @param itor      the item itor
 * @param data      the item data
 */
xu_void_t xu_vector_insert_next(xu_vector_ref_t vector, xu_size_t itor, xu_cpointer_t data);

/*! insert the vector head item
 *
 * @param vector    the vector
 * @param data      the item data
 */
xu_void_t xu_vector_insert_head(xu_vector_ref_t vector, xu_cpointer_t data);

/*! insert the vector tail item
 *
 * @param vector    the vector
 * @param data      the item data
 */
xu_void_t xu_vector_insert_tail(xu_vector_ref_t vector, xu_cpointer_t data);

/*! insert the vector prev items
 *
 * @param vector    the vector
 * @param itor      the item itor
 * @param data      the item data
 * @param size      the item count
 */
xu_void_t xu_vector_ninsert_prev(xu_vector_ref_t vector, xu_size_t itor, xu_cpointer_t data, xu_size_t size);

/*! insert the vector next items
 *
 * @param vector    the vector
 * @param itor      the item itor
 * @param data      the item data
 * @param size      the item count
 */
xu_void_t xu_vector_ninsert_next(xu_vector_ref_t vector, xu_size_t itor, xu_cpointer_t data, xu_size_t size);

/*! insert the vector head items
 *
 * @param vector    the vector
 * @param data      the item data
 * @param size      the item count
 */
xu_void_t xu_vector_ninsert_head(xu_vector_ref_t vector, xu_cpointer_t data, xu_size_t size);

/*! insert the vector tail items
 *
 * @param vector    the vector
 * @param data      the item data
 * @param size      the item count
 */
xu_void_t xu_vector_ninsert_tail(xu_vector_ref_t vector, xu_cpointer_t data, xu_size_t size);

/*! replace the vector item
 *
 * @param vector    the vector
 * @param itor      the item itor
 * @param data      the item data
 */
xu_void_t xu_vector_replace(xu_vector_ref_t vector, xu_size_t itor, xu_cpointer_t data);

/*! replace the vector head item
 *
 * @param vector    the vector
 * @param data      the item data
 */
xu_void_t xu_vector_replace_head(xu_vector_ref_t vector, xu_cpointer_t data);

/*! replace the vector last item
 *
 * @param vector    the vector
 * @param data      the item data
 */
xu_void_t xu_vector_replace_last(xu_vector_ref_t vector, xu_cpointer_t data);

/*! replace the vector items
 *
 * @param vector    the vector
 * @param itor      the item itor
 * @param data      the item data
 * @param size      the item count
 */
xu_void_t xu_vector_nreplace(xu_vector_ref_t vector, xu_size_t itor, xu_cpointer_t data, xu_size_t size);

/*! replace the vector head items
 *
 * @param vector    the vector
 * @param data      the item data
 * @param size      the item count
 */
xu_void_t xu_vector_nreplace_head(xu_vector_ref_t vector, xu_cpointer_t data, xu_size_t size);

/*! replace the vector last items
 *
 * @param vector    the vector
 * @param data      the item data
 * @param size      the item count
 */
xu_void_t xu_vector_nreplace_last(xu_vector_ref_t vector, xu_cpointer_t data, xu_size_t size);

/*! remove the vector item
 *
 * @param vector    the vector
 * @param itor      the item itor
 */
xu_void_t xu_vector_remove(xu_vector_ref_t vector, xu_size_t itor);

/*! remove the vector head item
 *
 * @param vector    the vector
 */
xu_void_t xu_vector_remove_head(xu_vector_ref_t vector);

/*! remove the vector last item
 *
 * @param vector    the vector
 */
xu_void_t xu_vector_remove_last(xu_vector_ref_t vector);

/*! remove the vector items
 *
 * @param vector    the vector
 * @param itor      the item itor
 * @param size      the item count
 */
xu_void_t xu_vector_nremove(xu_vector_ref_t vector, xu_size_t itor, xu_size_t size);

/*! remove the vector head items
 *
 * @param vector    the vector
 * @param size      the item count
 */
xu_void_t xu_vector_nremove_head(xu_vector_ref_t vector, xu_size_t size);

/*! remove the vector last items
 *
 * @param vector    the vector
 * @param size      the item count
 */
xu_void_t xu_vector_nremove_last(xu_vector_ref_t vector, xu_size_t size);

/*! the vector size
 *
 * @param vector    the vector
 *
 * @return          the vector size
 */
xu_size_t xu_vector_size(xu_vector_ref_t vector);

/*! the vector grow
 *
 * @param vector    the vector
 *
 * @return          the vector grow
 */
xu_size_t xu_vector_grow(xu_vector_ref_t vector);

/*! the vector maxn
 *
 * @param vector    the vector
 *
 * @return          the vector maxn
 */
xu_size_t xu_vector_maxn(xu_vector_ref_t vector);

#ifdef __xu_debug__
/*! dump vector
 *
 * @param vector    the vector
 */
xu_void_t xu_vector_dump(xu_vector_ref_t vector);
#endif
