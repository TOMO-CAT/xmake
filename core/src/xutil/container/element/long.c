#include "xutil/container/element.h"

/* *******************************************************
 * implementation
 */
static xu_long_t xu_element_long_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    // compare it
    return ((xu_long_t)ldata < (xu_long_t)rdata) ? -1 : ((xu_long_t)ldata > (xu_long_t)rdata);
}
static xu_pointer_t xu_element_long_data(xu_element_ref_t element, xu_cpointer_t buff)
{
    // check
    xu_assert_and_check_return_val(buff, xu_null);

    // the element data
    return (xu_pointer_t) * ((xu_long_t*)buff);
}
static xu_char_t const* xu_element_long_cstr(xu_element_ref_t element, xu_cpointer_t data, xu_char_t* cstr,
                                             xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(element && cstr, "");

    // format string
    xu_long_t n = xu_snprintf(cstr, maxn, "%ld", (xu_long_t)data);
    if (n >= 0 && n < (xu_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (xu_char_t const*)cstr;
}
static xu_void_t xu_element_long_free(xu_element_ref_t element, xu_pointer_t buff)
{
    // check
    xu_assert_and_check_return(buff);

    // clear
    *((xu_size_t*)buff) = 0;
}
static xu_void_t xu_element_long_copy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(buff);

    // copy element
    *((xu_long_t*)buff) = (xu_long_t)data;
}
static xu_void_t xu_element_long_nfree(xu_element_ref_t element, xu_pointer_t buff, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buff);

    // clear elements
    if (size) xu_memset(buff, 0, size * sizeof(xu_long_t));
}
static xu_void_t xu_element_long_ncopy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buff);

    // copy elements
    if (size) xu_memset_ptr(buff, data, size);
}

/* *******************************************************
 * interfaces
 */
xu_element_t xu_element_long()
{
    // the size element
    xu_element_t element_size = xu_element_size();

    // init element
    xu_element_t element = {0};
    element.type         = XU_ELEMENT_TYPE_LONG;
    element.flag         = 0;
    element.hash         = element_size.hash;
    element.comp         = xu_element_long_comp;
    element.data         = xu_element_long_data;
    element.cstr         = xu_element_long_cstr;
    element.free         = xu_element_long_free;
    element.dupl         = xu_element_long_copy;
    element.repl         = xu_element_long_copy;
    element.copy         = xu_element_long_copy;
    element.nfree        = xu_element_long_nfree;
    element.ndupl        = xu_element_long_ncopy;
    element.nrepl        = xu_element_long_ncopy;
    element.ncopy        = xu_element_long_ncopy;
    element.size         = sizeof(xu_long_t);

    // ok?
    return element;
}
