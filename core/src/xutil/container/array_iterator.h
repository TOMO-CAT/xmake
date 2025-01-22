#pragma once

#include "xutil/container/element.h"
#include "xutil/container/iterator.h"
#include "xutil/libc/string/string.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  types
 */

// the array iterator type
typedef struct __xu_array_iterator_t
{
    /// the iterator base
    xu_iterator_t base;

    /// the items
    xu_pointer_t items;

    /// the items count
    xu_size_t count;

} xu_array_iterator_t, *xu_array_iterator_ref_t;

/* *******************************************************
 *  interfaces
 */

/*! init array iterator for pointer element
 *
 * @param iterator  the array iterator
 * @param items     the items
 * @param count     the count
 *
 * @return          the iterator
 */
xu_iterator_ref_t xu_array_iterator_init_ptr(xu_array_iterator_ref_t iterator, xu_pointer_t* items, xu_size_t count);

/*! init array iterator for memory element
 *
 * @param iterator  the array iterator
 * @param items     the items
 * @param count     the count
 * @param size      the element size
 *
 * @return          the iterator
 */
xu_iterator_ref_t xu_array_iterator_init_mem(xu_array_iterator_ref_t iterator, xu_pointer_t items, xu_size_t count,
                                             xu_size_t size);

/*! init array iterator for c-string element
 *
 * @param iterator  the array iterator
 * @param items     the items
 * @param count     the count
 *
 * @return          the iterator
 */
xu_iterator_ref_t xu_array_iterator_init_str(xu_array_iterator_ref_t iterator, xu_char_t** items, xu_size_t count);

/*! init array iterator for c-string element and ignore case
 *
 * @param iterator  the array iterator
 * @param items     the items
 * @param count     the count
 *
 * @return          the iterator
 */
xu_iterator_ref_t xu_array_iterator_init_istr(xu_array_iterator_ref_t iterator, xu_char_t** items, xu_size_t count);

/*! init array iterator for long element
 *
 * @param iterator  the array iterator
 * @param items     the items
 * @param count     the count
 *
 * @return          the iterator
 */
xu_iterator_ref_t xu_array_iterator_init_long(xu_array_iterator_ref_t iterator, xu_long_t* items, xu_size_t count);

/*! init array iterator for size element
 *
 * @param iterator  the array iterator
 * @param items     the items
 * @param count     the count
 *
 * @return          the iterator
 */
xu_iterator_ref_t xu_array_iterator_init_size(xu_array_iterator_ref_t iterator, xu_size_t* items, xu_size_t count);
