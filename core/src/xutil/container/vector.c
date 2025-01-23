#include "xutil/container/vector.h"

/* *******************************************************
 *  macros
 */

// the vector grow
#ifdef __xu_small__
#    define XU_VECTOR_GROW (128)
#else
#    define XU_VECTOR_GROW (256)
#endif

// the vector maxn
#ifdef __xu_small__
#    define XU_VECTOR_MAXN (1 << 16)
#else
#    define XU_VECTOR_MAXN (1 << 30)
#endif

/* *******************************************************
 *  types
 */

// the vector type
typedef struct __xu_vector_t
{
    // the itor
    xu_iterator_t itor;

    // the data
    xu_byte_t* data;

    // the size
    xu_size_t size;

    // the grow
    xu_size_t grow;

    // the maxn
    xu_size_t maxn;

    // the element
    xu_element_t element;

} xu_vector_t;

/* *******************************************************
 *  private implementation
 */

static xu_size_t xu_vector_itor_size(xu_iterator_ref_t iterator)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert(vector);

    // size
    return vector->size;
}
static xu_size_t xu_vector_itor_head(xu_iterator_ref_t iterator)
{
    // head
    return 0;
}
static xu_size_t xu_vector_itor_last(xu_iterator_ref_t iterator)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert(vector);

    // last
    return vector->size ? vector->size - 1 : 0;
}
static xu_size_t xu_vector_itor_tail(xu_iterator_ref_t iterator)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert(vector);

    // tail
    return vector->size;
}
static xu_size_t xu_vector_itor_next(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert(vector);
    xu_assert_and_check_return_val(itor < vector->size, vector->size);

    // next
    return itor + 1;
}
static xu_size_t xu_vector_itor_prev(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert(vector);
    xu_assert_and_check_return_val(itor && itor <= vector->size, 0);

    // prev
    return itor - 1;
}
static xu_pointer_t xu_vector_itor_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert_and_check_return_val(vector && itor < vector->size, xu_null);

    // data
    return vector->element.data(&vector->element, vector->data + itor * iterator->step);
}
static xu_void_t xu_vector_itor_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert(vector);

    // copy
    vector->element.copy(&vector->element, vector->data + itor * iterator->step, item);
}
static xu_long_t xu_vector_itor_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert(vector && vector->element.comp);

    // comp
    return vector->element.comp(&vector->element, litem, ritem);
}
static xu_void_t xu_vector_itor_remove(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // remove it
    xu_vector_remove((xu_vector_ref_t)iterator, itor);
}
static xu_void_t xu_vector_itor_nremove(xu_iterator_ref_t iterator, xu_size_t prev, xu_size_t next, xu_size_t size)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)iterator;
    xu_assert(vector);

    // remove the items
    if (size) xu_vector_nremove((xu_vector_ref_t)iterator, prev != vector->size ? prev + 1 : 0, size);
}

/* *******************************************************
 *  implementation
 */

