#include "xutil/container/element.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */

static xu_size_t xu_element_true_hash(xu_element_ref_t element, xu_cpointer_t data, xu_size_t size, xu_size_t index)
{
    return 0;
}
static xu_long_t xu_element_true_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    // always be equal
    return 0;
}
static xu_pointer_t xu_element_true_data(xu_element_ref_t element, xu_cpointer_t buff)
{
    // the element data
    return (xu_pointer_t)xu_true;
}
static xu_char_t const* xu_element_true_cstr(xu_element_ref_t element, xu_cpointer_t data, xu_char_t* cstr,
                                             xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(element && cstr && maxn, "");

    // format string
    xu_strlcpy(cstr, "true", maxn);

    // ok?
    return (xu_char_t const*)cstr;
}
static xu_void_t xu_element_true_free(xu_element_ref_t element, xu_pointer_t buff) {}
static xu_void_t xu_element_true_nfree(xu_element_ref_t element, xu_pointer_t buff, xu_size_t size) {}
static xu_void_t xu_element_true_repl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    xu_assert((xu_bool_t)(xu_size_t)data == xu_true);
}
static xu_void_t xu_element_true_nrepl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    xu_assert((xu_bool_t)(xu_size_t)data == xu_true);
}

/* *******************************************************
 * interfaces
 * *******************************************************
 */

xu_element_t xu_element_true()
{
    // init element
    xu_element_t element = {0};
    element.type         = XU_ELEMENT_TYPE_TRUE;
    element.flag         = 0;
    element.hash         = xu_element_true_hash;
    element.comp         = xu_element_true_comp;
    element.data         = xu_element_true_data;
    element.cstr         = xu_element_true_cstr;
    element.free         = xu_element_true_free;
    element.dupl         = xu_element_true_repl;
    element.repl         = xu_element_true_repl;
    element.copy         = xu_element_true_repl;
    element.nfree        = xu_element_true_nfree;
    element.ndupl        = xu_element_true_nrepl;
    element.nrepl        = xu_element_true_nrepl;
    element.ncopy        = xu_element_true_nrepl;
    element.size         = 0;

    // ok?
    return element;
}
