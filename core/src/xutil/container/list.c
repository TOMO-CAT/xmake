#include "xutil/container/list.h"
#include "xutil/container/list_entry.h"
#include "xutil/macros/malloc.h"
#include "xutil/memory/fixed_pool.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the self grow
#ifdef __xu_small__
#    define XU_LIST_GROW (128)
#else
#    define XU_LIST_GROW (256)
#endif

// the self maxn
#ifdef __xu_small__
#    define XU_LIST_MAXN (1 << 16)
#else
#    define XU_LIST_MAXN (1 << 30)
#endif

/* *******************************************************
 * types
 * *******************************************************
 */

// the self type
typedef struct __xu_list_t
{
    // the itor
    xu_iterator_t itor;

    // the pool
    xu_fixed_pool_ref_t pool;

    // the head
    xu_list_entry_head_t head;

    // the element
    xu_element_t element;

} xu_list_t;

/* *******************************************************
 * private implementation
 * *******************************************************
 */

static xu_size_t xu_list_itor_size(xu_iterator_ref_t iterator)
{
    // the size
    return xu_list_size((xu_list_ref_t)iterator);
}
static xu_size_t xu_list_itor_head(xu_iterator_ref_t iterator)
{
    // check
    xu_list_t* list = (xu_list_t*)iterator;
    xu_assert(list);

    // head
    return (xu_size_t)xu_list_entry_head(&list->head);
}
static xu_size_t xu_list_itor_last(xu_iterator_ref_t iterator)
{
    // check
    xu_list_t* list = (xu_list_t*)iterator;
    xu_assert(list);

    // last
    return (xu_size_t)xu_list_entry_last(&list->head);
}
static xu_size_t xu_list_itor_tail(xu_iterator_ref_t iterator)
{
    // check
    xu_list_t* list = (xu_list_t*)iterator;
    xu_assert(list);

    // tail
    return (xu_size_t)xu_list_entry_tail(&list->head);
}
static xu_size_t xu_list_itor_next(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(itor);

    // next
    return (xu_size_t)xu_list_entry_next((xu_list_entry_t*)itor);
}
static xu_size_t xu_list_itor_prev(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_assert(itor);

    // prev
    return (xu_size_t)xu_list_entry_prev((xu_list_entry_t*)itor);
}
static xu_pointer_t xu_list_itor_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_list_t* list = (xu_list_t*)iterator;
    xu_assert(list && itor);

    // data
    return list->element.data(&list->element, (xu_cpointer_t)(((xu_list_entry_t*)itor) + 1));
}
static xu_void_t xu_list_itor_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    // check
    xu_list_t* list = (xu_list_t*)iterator;
    xu_assert(list && itor);

    // copy
    list->element.copy(&list->element, (xu_pointer_t)(((xu_list_entry_t*)itor) + 1), item);
}
static xu_long_t xu_list_itor_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    // check
    xu_list_t* list = (xu_list_t*)iterator;
    xu_assert(list && list->element.comp);

    // comp
    return list->element.comp(&list->element, litem, ritem);
}
static xu_void_t xu_list_itor_remove(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // remove it
    xu_list_remove((xu_list_ref_t)iterator, itor);
}
static xu_void_t xu_list_itor_nremove(xu_iterator_ref_t iterator, xu_size_t prev, xu_size_t next, xu_size_t size)
{
    // no size?
    xu_check_return(size);

    // the self size
    xu_size_t list_size = xu_list_size((xu_list_ref_t)iterator);
    xu_check_return(list_size);

    // limit size
    if (size > list_size) size = list_size;

    // remove the body items
    if (prev)
    {
        xu_size_t itor = xu_iterator_next((xu_list_ref_t)iterator, prev);
        while (itor != next && size--)
            itor = xu_list_remove((xu_list_ref_t)iterator, itor);
    }
    // remove the head items
    else
    {
        while (size--)
            xu_list_remove_head((xu_list_ref_t)iterator);
    }
}
static xu_void_t xu_list_item_exit(xu_pointer_t data, xu_cpointer_t priv)
{
    // check
    xu_list_t* list = (xu_list_t*)priv;
    xu_assert_and_check_return(list);

    // free data
    if (list->element.free) list->element.free(&list->element, (xu_pointer_t)(((xu_list_entry_t*)data) + 1));
}

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_list_ref_t xu_list_init(xu_size_t grow, xu_element_t element)
{
    // check
    xu_assert_and_check_return_val(element.size && element.data && element.dupl && element.repl, xu_null);

    // done
    xu_bool_t  ok   = xu_false;
    xu_list_t* list = xu_null;
    do
    {
        // using the default grow
        if (!grow) grow = XU_LIST_GROW;

        // make self
        list = xu_malloc0_type(xu_list_t);
        xu_assert_and_check_break(list);

        // init element
        list->element = element;

        // init operation
        static xu_iterator_op_t op = {xu_list_itor_size, xu_list_itor_head,   xu_list_itor_last,   xu_list_itor_tail,
                                      xu_list_itor_prev, xu_list_itor_next,   xu_list_itor_item,   xu_list_itor_comp,
                                      xu_list_itor_copy, xu_list_itor_remove, xu_list_itor_nremove};

        // init iterator
        list->itor.priv = xu_null;
        list->itor.step = element.size;
        list->itor.mode = XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE;
        list->itor.op   = &op;
        if (element.type == XU_ELEMENT_TYPE_MEM) list->itor.flag = XU_ITERATOR_FLAG_ITEM_REF;

        // init pool, item = entry + data
        list->pool = xu_fixed_pool_init(xu_null, grow, sizeof(xu_list_entry_t) + element.size, xu_null,
                                        xu_list_item_exit, (xu_cpointer_t)list);
        xu_assert_and_check_break(list->pool);

        // init head
        xu_list_entry_init_(&list->head, 0, sizeof(xu_list_entry_t) + element.size, xu_null);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (list) xu_list_exit((xu_list_ref_t)list);
        list = xu_null;
    }

    // ok?
    return (xu_list_ref_t)list;
}
xu_void_t xu_list_exit(xu_list_ref_t self)
{
    // check
    xu_list_t* list = (xu_list_t*)self;
    xu_assert_and_check_return(list);

    // clear data
    xu_list_clear((xu_list_ref_t)list);

    // exit pool
    if (list->pool) xu_fixed_pool_exit(list->pool);

    // exit it
    xu_free(list);
}
xu_void_t xu_list_clear(xu_list_ref_t self)
{
    // check
    xu_list_t* list = (xu_list_t*)self;
    xu_assert_and_check_return(list);

    // clear pool
    if (list->pool) xu_fixed_pool_clear(list->pool);

    // clear head
    xu_list_entry_clear(&list->head);
}
xu_pointer_t xu_list_head(xu_list_ref_t self) { return xu_iterator_item(self, xu_iterator_head(self)); }
xu_pointer_t xu_list_last(xu_list_ref_t self) { return xu_iterator_item(self, xu_iterator_last(self)); }
xu_size_t    xu_list_size(xu_list_ref_t self)
{
    // check
    xu_list_t* list = (xu_list_t*)self;
    xu_assert_and_check_return_val(list && list->pool, 0);
    xu_assert(xu_list_entry_size(&list->head) == xu_fixed_pool_size(list->pool));

    // the size
    return xu_list_entry_size(&list->head);
}
xu_size_t xu_list_maxn(xu_list_ref_t self)
{
    // the item maxn
    return XU_LIST_MAXN;
}
xu_size_t xu_list_insert_prev(xu_list_ref_t self, xu_size_t itor, xu_cpointer_t data)
{
    // check
    xu_list_t* list = (xu_list_t*)self;
    xu_assert_and_check_return_val(list && list->element.dupl && list->pool, 0);

    // full?
    xu_assert_and_check_return_val(xu_list_size(self) < xu_list_maxn(self), xu_iterator_tail(self));

    // the node
    xu_list_entry_ref_t node = (xu_list_entry_ref_t)itor;
    xu_assert_and_check_return_val(node, xu_iterator_tail(self));

    // make entry
    xu_list_entry_ref_t entry = (xu_list_entry_ref_t)xu_fixed_pool_malloc(list->pool);
    xu_assert_and_check_return_val(entry, xu_iterator_tail(self));

    // init entry data
    list->element.dupl(&list->element, (xu_pointer_t)(((xu_list_entry_t*)entry) + 1), data);

    // insert it
    xu_list_entry_insert_prev(&list->head, node, entry);

    // ok
    return (xu_size_t)entry;
}
xu_size_t xu_list_insert_next(xu_list_ref_t self, xu_size_t itor, xu_cpointer_t data)
{
    return xu_list_insert_prev(self, xu_iterator_next(self, itor), data);
}
xu_size_t xu_list_insert_head(xu_list_ref_t self, xu_cpointer_t data)
{
    return xu_list_insert_prev(self, xu_iterator_head(self), data);
}
xu_size_t xu_list_insert_tail(xu_list_ref_t self, xu_cpointer_t data)
{
    return xu_list_insert_prev(self, xu_iterator_tail(self), data);
}
xu_void_t xu_list_replace(xu_list_ref_t self, xu_size_t itor, xu_cpointer_t data)
{
    // check
    xu_list_t* list = (xu_list_t*)self;
    xu_assert_and_check_return(list && list->element.repl && itor);

    // the node
    xu_list_entry_ref_t node = (xu_list_entry_ref_t)itor;
    xu_assert_and_check_return(node);

    // replace data
    list->element.repl(&list->element, (xu_pointer_t)(((xu_list_entry_t*)node) + 1), data);
}
xu_void_t xu_list_replace_head(xu_list_ref_t self, xu_cpointer_t data)
{
    xu_list_replace(self, xu_iterator_head(self), data);
}
xu_void_t xu_list_replace_last(xu_list_ref_t self, xu_cpointer_t data)
{
    xu_list_replace(self, xu_iterator_last(self), data);
}
xu_size_t xu_list_remove(xu_list_ref_t self, xu_size_t itor)
{
    // check
    xu_list_t* list = (xu_list_t*)self;
    xu_assert_and_check_return_val(list && list->pool && itor, 0);

    // the node
    xu_list_entry_ref_t node = (xu_list_entry_ref_t)itor;
    xu_assert_and_check_return_val(node, xu_iterator_tail(self));

    // the next node
    xu_list_entry_ref_t next = xu_list_entry_next(node);

    // remove node
    xu_list_entry_remove(&list->head, node);

    // free node
    xu_fixed_pool_free(list->pool, node);

    // the next node
    return (xu_size_t)next;
}
xu_void_t xu_list_remove_head(xu_list_ref_t self) { xu_list_remove(self, xu_iterator_head(self)); }
xu_void_t xu_list_remove_last(xu_list_ref_t self) { xu_list_remove(self, xu_iterator_last(self)); }
xu_void_t xu_list_moveto_prev(xu_list_ref_t self, xu_size_t itor, xu_size_t move)
{
    // check
    xu_list_t* list = (xu_list_t*)self;
    xu_assert_and_check_return(list && list->pool && move);

    // the node
    xu_list_entry_ref_t node = (xu_list_entry_ref_t)itor;
    xu_assert_and_check_return(node);

    // the entry
    xu_list_entry_ref_t entry = (xu_list_entry_ref_t)move;
    xu_assert_and_check_return(entry);

    // move to the prev node
    xu_list_entry_moveto_prev(&list->head, node, entry);
}
xu_void_t xu_list_moveto_next(xu_list_ref_t self, xu_size_t itor, xu_size_t move)
{
    xu_list_moveto_prev(self, xu_iterator_next(self, itor), move);
}
xu_void_t xu_list_moveto_head(xu_list_ref_t self, xu_size_t move)
{
    xu_list_moveto_prev(self, xu_iterator_head(self), move);
}
xu_void_t xu_list_moveto_tail(xu_list_ref_t self, xu_size_t move)
{
    xu_list_moveto_prev(self, xu_iterator_tail(self), move);
}
#ifdef __xu_debug__
xu_void_t xu_list_dump(xu_list_ref_t self)
{
    // check
    xu_list_t* list = (xu_list_t*)self;
    xu_assert_and_check_return(list);

    // trace
    xu_trace_i("self: size: %lu", xu_list_size(self));

    // done
    xu_char_t cstr[4096];
    xu_for_all(xu_pointer_t, data, self)
    {
        // trace
        if (list->element.cstr)
        {
            xu_trace_i("    %s", list->element.cstr(&list->element, data, cstr, sizeof(cstr)));
        }
        else
        {
            xu_trace_i("    %p", data);
        }
    }
}
#endif