xu_vector_ref_t xu_vector_init(xu_size_t grow, xu_element_t element)
{
    // check
    xu_assert_and_check_return_val(
        element.size && element.data && element.dupl && element.repl && element.ndupl && element.nrepl, xu_null);

    // done
    xu_bool_t    ok     = xu_false;
    xu_vector_t* vector = xu_null;
    do
    {
        // using the default grow
        if (!grow) grow = XU_VECTOR_GROW;

        // make vector
        vector = xu_malloc0_type(xu_vector_t);
        xu_assert_and_check_break(vector);

        // init vector
        vector->size    = 0;
        vector->grow    = grow;
        vector->maxn    = grow;
        vector->element = element;
        xu_assert_and_check_break(vector->maxn < XU_VECTOR_MAXN);

        // init operation
        static xu_iterator_op_t op = {xu_vector_itor_size,   xu_vector_itor_head,   xu_vector_itor_last,
                                      xu_vector_itor_tail,   xu_vector_itor_prev,   xu_vector_itor_next,
                                      xu_vector_itor_item,   xu_vector_itor_comp,   xu_vector_itor_copy,
                                      xu_vector_itor_remove, xu_vector_itor_nremove};

        // init iterator
        vector->itor.priv = xu_null;
        vector->itor.step = element.size;
        vector->itor.mode =
            XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_REVERSE | XU_ITERATOR_MODE_RACCESS | XU_ITERATOR_MODE_MUTABLE;
        vector->itor.op = &op;
        if (element.type == XU_ELEMENT_TYPE_MEM) vector->itor.flag = XU_ITERATOR_FLAG_ITEM_REF;

        // make data
        vector->data = (xu_byte_t*)xu_nalloc0(vector->maxn, element.size);
        xu_assert_and_check_break(vector->data);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (vector) xu_vector_exit((xu_vector_ref_t)vector);
        vector = xu_null;
    }
    return (xu_vector_ref_t)vector;
}
xu_void_t xu_vector_exit(xu_vector_ref_t self)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector);

    // clear data
    xu_vector_clear(self);

    // free data
    if (vector->data) xu_free(vector->data);
    vector->data = xu_null;

    // free it
    xu_free(vector);
}
xu_void_t xu_vector_clear(xu_vector_ref_t self)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector);

    // free data
    if (vector->element.nfree) vector->element.nfree(&vector->element, vector->data, vector->size);

    // reset size
    vector->size = 0;
}
xu_void_t xu_vector_copy(xu_vector_ref_t self, xu_vector_ref_t copy)
{
    // check
    xu_vector_t*       vector      = (xu_vector_t*)self;
    xu_vector_t const* vector_copy = (xu_vector_t const*)copy;
    xu_assert_and_check_return(vector && vector_copy);

    // check element
    xu_assert_and_check_return(vector->element.type == vector_copy->element.type);
    xu_assert_and_check_return(vector->element.size == vector_copy->element.size);

    // check itor
    xu_assert_and_check_return(vector->itor.step == vector_copy->itor.step);

    // null? clear it
    if (!vector_copy->size)
    {
        xu_vector_clear(self);
        return;
    }

    // resize if small
    if (vector->size < vector_copy->size) xu_vector_resize(self, vector_copy->size);
    xu_assert_and_check_return(vector->data && vector_copy->data && vector->size >= vector_copy->size);

    // copy data
    if (vector_copy->data != vector->data)
        xu_memcpy(vector->data, vector_copy->data, vector_copy->size * vector_copy->element.size);

    // copy size
    vector->size = vector_copy->size;
}
xu_pointer_t xu_vector_data(xu_vector_ref_t self)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return_val(vector, xu_null);

    // data
    return vector->data;
}
xu_pointer_t xu_vector_head(xu_vector_ref_t self) { return xu_iterator_item(self, xu_iterator_head(self)); }
xu_pointer_t xu_vector_last(xu_vector_ref_t self) { return xu_iterator_item(self, xu_iterator_last(self)); }
xu_size_t    xu_vector_size(xu_vector_ref_t self)
{
    // check
    xu_vector_t const* vector = (xu_vector_t const*)self;
    xu_assert_and_check_return_val(vector, 0);

    // size
    return vector->size;
}
xu_size_t xu_vector_grow(xu_vector_ref_t self)
{
    // check
    xu_vector_t const* vector = (xu_vector_t const*)self;
    xu_assert_and_check_return_val(vector, 0);

    // grow
    return vector->grow;
}
xu_size_t xu_vector_maxn(xu_vector_ref_t self)
{
    // check
    xu_vector_t const* vector = (xu_vector_t const*)self;
    xu_assert_and_check_return_val(vector, 0);

    // maxn
    return vector->maxn;
}
xu_bool_t xu_vector_resize(xu_vector_ref_t self, xu_size_t size)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return_val(vector, xu_false);

    // free items if the vector is decreased
    if (size < vector->size)
    {
        // free data
        if (vector->element.nfree)
            vector->element.nfree(&vector->element, vector->data + size * vector->element.size, vector->size - size);
    }

    // resize buffer
    if (size > vector->maxn)
    {
        xu_size_t maxn = xu_align4(size + vector->grow);
        xu_assert_and_check_return_val(maxn < XU_VECTOR_MAXN, xu_false);

        // realloc data
        vector->data = (xu_byte_t*)xu_ralloc(vector->data, maxn * vector->element.size);
        xu_assert_and_check_return_val(vector->data, xu_false);

        // must be align by 4-bytes
        xu_assert_and_check_return_val(!(((xu_size_t)(vector->data)) & 3), xu_false);

        // clear the grow data
        xu_memset(vector->data + vector->size * vector->element.size, 0, (maxn - vector->maxn) * vector->element.size);

        // save maxn
        vector->maxn = maxn;
    }

    // update size
    vector->size = size;
    return xu_true;
}
xu_void_t xu_vector_insert_prev(xu_vector_ref_t self, xu_size_t itor, xu_cpointer_t data)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && vector->data && vector->element.size && itor <= vector->size);

    // save size
    xu_size_t osize = vector->size;

    // grow a item
    if (!xu_vector_resize(self, osize + 1))
    {
        xu_trace_d("vector resize: %u => %u failed", osize, osize + 1);
        return;
    }

    // move items if not at tail
    if (osize != itor)
        xu_memmov(vector->data + (itor + 1) * vector->element.size, vector->data + itor * vector->element.size,
                  (osize - itor) * vector->element.size);

    // save data
    vector->element.dupl(&vector->element, vector->data + itor * vector->element.size, data);
}
xu_void_t xu_vector_insert_next(xu_vector_ref_t self, xu_size_t itor, xu_cpointer_t data)
{
    xu_vector_insert_prev(self, xu_iterator_next(self, itor), data);
}
xu_void_t xu_vector_insert_head(xu_vector_ref_t self, xu_cpointer_t data) { xu_vector_insert_prev(self, 0, data); }
xu_void_t xu_vector_insert_tail(xu_vector_ref_t self, xu_cpointer_t data)
{
    xu_vector_insert_prev(self, xu_vector_size(self), data);
}
xu_void_t xu_vector_ninsert_prev(xu_vector_ref_t self, xu_size_t itor, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && vector->data && size && itor <= vector->size);

    // save size
    xu_size_t osize = vector->size;

    // grow size
    if (!xu_vector_resize(self, osize + size))
    {
        xu_trace_d("vector resize: %u => %u failed", osize, osize + 1);
        return;
    }

    // move items if not at tail
    if (osize != itor)
        xu_memmov(vector->data + (itor + size) * vector->element.size, vector->data + itor * vector->element.size,
                  (osize - itor) * vector->element.size);

    // duplicate data
    vector->element.ndupl(&vector->element, vector->data + itor * vector->element.size, data, size);
}
xu_void_t xu_vector_ninsert_next(xu_vector_ref_t self, xu_size_t itor, xu_cpointer_t data, xu_size_t size)
{
    xu_vector_ninsert_prev(self, xu_iterator_next(self, itor), data, size);
}
xu_void_t xu_vector_ninsert_head(xu_vector_ref_t self, xu_cpointer_t data, xu_size_t size)
{
    xu_vector_ninsert_prev(self, 0, data, size);
}
xu_void_t xu_vector_ninsert_tail(xu_vector_ref_t self, xu_cpointer_t data, xu_size_t size)
{
    xu_vector_ninsert_prev(self, xu_vector_size(self), data, size);
}
xu_void_t xu_vector_replace(xu_vector_ref_t self, xu_size_t itor, xu_cpointer_t data)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && vector->data && itor <= vector->size);

    // replace data
    vector->element.repl(&vector->element, vector->data + itor * vector->element.size, data);
}
xu_void_t xu_vector_replace_head(xu_vector_ref_t self, xu_cpointer_t data) { xu_vector_replace(self, 0, data); }
xu_void_t xu_vector_replace_last(xu_vector_ref_t self, xu_cpointer_t data)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && vector->size);

    // replace
    xu_vector_replace(self, vector->size - 1, data);
}
xu_void_t xu_vector_nreplace(xu_vector_ref_t self, xu_size_t itor, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && vector->data && vector->size && itor <= vector->size && size);

    // strip size
    if (itor + size > vector->size) size = vector->size - itor;

    // replace data
    vector->element.nrepl(&vector->element, vector->data + itor * vector->element.size, data, size);
}
xu_void_t xu_vector_nreplace_head(xu_vector_ref_t self, xu_cpointer_t data, xu_size_t size)
{
    xu_vector_nreplace(self, 0, data, size);
}
xu_void_t xu_vector_nreplace_last(xu_vector_ref_t self, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && vector->size && size);

    // replace
    xu_vector_nreplace(self, size >= vector->size ? 0 : vector->size - size, data, size);
}
xu_void_t xu_vector_remove(xu_vector_ref_t self, xu_size_t itor)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && itor < vector->size);

    if (vector->size)
    {
        // do free
        if (vector->element.free) vector->element.free(&vector->element, vector->data + itor * vector->element.size);

        // move data if itor is not last
        if (itor < vector->size - 1)
            xu_memmov(vector->data + itor * vector->element.size, vector->data + (itor + 1) * vector->element.size,
                      (vector->size - itor - 1) * vector->element.size);

        // resize
        vector->size--;
    }
}
xu_void_t xu_vector_remove_head(xu_vector_ref_t self) { xu_vector_remove(self, 0); }
xu_void_t xu_vector_remove_last(xu_vector_ref_t self)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector);

    if (vector->size)
    {
        // do free
        if (vector->element.free)
            vector->element.free(&vector->element, vector->data + (vector->size - 1) * vector->element.size);

        // resize
        vector->size--;
    }
}
xu_void_t xu_vector_nremove(xu_vector_ref_t self, xu_size_t itor, xu_size_t size)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && size && itor < vector->size);

    // clear it
    if (!itor && size >= vector->size)
    {
        xu_vector_clear(self);
        return;
    }

    // strip size
    if (itor + size > vector->size) size = vector->size - itor;

    // compute the left size
    xu_size_t left = vector->size - itor - size;

    // free data
    if (vector->element.nfree)
        vector->element.nfree(&vector->element, vector->data + itor * vector->element.size, size);

    // move the left data
    if (left)
    {
        xu_byte_t* pd = vector->data + itor * vector->element.size;
        xu_byte_t* ps = vector->data + (itor + size) * vector->element.size;
        xu_memmov(pd, ps, left * vector->element.size);
    }

    // update size
    vector->size -= size;
}
xu_void_t xu_vector_nremove_head(xu_vector_ref_t self, xu_size_t size)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && size);

    // clear it
    if (size >= vector->size)
    {
        xu_vector_clear(self);
        return;
    }

    // remove head
    xu_vector_nremove(self, 0, size);
}
xu_void_t xu_vector_nremove_last(xu_vector_ref_t self, xu_size_t size)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector && size);

    // clear it
    if (size >= vector->size)
    {
        xu_vector_clear(self);
        return;
    }

    // remove last
    xu_vector_nremove(self, vector->size - size, size);
}
#ifdef __xu_debug__
xu_void_t xu_vector_dump(xu_vector_ref_t self)
{
    // check
    xu_vector_t* vector = (xu_vector_t*)self;
    xu_assert_and_check_return(vector);

    // trace
    xu_trace_i("vector: size: %lu", xu_vector_size(self));

    // done
    xu_char_t cstr[4096];
    xu_for_all(xu_pointer_t, data, self)
    {
        // trace
        if (vector->element.cstr)
        {
            xu_trace_i("    %s", vector->element.cstr(&vector->element, data, cstr, sizeof(cstr)));
        }
        else
        {
            xu_trace_i("    %p", data);
        }
    }
}
#endif
