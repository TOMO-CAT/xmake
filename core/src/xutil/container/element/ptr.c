#include "xutil/container/element.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/macros/assert.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */

static xu_long_t xu_element_ptr_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    return (ldata < rdata) ? -1 : (ldata > rdata);
}
static xu_pointer_t xu_element_ptr_data(xu_element_ref_t element, xu_cpointer_t buff)
{
    // check
    xu_assert_and_check_return_val(buff, xu_null);

    // the element data
    return *((xu_pointer_t*)buff);
}
static xu_char_t const* xu_element_ptr_cstr(xu_element_ref_t element, xu_cpointer_t data, xu_char_t* cstr,
                                            xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(cstr, "");

    // format string
    xu_long_t n = xu_snprintf(cstr, maxn - 1, "%p", data);
    if (n >= 0 && n < (xu_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (xu_char_t const*)cstr;
}
static xu_void_t xu_element_ptr_free(xu_element_ref_t element, xu_pointer_t buff)
{
    // check
    xu_assert_and_check_return(buff);

    // clear it
    *((xu_pointer_t*)buff) = xu_null;
}
static xu_void_t xu_element_ptr_repl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(element && buff);

    // the free is hooked? free it
    if (element->free != xu_element_ptr_free && element->free) element->free(element, buff);

    // copy it
    *((xu_cpointer_t*)buff) = data;
}
static xu_void_t xu_element_ptr_copy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(buff);

    // copy it
    *((xu_cpointer_t*)buff) = data;
}
static xu_void_t xu_element_ptr_dupl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(buff);

    // dupl it
    *((xu_cpointer_t*)buff) = data;
}
static xu_void_t xu_element_ptr_nfree(xu_element_ref_t element, xu_pointer_t buff, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && buff);

    // the free is hooked? free it
    if (element->free != xu_element_ptr_free && element->free)
    {
        xu_size_t n = size;
        while (n--)
            element->free(element, (xu_byte_t*)buff + n * sizeof(xu_pointer_t));
    }

    // clear
    if (size) xu_memset(buff, 0, size * sizeof(xu_pointer_t));
}
static xu_void_t xu_element_ptr_ndupl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buff);

    // copy elements
    if (element->ncopy) element->ncopy(element, buff, data, size);
}
static xu_void_t xu_element_ptr_nrepl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && buff);

    // free element
    if (element->nfree) element->nfree(element, buff, size);

    // copy elements
    if (element->ncopy) element->ncopy(element, buff, data, size);
}
static xu_void_t xu_element_ptr_ncopy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buff);

    // fill elements
    if (size) xu_memset_ptr(buff, data, size);
}

/* *******************************************************
 * interfaces
 * *******************************************************
 */

xu_element_t xu_element_ptr(xu_element_free_func_t free, xu_cpointer_t priv)
{
    // the size element
    xu_element_t element_size = xu_element_size();

    // init element
    xu_element_t element = {0};
    element.type         = XU_ELEMENT_TYPE_PTR;
    element.flag         = 0;
    element.hash         = element_size.hash;
    element.comp         = xu_element_ptr_comp;
    element.data         = xu_element_ptr_data;
    element.cstr         = xu_element_ptr_cstr;
    element.free         = free ? free : xu_element_ptr_free;
    element.dupl         = xu_element_ptr_dupl;
    element.repl         = xu_element_ptr_repl;
    element.copy         = xu_element_ptr_copy;
    element.nfree        = xu_element_ptr_nfree;
    element.ndupl        = xu_element_ptr_ndupl;
    element.nrepl        = xu_element_ptr_nrepl;
    element.ncopy        = xu_element_ptr_ncopy;
    element.priv         = priv;
    element.size         = sizeof(xu_pointer_t);

    // ok?
    return element;
}
