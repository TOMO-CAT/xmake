#pragma once

#include "xutil/container/iterator.h"
#include "xutil/container/prefix.h"
#include "xutil/prefix.h"
#include "xutil/utils/utils.h"

// the list entry
#define xu_single_list_entry(head, entry) ((((xu_byte_t*)(entry)) - (head)->eoff))

/*! get the list entry with zero offset
 *
 * @code
 *
    // the xxxx entry type
    typedef struct __xu_xxxx_entry_t
    {
        // the list entry (be placed in header with zero offset)
        xu_single_list_entry_t  entry;

        // ..

    }xu_xxxx_entry_t;
 *
 * @endcode
 */
#define xu_single_list_entry0(entry) (entry)

/*! init the list entry
 *
 * @code
 *
    // the xxxx entry type
    typedef struct __xu_xxxx_entry_t
    {
        // the list entry
        xu_single_list_entry_t      entry;

        // the data
        xu_size_t                   data;

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
    xu_single_list_entry_head_t list;
    xu_single_list_entry_init(&list, xu_xxxx_entry_t, entry, xu_xxxx_entry_copy);

 * @endcode
 */
#define xu_single_list_entry_init(list, type, entry, copy)                                                             \
    xu_single_list_entry_init_(list, xu_offsetof(type, entry), sizeof(type), copy)

/*! the single-linked list entry type
 *
 * <pre>
 * list: head => ... => last => null
 *
 * </pre>
 */
typedef struct __xu_single_list_entry_t
{
    // the next entry
    struct __xu_single_list_entry_t* next;

} xu_single_list_entry_t, *xu_single_list_entry_ref_t;

// the single-linked list entry head type
typedef struct __xu_single_list_entry_head_t
{
    // the next entry
    struct __xu_single_list_entry_t* next;

    // the last entry
    struct __xu_single_list_entry_t* last;

    // the list size
    xu_size_t size;

    // the iterator
    xu_iterator_t itor;

    // the entry offset
    xu_size_t eoff;

    // the entry copy func
    xu_entry_copy_t copy;

} xu_single_list_entry_head_t, *xu_single_list_entry_head_ref_t;

/*! the list iterator
 *
 * @param list                                  the list
 *
 * @return                                      the list iterator
 */
xu_iterator_ref_t xu_single_list_entry_itor(xu_single_list_entry_head_ref_t list);

/*! init list
 *
 * @param list                                  the list
 * @param entry_offset                          the entry offset
 * @param entry_size                            the entry size
 * @param copy                                  the copy func of the entry for algorithm, .e.g sort
 */
xu_void_t xu_single_list_entry_init_(xu_single_list_entry_head_ref_t list, xu_size_t entry_offset, xu_size_t entry_size,
                                     xu_entry_copy_t copy);

/*! exit list
 *
 * @param list                                  the list
 */
xu_void_t xu_single_list_entry_exit(xu_single_list_entry_head_ref_t list);

/*! clear list
 *
 * @param list                                  the list
 */
static __xu_inline__ xu_void_t xu_single_list_entry_clear(xu_single_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // clear it
    list->next = xu_null;
    list->last = xu_null;
    list->size = 0;
}

/*! the list entry count
 *
 * @param list                                  the list
 *
 * @return                                      the list entry count
 */
static __xu_inline__ xu_size_t xu_single_list_entry_size(xu_single_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return list->size;
}

/*! the list next entry
 *
 * @param entry                                 the entry
 *
 * @return                                      the next entry
 */
static __xu_inline__ xu_single_list_entry_ref_t xu_single_list_entry_next(xu_single_list_entry_ref_t entry)
{
    // check
    xu_assert(entry);

    // done
    return entry->next;
}

/*! the list head entry
 *
 * @param list                                  the list
 *
 * @return                                      the head entry
 */
static __xu_inline__ xu_single_list_entry_ref_t xu_single_list_entry_head(xu_single_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return list->next;
}

/*! the list last entry
 *
 * @param list                                  the list
 *
 * @return                                      the last entry
 */
static __xu_inline__ xu_single_list_entry_ref_t xu_single_list_entry_last(xu_single_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return list->last;
}

/*! the list tail entry
 *
 * @param list                                  the list
 *
 * @return                                      the tail entry
 */
static __xu_inline__ xu_single_list_entry_ref_t xu_single_list_entry_tail(xu_single_list_entry_head_ref_t list)
{
    return xu_null;
}

