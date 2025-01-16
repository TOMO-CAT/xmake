#pragma once

#include "xutil/container/iterator.h"
#include "xutil/container/prefix.h"
#include "xutil/macros/type.h"
#include "xutil/prefix.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */
// get the list entry
#define xu_list_entry(head, entry) ((((xu_byte_t*)(entry)) - (head)->eoff))

/*! get the list entry with zero offset
 *
 * @code
 *
    // the xxxx entry type
    typedef struct __xu_xxxx_entry_t
    {
        // the list entry (be placed in header with zero offset)
        xu_list_entry_t     entry;

        // ..

    }xu_xxxx_entry_t;
 *
 * @endcode
 */
#define xu_list_entry0(entry) (entry)

/*! init the list entry
 *
 * @code
 *
    // the xxxx entry type
    typedef struct __xu_xxxx_entry_t
    {
        // the list entry
        xu_list_entry_t     entry;

        // the data
        xu_size_t           data;

    }xu_xxxx_entry_t;

    // the xxxx entry copy func
    static xu_void_t xu_xxxx_entry_copy(xu_pointer_t litem, xu_pointer_t ritem)
    {
        // check
        xu_assert(litem && ritem);

        // copy it
        ((xu_xxxx_entry_t*)litem)->data = ((xu_xxxx_entry_t*)ritem)->data;
    }

    // init the list
    xu_list_entry_head_t list;
    xu_list_entry_init(&list, xu_xxxx_entry_t, entry, xu_xxxx_entry_copy);

 * @endcode
 */
#define xu_list_entry_init(list, type, entry, copy)                                                                    \
    xu_list_entry_init_(list, xu_offsetof(type, entry), sizeof(type), copy)

/* *******************************************************
 * types
 * *******************************************************
 */

/*! the doubly-linked list entry type
 *
 * <pre>
 * list: list => ... => last
 *        |               |
 *        <---------------
 *
 * </pre>
 */

typedef struct __xu_list_entry_t
{
    /// the next entry
    struct __xu_list_entry_t* next;

    /// the prev entry
    struct __xu_list_entry_t* prev;

} xu_list_entry_t, *xu_list_entry_ref_t;

// the list entry head type
typedef struct __xu_list_entry_head_t
{
    /// the next entry
    struct __xu_list_entry_t* next;

    /// the prev entry
    struct __xu_list_entry_t* prev;

    /// the list size
    xu_size_t size;

    /// the iterator
    xu_iterator_t itor;

    /// the entry offset
    xu_size_t eoff;

    /// the entry copy func
    xu_entry_copy_t copy;

} xu_list_entry_head_t, *xu_list_entry_head_ref_t;

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

/*! the list iterator
 *
 * @param list                              the list
 *
 * @return                                  the list iterator
 */
xu_iterator_ref_t xu_list_entry_itor(xu_list_entry_head_ref_t list);

/*! init list
 *
 * @param list                              the list
 * @param entry_offset                      the entry offset
 * @param entry_size                        the entry size
 * @param copy                              the copy func of the entry for algorithm, .e.g sort
 */
xu_void_t xu_list_entry_init_(xu_list_entry_head_ref_t list, xu_size_t entry_offset, xu_size_t entry_size,
                              xu_entry_copy_t copy);

/*! exit list
 *
 * @param list                              the list
 */
xu_void_t xu_list_entry_exit(xu_list_entry_head_ref_t list);

/*! clear list
 *
 * @param list                              the list
 */
static __xu_inline__ xu_void_t xu_list_entry_clear(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // clear it
    list->next = (xu_list_entry_ref_t)list;
    list->prev = (xu_list_entry_ref_t)list;
    list->size = 0;
}

/*! the list entry count
 *
 * @param list                              the list
 *
 * @return                                  the list entry count
 */
static __xu_inline__ xu_size_t xu_list_entry_size(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return list->size;
}

/*! the list next entry
 *
 * @param entry                             the entry
 *
 * @return                                  the next entry
 */
static __xu_inline__ xu_list_entry_ref_t xu_list_entry_next(xu_list_entry_ref_t entry)
{
    // check
    xu_assert(entry);

    // done
    return entry->next;
}

/*! the list prev entry
 *
 * @param entry                             the entry
 *
 * @return                                  the prev entry
 */
static __xu_inline__ xu_list_entry_ref_t xu_list_entry_prev(xu_list_entry_ref_t entry)
{
    // check
    xu_assert(entry);

    // done
    return entry->prev;
}

/*! the list head entry
 *
 * @param list                              the list
 *
 * @return                                  the head entry
 */
static __xu_inline__ xu_list_entry_ref_t xu_list_entry_head(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return list->next;
}

/*! the list last entry
 *
 * @param list                              the list
 *
 * @return                                  the last entry
 */
