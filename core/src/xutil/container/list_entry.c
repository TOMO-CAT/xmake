#include "xutil/container/list_entry.h"

/* *******************************************************
 * iterator implementation
 * *******************************************************
 */

static xu_size_t xu_list_entry_itor_size(xu_iterator_ref_t iterator)
{
    // check
    xu_list_entry_head_ref_t list = xu_container_of(xu_list_entry_head_t, itor, iterator);
    xu_assert(list);

    // the size
    return list->size;
}
static xu_size_t xu_list_entry_itor_head(xu_iterator_ref_t iterator)
{
    // check
    xu_list_entry_head_ref_t list = xu_container_of(xu_list_entry_head_t, itor, iterator);
    xu_assert(list);

    // head
    return (xu_size_t)list->next;
}
static xu_size_t xu_list_entry_itor_last(xu_iterator_ref_t iterator)
{
    // check
    xu_list_entry_head_ref_t list = xu_container_of(xu_list_entry_head_t, itor, iterator);
    xu_assert(list);

    // last
    return (xu_size_t)list->prev;
}
static xu_size_t xu_list_entry_itor_tail(xu_iterator_ref_t iterator)
{
    // check
    xu_list_entry_head_ref_t list = xu_container_of(xu_list_entry_head_t, itor, iterator);
    xu_assert(list);

    // tail
    return (xu_size_t)list;
}
static xu_size_t xu_list_entry_itor_next(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(itor);

    // next
    return (xu_size_t)((xu_list_entry_ref_t)itor)->next;
}
static xu_size_t xu_list_entry_itor_prev(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(itor);

    // prev
    return (xu_size_t)((xu_list_entry_ref_t)itor)->prev;
}
static xu_pointer_t xu_list_entry_itor_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_list_entry_head_ref_t list = xu_container_of(xu_list_entry_head_t, itor, iterator);
    xu_assert(list && list->eoff < itor);

    // data
    return (xu_pointer_t)(itor - list->eoff);
}
static xu_void_t xu_list_entry_itor_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    // check
    xu_list_entry_head_ref_t list = xu_container_of(xu_list_entry_head_t, itor, iterator);
    xu_assert(list && list->copy);
    xu_assert(list->eoff < itor && item);

    // copy it
    list->copy((xu_pointer_t)(itor - list->eoff), (xu_pointer_t)item);
}
static xu_void_t xu_list_entry_itor_remove(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_list_entry_head_ref_t list = xu_container_of(xu_list_entry_head_t, itor, iterator);
    xu_assert(list && itor);

    // remove it
    xu_list_entry_remove(list, (xu_list_entry_ref_t)itor);
}
static xu_void_t xu_list_entry_itor_nremove(xu_iterator_ref_t iterator, xu_size_t prev, xu_size_t next, xu_size_t size)
{
    // check
    xu_list_entry_head_ref_t list = xu_container_of(xu_list_entry_head_t, itor, iterator);
    xu_assert(list && prev && next);

    // no size?
    xu_check_return(size);

    // the entry
    xu_list_entry_ref_t prev_entry = (xu_list_entry_ref_t)prev;
    xu_list_entry_ref_t next_entry = (xu_list_entry_ref_t)next;

    // remove entries
    prev_entry->next = next_entry;
    next_entry->prev = prev_entry;

    // update size
    list->size -= size;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_iterator_ref_t xu_list_entry_itor(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert_and_check_return_val(list, xu_null);

    // the iterator
    return &list->itor;
}
xu_void_t xu_list_entry_init_(xu_list_entry_head_ref_t list, xu_size_t entry_offset, xu_size_t entry_size,
                              xu_entry_copy_t copy)
{
    // check
    xu_assert_and_check_return(list && entry_size > sizeof(xu_list_entry_t));

    // init list
    list->next = (xu_list_entry_ref_t)list;
    list->prev = (xu_list_entry_ref_t)list;
    list->size = 0;
    list->eoff = entry_offset;
    list->copy = copy;

    // init operation
    static xu_iterator_op_t op = {
        xu_list_entry_itor_size, xu_list_entry_itor_head,   xu_list_entry_itor_last,   xu_list_entry_itor_tail,
        xu_list_entry_itor_prev, xu_list_entry_itor_next,   xu_list_entry_itor_item,   xu_null,
        xu_list_entry_itor_copy, xu_list_entry_itor_remove, xu_list_entry_itor_nremove};

    // init iterator
    list->itor.priv = xu_null;
    list->itor.step = entry_size;
    list->itor.mode = XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE;
    list->itor.flag = XU_ITERATOR_FLAG_ITEM_REF;
    list->itor.op   = &op;
}
xu_void_t xu_list_entry_exit(xu_list_entry_head_ref_t list)
{
    // check
    xu_assert_and_check_return(list);

    // exit it
    list->next = (xu_list_entry_ref_t)list;
    list->prev = (xu_list_entry_ref_t)list;
    list->size = 0;
}