/*! the list is null?
 *
 * @param list                                  the list
 *
 * @return                                      xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_single_list_entry_is_null(xu_single_list_entry_head_ref_t list)
{
    // check
    xu_assert(list);

    // done
    return !list->size;
}
/*! is the list head entry?
 *
 * @param list                                  the list
 * @param entry                                 the entry
 *
 * @return                                      xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_single_list_entry_is_head(xu_single_list_entry_head_ref_t list,
                                                            xu_single_list_entry_ref_t      entry)
{
    // check
    xu_assert(list);

    // done
    return list->next == entry;
}

/*! is the list last entry?
 *
 * @param list                                  the list
 * @param entry                                 the entry
 *
 * @return                                      xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_single_list_entry_is_last(xu_single_list_entry_head_ref_t list,
                                                            xu_single_list_entry_ref_t      entry)
{
    // check
    xu_assert(list);

    // done
    return list->last == entry;
}

/*! insert entry to the next
 *
 * @param list                                  the list
 * @param node                                  the list node
 * @param entry                                 the inserted list entry
 */
static __xu_inline__ xu_void_t xu_single_list_entry_insert_next(xu_single_list_entry_head_ref_t list,
                                                                xu_single_list_entry_ref_t      node,
                                                                xu_single_list_entry_ref_t      entry)
{
    // check
    xu_assert(list && node && entry);
    xu_assert(node != entry);

    // update last
    if (node == list->last || !list->next) list->last = entry;

    // insert entry
    entry->next = node->next;
    node->next  = entry;

    // size++
    list->size++;
}

/*! insert entry to the head
 *
 * @param list                                  the list
 * @param entry                                 the inserted list entry
 */
static __xu_inline__ xu_void_t xu_single_list_entry_insert_head(xu_single_list_entry_head_ref_t list,
                                                                xu_single_list_entry_ref_t      entry)
{
    // insert it
    xu_single_list_entry_insert_next(list, (xu_single_list_entry_ref_t)list, entry);
}

/*! insert entry to the tail
 *
 * @param list                                  the list
 * @param entry                                 the inserted list entry
 */
static __xu_inline__ xu_void_t xu_single_list_entry_insert_tail(xu_single_list_entry_head_ref_t list,
                                                                xu_single_list_entry_ref_t      entry)
{
    // check
    xu_assert(list && entry);

    // insert it
    if (list->last)
        xu_single_list_entry_insert_next(list, list->last, entry);
    else
        xu_single_list_entry_insert_head(list, entry);
}

/*! replace the next entry
 *
 * @param list                                  the list
 * @param node                                  the list node
 * @param entry                                 the new list entry
 */
static __xu_inline__ xu_void_t xu_single_list_entry_replace_next(xu_single_list_entry_head_ref_t list,
                                                                 xu_single_list_entry_ref_t      node,
                                                                 xu_single_list_entry_ref_t      entry)
{
    // check
    xu_assert(node && node->next);
    xu_assert(node != entry);

    // update last
    if (node->next == list->last) list->last = entry;

    // replace it
    entry->next = node->next->next;
    node->next  = entry;
}

/*! replace the head entry
 *
 * @param list                                  the list
 * @param entry                                 the new list entry
 */
static __xu_inline__ xu_void_t xu_single_list_entry_replace_head(xu_single_list_entry_head_ref_t list,
                                                                 xu_single_list_entry_ref_t      entry)
{
    // replace it
    xu_single_list_entry_replace_next(list, (xu_single_list_entry_ref_t)list, entry);
}

/*! remove the entry (private interface)
 *
 * @param list                                  the list
 * @param prev                                  the prev entry
 * @param next                                  the next entry
 */
static __xu_inline__ xu_void_t xu_single_list_entry_remove_(xu_single_list_entry_head_ref_t list,
                                                            xu_single_list_entry_ref_t      prev,
                                                            xu_single_list_entry_ref_t      next)
{
    // check
    xu_assert(list && list->size && prev);

    // update last, prev -> last -> next (null)
    if (prev->next == list->last) list->last = prev;

    // remove entries
    prev->next = next;

    // update size
    list->size--;
}

/*! remove the next entry
 *
 * @param list                                  the list
 * @param prev                                  the prev entry
 */
static __xu_inline__ xu_void_t xu_single_list_entry_remove_next(xu_single_list_entry_head_ref_t list,
                                                                xu_single_list_entry_ref_t      prev)
{
    // check
    xu_assert(prev && prev->next);

    // remove it
    xu_single_list_entry_remove_(list, prev, prev->next->next);
}

/*! remove the head entry
 *
 * @param list                                  the list
 */
static __xu_inline__ xu_void_t xu_single_list_entry_remove_head(xu_single_list_entry_head_ref_t list)
{
    // check
    xu_assert(list->next);

    // remove it
    xu_single_list_entry_remove_(list, (xu_single_list_entry_ref_t)list, list->next->next);
}