static __xu_inline__ xu_list_entry_ref_t xu_list_entry_last(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return list->prev;
}

/*! the list tail entry
 *
 * @param list                              the list
 *
 * @return                                  the tail entry
 */
static __xu_inline__ xu_list_entry_ref_t xu_list_entry_tail(xu_list_entry_head_ref_t list)
{
    // done
    return (xu_list_entry_ref_t)list;
}

/*! the list is null?
 *
 * @param list                              the list
 *
 * @return                                  xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_list_entry_is_null(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return !list->size;
}

/*! is the list head entry?
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_list_entry_is_head(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    // check
    xu_assert(list);

    // done
    return list->next == entry;
}

/*! is the list last entry?
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_list_entry_is_last(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    // check
    xu_assert(list);

    // done
    return list->prev == entry;
}

/*! the list is valid?
 *
 * @param list                              the list
 *
 * @return                                  xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_list_entry_is_valid(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return (list->next && list->next->prev == (xu_list_entry_ref_t)list) &&
           (list->prev && list->prev->next == (xu_list_entry_ref_t)list);
}

/*! splice the spliced_list to list[prev, next]
 *
 * @param list                              the list
 * @param prev                              the prev
 * @param next                              the next
 * @param spliced_list                      the spliced list
 */
static __xu_inline__ xu_void_t xu_list_entry_splice(xu_list_entry_head_ref_t list, xu_list_entry_ref_t prev,
                                                    xu_list_entry_ref_t next, xu_list_entry_head_ref_t spliced_list)
{
    // check
    xu_assert(list && prev && next);
    xu_assert(spliced_list && spliced_list->next && spliced_list->prev);

    // valid?
    xu_assert(xu_list_entry_is_valid(list));
    xu_assert(xu_list_entry_is_valid(spliced_list));

    // empty?
    xu_check_return(!xu_list_entry_is_null(spliced_list));

    // done
    spliced_list->next->prev = prev;
    prev->next               = spliced_list->next;
    spliced_list->prev->next = next;
    next->prev               = spliced_list->prev;

    // update size
    list->size += spliced_list->size;

    // clear the spliced list
    xu_list_entry_clear(spliced_list);
}

/*! splice the spliced_list at the list head
 *
 * @param list                              the list
 * @param spliced_list                      the spliced list
 */
static __xu_inline__ xu_void_t xu_list_entry_splice_head(xu_list_entry_head_ref_t list,
                                                         xu_list_entry_head_ref_t spliced_list)
{
    // check
    xu_assert(list);

    // done
    xu_list_entry_splice(list, (xu_list_entry_ref_t)list, list->next, spliced_list);
}

/*! splice the spliced_list at the list tail
 *
 * @param list                              the list
 * @param spliced_list                      the spliced list
 */
static __xu_inline__ xu_void_t xu_list_entry_splice_tail(xu_list_entry_head_ref_t list,
                                                         xu_list_entry_head_ref_t spliced_list)
{
    // check
    xu_assert(list);

    // done
    xu_list_entry_splice(list, list->prev, (xu_list_entry_ref_t)list, spliced_list);
}

/*! insert entry to the next
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the inserted list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_insert_next(xu_list_entry_head_ref_t list, xu_list_entry_ref_t node,
                                                         xu_list_entry_ref_t entry)
{
    // check
    xu_assert(list && node && node->next && entry);
    xu_assert(node != entry);

    // valid?
    xu_assert(xu_list_entry_is_valid(list));

    // insert entry
    node->next->prev = entry;
    entry->next      = node->next;
    entry->prev      = node;
    node->next       = entry;

    // size++
    list->size++;
}

/*! insert entry to the prev
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the inserted list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_insert_prev(xu_list_entry_head_ref_t list, xu_list_entry_ref_t node,
                                                         xu_list_entry_ref_t entry)
{
    // check
    xu_assert(list && node);

    // insert it
    xu_list_entry_insert_next(list, node->prev, entry);
}

/*! insert entry to the head
 *
 * @param list                              the list
 * @param entry                             the inserted list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_insert_head(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    xu_list_entry_insert_next(list, (xu_list_entry_ref_t)list, entry);
}

/*! insert entry to the tail
 *
 * @param list                              the list
 * @param entry                             the inserted list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_insert_tail(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    // check
    xu_assert(list && entry);

    // insert it
    xu_list_entry_insert_next(list, list->prev, entry);
}

/*! replace the entry
 *
 * @param list                              the list
 * @param node                              the replaced list node
 * @param entry                             the new list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_replace(xu_list_entry_head_ref_t list, xu_list_entry_ref_t node,
                                                     xu_list_entry_ref_t entry)
{
    // check
    xu_assert(node && node->next && node->prev && entry);
    xu_assert(node != entry);

    // valid?
    xu_assert(xu_list_entry_is_valid(list));

    // replace it
    entry->next       = node->next;
    entry->next->prev = entry;
    entry->prev       = node->prev;
    entry->prev->next = entry;
}

/*! replace the next entry
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the new list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_replace_next(xu_list_entry_head_ref_t list, xu_list_entry_ref_t node,
                                                          xu_list_entry_ref_t entry)
{
    // check
    xu_assert(node);

    // replace it
    xu_list_entry_replace(list, node->next, entry);
}

/*! replace the prev entry
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the new list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_replace_prev(xu_list_entry_head_ref_t list, xu_list_entry_ref_t node,
                                                          xu_list_entry_ref_t entry)
{
    // check
    xu_assert(node);

    // replace it
    xu_list_entry_replace(list, node->prev, entry);
}

/*! replace the head entry
 *
 * @param list                              the list
 * @param entry                             the new list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_replace_head(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    // check
    xu_assert(list);

    // replace it
    xu_list_entry_replace(list, list->next, entry);
}

/*! replace the last entry
 *
 * @param list                              the list
 * @param entry                             the new list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_replace_last(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    // check
    xu_assert(list);

    // replace it
    xu_list_entry_replace(list, list->prev, entry);
}

/*! remove the entry (private interface)
 *
 * @param list                              the list
 * @param prev                              the prev entry
 * @param next                              the next entry
 */
