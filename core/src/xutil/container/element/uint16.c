#include "xutil/container/element.h"
#include "xutil/utils/utils.h"
#include "xutil/container/element/hash.h"
#include "xutil/prefix.h"

/* *******************************************************
 * implementation
 */
static xu_size_t xu_element_uint16_hash(xu_element_ref_t element, xu_cpointer_t data, xu_size_t mask, xu_size_t index)
{
    return xu_element_hash_uint16(xu_p2u16(data), mask, index);
}
static xu_long_t xu_element_uint16_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    // compare it
    return ((xu_p2u16(ldata) < xu_p2u16(rdata)) ? -1 : (xu_p2u16(ldata) > xu_p2u16(rdata)));
}
static xu_pointer_t xu_element_uint16_data(xu_element_ref_t element, xu_cpointer_t buff)
{
    // check
    xu_assert_and_check_return_val(buff, xu_null);

    // the element data
    return xu_u2p(*((xu_uint16_t*)buff));
}
static xu_char_t const* xu_element_uint16_cstr(xu_element_ref_t element, xu_cpointer_t data, xu_char_t* cstr,
                                               xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(element && cstr, "");

    // format string
    xu_long_t n = xu_snprintf(cstr, maxn, "%u", (xu_uint16_t)(xu_size_t)data);
    if (n >= 0 && n < (xu_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (xu_char_t const*)cstr;
}
static xu_void_t xu_element_uint16_free(xu_element_ref_t element, xu_pointer_t buff)
{
    // check
    xu_assert_and_check_return(buff);

    // clear
    *((xu_uint16_t*)buff) = 0;
}
static xu_void_t xu_element_uint16_copy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(buff);

    // copy element
    *((xu_uint16_t*)buff) = xu_p2u16(data);
}
static xu_void_t xu_element_uint16_nfree(xu_element_ref_t element, xu_pointer_t buff, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buff);

    // clear elements
    if (size) xu_memset(buff, 0, size * sizeof(xu_uint16_t));
}
static xu_void_t xu_element_uint16_ncopy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data,
                                         xu_size_t size)
{
    // check
    xu_assert_and_check_return(buff);

    // copy elements
    xu_memset_u16(buff, xu_p2u16(data), size);
}

/* *******************************************************
 * interfaces
 */
xu_element_t xu_element_uint16()
{
    // init element
    xu_element_t element = {0};
    element.type         = XU_ELEMENT_TYPE_UINT16;
    element.flag         = 0;
    element.hash         = xu_element_uint16_hash;
    element.comp         = xu_element_uint16_comp;
    element.data         = xu_element_uint16_data;
    element.cstr         = xu_element_uint16_cstr;
    element.free         = xu_element_uint16_free;
    element.dupl         = xu_element_uint16_copy;
    element.repl         = xu_element_uint16_copy;
    element.copy         = xu_element_uint16_copy;
    element.nfree        = xu_element_uint16_nfree;
    element.ndupl        = xu_element_uint16_ncopy;
    element.nrepl        = xu_element_uint16_ncopy;
    element.ncopy        = xu_element_uint16_ncopy;
    element.size         = sizeof(xu_uint16_t);

    // ok?
    return element;
}
