#include "xutil/container/element.h"
#include "xutil/container/element/hash.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/macros/assert.h"

/* *******************************************************
 * implementation
 */

static xu_size_t xu_element_size_hash(xu_element_ref_t element, xu_cpointer_t data, xu_size_t mask, xu_size_t index)
{
#if XU_CPU_BIT64
    xu_assert_static(sizeof(xu_size_t) == sizeof(xu_uint64_t));
    return xu_element_hash_uint64((xu_uint64_t)data, mask, index);
#else
    xu_assert_static(sizeof(xu_size_t) == sizeof(xu_uint32_t));
    return xu_element_hash_uint32((xu_uint32_t)data, mask, index);
#endif
}
static xu_long_t xu_element_size_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    // compare it
    return ((xu_size_t)ldata < (xu_size_t)rdata) ? -1 : ((xu_size_t)ldata > (xu_size_t)rdata);
}
static xu_pointer_t xu_element_size_data(xu_element_ref_t element, xu_cpointer_t buff)
{
    // check
    xu_assert_and_check_return_val(buff, xu_null);

    // the element data
    return (xu_pointer_t) * ((xu_size_t*)buff);
}
static xu_char_t const* xu_element_size_cstr(xu_element_ref_t element, xu_cpointer_t data, xu_char_t* cstr,
                                             xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(element && cstr, "");

    // format string
    xu_long_t n = xu_snprintf(cstr, maxn, "%lu", (xu_size_t)data);
    if (n >= 0 && n < (xu_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (xu_char_t const*)cstr;
}
static xu_void_t xu_element_size_free(xu_element_ref_t element, xu_pointer_t buff)
{
    // check
    xu_assert_and_check_return(buff);

    // clear
    *((xu_size_t*)buff) = 0;
}
static xu_void_t xu_element_size_copy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(buff);

    // copy element
    *((xu_size_t*)buff) = (xu_size_t)data;
}
static xu_void_t xu_element_size_nfree(xu_element_ref_t element, xu_pointer_t buff, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buff);

    // clear elements
    if (size) xu_memset(buff, 0, size * sizeof(xu_size_t));
}
static xu_void_t xu_element_size_ncopy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buff);

    // copy elements
    if (size) xu_memset_ptr(buff, data, size);
}

/* *******************************************************
 * interfaces
 */

xu_element_t xu_element_size()
{
    // init element
    xu_element_t element = {0};
    element.type         = XU_ELEMENT_TYPE_SIZE;
    element.flag         = 0;
    element.hash         = xu_element_size_hash;
    element.comp         = xu_element_size_comp;
    element.data         = xu_element_size_data;
    element.cstr         = xu_element_size_cstr;
    element.free         = xu_element_size_free;
    element.dupl         = xu_element_size_copy;
    element.repl         = xu_element_size_copy;
    element.copy         = xu_element_size_copy;
    element.nfree        = xu_element_size_nfree;
    element.ndupl        = xu_element_size_ncopy;
    element.nrepl        = xu_element_size_ncopy;
    element.ncopy        = xu_element_size_ncopy;
    element.size         = sizeof(xu_size_t);

    // ok?
    return element;
}