static __xu_inline__ xu_void_t xu_list_entry_remove_(xu_list_entry_head_ref_t list, xu_list_entry_ref_t prev,
                                                     xu_list_entry_ref_t next)
{
    // check
    xu_assert(list && list->size && prev && next);

    // valid?
    xu_assert(xu_list_entry_is_valid(list));

    // remove entries
    prev->next = next;
    next->prev = prev;

    // update size
    list->size--;
}

/*! remove the entry
 *
 * @param list                              the list
 * @param entry                             the removed list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_remove(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    // check
    xu_assert(entry);

    // remove it
    xu_list_entry_remove_(list, entry->prev, entry->next);
}

/*! remove the next entry
 *
 * @param list                              the list
 * @param prev                              the prev entry
 */
static __xu_inline__ xu_void_t xu_list_entry_remove_next(xu_list_entry_head_ref_t list, xu_list_entry_ref_t prev)
{
    // check
    xu_assert(prev && prev->next);

    // remove it
    xu_list_entry_remove_(list, prev, prev->next->next);
}

/*! remove the prev entry
 *
 * @param list                              the list
 * @param next                              the next entry
 */
static __xu_inline__ xu_void_t xu_list_entry_remove_prev(xu_list_entry_head_ref_t list, xu_list_entry_ref_t next)
{
    // check
    xu_assert(next && next->prev);

    // remove it
    xu_list_entry_remove_(list, next->prev->prev, next);
}

/*! remove the head entry
 *
 * @param list                              the list
 */
static __xu_inline__ xu_void_t xu_list_entry_remove_head(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert(list && list->next);

    // remove it
    xu_list_entry_remove_(list, (xu_list_entry_ref_t)list, list->next->next);
}

/*! remove the last entry
 *
 * @param list                              the list
 */
static __xu_inline__ xu_void_t xu_list_entry_remove_last(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert(list && list->prev);

    // remove it
    xu_list_entry_remove_(list, list->prev->prev, (xu_list_entry_ref_t)list);
}

/*! moveto the next entry
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the moved list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_moveto_next(xu_list_entry_head_ref_t list, xu_list_entry_ref_t node,
                                                         xu_list_entry_ref_t entry)
{
    // check
    xu_check_return(node != entry);

    // move it
    xu_list_entry_remove(list, entry);
    xu_list_entry_insert_next(list, node, entry);
}

/*! moveto the prev entry
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the moved list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_moveto_prev(xu_list_entry_head_ref_t list, xu_list_entry_ref_t node,
                                                         xu_list_entry_ref_t entry)
{
    // check
    xu_assert(node);

    // move it
    xu_list_entry_moveto_next(list, node->prev, entry);
}

/*! moveto the head entry
 *
 * @param list                              the list
 * @param entry                             the moved list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_moveto_head(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    // move it
    xu_list_entry_moveto_next(list, (xu_list_entry_ref_t)list, entry);
}

/*! moveto the tail entry
 *
 * @param list                              the list
 * @param entry                             the moved list entry
 */
static __xu_inline__ xu_void_t xu_list_entry_moveto_tail(xu_list_entry_head_ref_t list, xu_list_entry_ref_t entry)
{
    // check
    xu_assert(list);

    // move it
    xu_list_entry_moveto_next(list, list->prev, entry);
}
