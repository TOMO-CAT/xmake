#include "xutil/container/array_iterator.h"

/* *******************************************************
 * iterator implementation for pointer element
 */

static xu_size_t xu_array_iterator_ptr_size(xu_iterator_ref_t iterator)
{
    // check
    xu_assert(iterator);

    // the size
    return ((xu_array_iterator_ref_t)iterator)->count;
}
static xu_size_t xu_array_iterator_ptr_head(xu_iterator_ref_t iterator) { return 0; }
static xu_size_t xu_array_iterator_ptr_tail(xu_iterator_ref_t iterator)
{
    // check
    xu_assert(iterator);

    // the tail
    return ((xu_array_iterator_ref_t)iterator)->count;
}
static xu_size_t xu_array_iterator_ptr_next(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(iterator && itor < ((xu_array_iterator_ref_t)iterator)->count);

    // the next
    return itor + 1;
}
static xu_size_t xu_array_iterator_ptr_prev(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(iterator && itor);

    // the prev
    return itor - 1;
}
static xu_pointer_t xu_array_iterator_ptr_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(iterator && itor < ((xu_array_iterator_ref_t)iterator)->count);

    // the item
    return ((xu_pointer_t*)((xu_array_iterator_ref_t)iterator)->items)[itor];
}
static xu_void_t xu_array_iterator_ptr_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    // check
    xu_assert(iterator && itor < ((xu_array_iterator_ref_t)iterator)->count);

    // copy
    ((xu_cpointer_t*)((xu_array_iterator_ref_t)iterator)->items)[itor] = item;
}
static xu_long_t xu_array_iterator_ptr_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    return (litem < ritem) ? -1 : (litem > ritem);
}

/* *******************************************************
 * iterator implementation for memory element
 */

static xu_pointer_t xu_array_iterator_mem_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(iterator && itor < ((xu_array_iterator_ref_t)iterator)->count);

    // the item
    return (xu_pointer_t)((xu_byte_t*)((xu_array_iterator_ref_t)iterator)->items + itor * iterator->step);
}
static xu_void_t xu_array_iterator_mem_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    // check
    xu_assert(iterator && itor < ((xu_array_iterator_ref_t)iterator)->count);

    // copy
    xu_memcpy((xu_byte_t*)((xu_array_iterator_ref_t)iterator)->items + itor * iterator->step, item, iterator->step);
}
static xu_long_t xu_array_iterator_mem_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    // check
    xu_assert(litem && ritem);

    // compare it
    return xu_memcmp(litem, ritem, iterator->step);
}

/* *******************************************************
 * iterator implementation for c-string element
 */

static xu_long_t xu_array_iterator_str_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    // check
    xu_assert(litem && ritem);

    // compare it
    return xu_strcmp((xu_char_t const*)litem, (xu_char_t const*)ritem);
}
static xu_long_t xu_array_iterator_istr_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    // check
    xu_assert(litem && ritem);

    // compare it
    return xu_stricmp((xu_char_t const*)litem, (xu_char_t const*)ritem);
}

/* *******************************************************
 * iterator implementation for long element
 */
static xu_long_t xu_array_iterator_long_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    return ((xu_long_t)litem < (xu_long_t)ritem) ? -1 : ((xu_long_t)litem > (xu_long_t)ritem);
}

/* *******************************************************
 * implementation
 */

