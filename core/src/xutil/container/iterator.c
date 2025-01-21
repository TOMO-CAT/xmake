#include "xutil/container/iterator.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_size_t xu_iterator_mode(xu_iterator_ref_t iterator)
{
    xu_assert(iterator);
    return (xu_size_t)iterator->mode;
}
xu_size_t xu_iterator_flag(xu_iterator_ref_t iterator)
{
    xu_assert(iterator);
    return (xu_size_t)iterator->flag;
}
xu_size_t xu_iterator_step(xu_iterator_ref_t iterator)
{
    xu_assert(iterator);
    return (xu_size_t)iterator->step;
}
xu_size_t xu_iterator_size(xu_iterator_ref_t iterator)
{
    xu_assert(iterator && iterator->op && iterator->op->size);
    return iterator->op->size(iterator);
}
xu_size_t xu_iterator_head(xu_iterator_ref_t iterator)
{
    xu_assert(iterator && iterator->op && iterator->op->head);
    return iterator->op->head(iterator);
}
xu_size_t xu_iterator_last(xu_iterator_ref_t iterator)
{
    xu_assert(iterator && iterator->op && iterator->op->last);
    return iterator->op->last(iterator);
}
xu_size_t xu_iterator_tail(xu_iterator_ref_t iterator)
{
    xu_assert(iterator && iterator->op && iterator->op->tail);
    return iterator->op->tail(iterator);
}
xu_size_t xu_iterator_prev(xu_iterator_ref_t iterator, xu_size_t itor)
{
    xu_assert(iterator && iterator->op && iterator->op->prev);
    return iterator->op->prev(iterator, itor);
}
xu_size_t xu_iterator_next(xu_iterator_ref_t iterator, xu_size_t itor)
{
    xu_assert(iterator && iterator->op && iterator->op->next);
    return iterator->op->next(iterator, itor);
}
xu_pointer_t xu_iterator_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    xu_assert(iterator && iterator->op && iterator->op->item);
    return iterator->op->item(iterator, itor);
}
xu_void_t xu_iterator_remove(xu_iterator_ref_t iterator, xu_size_t itor)
{
    xu_assert(iterator && iterator->op && iterator->op->remove);
    return iterator->op->remove(iterator, itor);
}
xu_void_t xu_iterator_nremove(xu_iterator_ref_t iterator, xu_size_t prev, xu_size_t next, xu_size_t size)
{
    xu_assert(iterator && iterator->op && iterator->op->nremove);
    return iterator->op->nremove(iterator, prev, next, size);
}
xu_void_t xu_iterator_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    xu_assert(iterator && iterator->op && iterator->op->copy);
    return iterator->op->copy(iterator, itor, item);
}
xu_long_t xu_iterator_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    xu_assert(iterator && iterator->op && iterator->op->comp);
    return iterator->op->comp(iterator, litem, ritem);
}