xu_iterator_ref_t xu_array_iterator_init_ptr(xu_array_iterator_ref_t iterator, xu_pointer_t* items, xu_size_t count)
{
    // check
    xu_assert(iterator && items && count);

    // init operation
    static xu_iterator_op_t op = {xu_array_iterator_ptr_size,
                                  xu_array_iterator_ptr_head,
                                  xu_null,
                                  xu_array_iterator_ptr_tail,
                                  xu_array_iterator_ptr_prev,
                                  xu_array_iterator_ptr_next,
                                  xu_array_iterator_ptr_item,
                                  xu_array_iterator_ptr_comp,
                                  xu_array_iterator_ptr_copy,
                                  xu_null,
                                  xu_null};

    // init iterator
    iterator->base.priv = xu_null;
    iterator->base.step = sizeof(xu_pointer_t);
    iterator->base.mode =
        XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE | XU_ITERATOR_MODE_RACCESS | XU_ITERATOR_MODE_MUTABLE;
    iterator->base.flag = XU_ITERATOR_FLAG_ITEM_VAL;
    iterator->base.op   = &op;
    iterator->items     = items;
    iterator->count     = count;

    // ok
    return (xu_iterator_ref_t)iterator;
}
xu_iterator_ref_t xu_array_iterator_init_mem(xu_array_iterator_ref_t iterator, xu_pointer_t items, xu_size_t count,
                                             xu_size_t size)
{
    // check
    xu_assert(iterator && items && count && size);

    // init operation
    static xu_iterator_op_t op = {xu_array_iterator_ptr_size,
                                  xu_array_iterator_ptr_head,
                                  xu_null,
                                  xu_array_iterator_ptr_tail,
                                  xu_array_iterator_ptr_prev,
                                  xu_array_iterator_ptr_next,
                                  xu_array_iterator_mem_item,
                                  xu_array_iterator_mem_comp,
                                  xu_array_iterator_mem_copy,
                                  xu_null,
                                  xu_null};

    // init
    iterator->base.priv = xu_null;
    iterator->base.step = size;
    iterator->base.mode =
        XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE | XU_ITERATOR_MODE_RACCESS | XU_ITERATOR_MODE_MUTABLE;
    iterator->base.flag = XU_ITERATOR_FLAG_ITEM_REF;
    iterator->base.op   = &op;
    iterator->items     = items;
    iterator->count     = count;

    // ok
    return (xu_iterator_ref_t)iterator;
}
xu_iterator_ref_t xu_array_iterator_init_str(xu_array_iterator_ref_t iterator, xu_char_t** items, xu_size_t count)
{
    // check
    xu_assert(iterator && items && count);

    // init operation
    static xu_iterator_op_t op = {xu_array_iterator_ptr_size,
                                  xu_array_iterator_ptr_head,
                                  xu_null,
                                  xu_array_iterator_ptr_tail,
                                  xu_array_iterator_ptr_prev,
                                  xu_array_iterator_ptr_next,
                                  xu_array_iterator_ptr_item,
                                  xu_array_iterator_str_comp,
                                  xu_array_iterator_ptr_copy,
                                  xu_null,
                                  xu_null};

    // init iterator
    iterator->base.priv = xu_null;
    iterator->base.step = sizeof(xu_char_t const*);
    iterator->base.mode =
        XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE | XU_ITERATOR_MODE_RACCESS | XU_ITERATOR_MODE_MUTABLE;
    iterator->base.flag = XU_ITERATOR_FLAG_ITEM_VAL;
    iterator->base.op   = &op;
    iterator->items     = items;
    iterator->count     = count;

    // ok
    return (xu_iterator_ref_t)iterator;
}
xu_iterator_ref_t xu_array_iterator_init_istr(xu_array_iterator_ref_t iterator, xu_char_t** items, xu_size_t count)
{
    // check
    xu_assert(iterator && items && count);

    // init operation
    static xu_iterator_op_t op = {xu_array_iterator_ptr_size,
                                  xu_array_iterator_ptr_head,
                                  xu_null,
                                  xu_array_iterator_ptr_tail,
                                  xu_array_iterator_ptr_prev,
                                  xu_array_iterator_ptr_next,
                                  xu_array_iterator_ptr_item,
                                  xu_array_iterator_istr_comp,
                                  xu_array_iterator_ptr_copy,
                                  xu_null,
                                  xu_null};

    // init iterator
    iterator->base.priv = xu_null;
    iterator->base.step = sizeof(xu_char_t const*);
    iterator->base.mode =
        XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE | XU_ITERATOR_MODE_RACCESS | XU_ITERATOR_MODE_MUTABLE;
    iterator->base.flag = XU_ITERATOR_FLAG_ITEM_VAL;
    iterator->base.op   = &op;
    iterator->items     = items;
    iterator->count     = count;

    // ok
    return (xu_iterator_ref_t)iterator;
}
xu_iterator_ref_t xu_array_iterator_init_size(xu_array_iterator_ref_t iterator, xu_size_t* items, xu_size_t count)
{
    return xu_array_iterator_init_ptr(iterator, (xu_pointer_t*)items, count);
}
xu_iterator_ref_t xu_array_iterator_init_long(xu_array_iterator_ref_t iterator, xu_long_t* items, xu_size_t count)
{

    // check
    xu_assert(iterator && items && count);

    // init operation
    static xu_iterator_op_t op = {xu_array_iterator_ptr_size,
                                  xu_array_iterator_ptr_head,
                                  xu_null,
                                  xu_array_iterator_ptr_tail,
                                  xu_array_iterator_ptr_prev,
                                  xu_array_iterator_ptr_next,
                                  xu_array_iterator_ptr_item,
                                  xu_array_iterator_long_comp,
                                  xu_array_iterator_ptr_copy,
                                  xu_null,
                                  xu_null};

    // init iterator
    iterator->base.priv = xu_null;
    iterator->base.step = sizeof(xu_long_t);
    iterator->base.mode =
        XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE | XU_ITERATOR_MODE_RACCESS | XU_ITERATOR_MODE_MUTABLE;
    iterator->base.flag = XU_ITERATOR_FLAG_ITEM_VAL;
    iterator->base.op   = &op;
    iterator->items     = items;
    iterator->count     = count;

    // ok
    return (xu_iterator_ref_t)iterator;
}
