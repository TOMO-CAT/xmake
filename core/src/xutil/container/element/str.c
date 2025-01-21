#include "xutil/container/element.h"
#include "xutil/container/element/hash.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */
static xu_size_t xu_element_str_hash(xu_element_ref_t element, xu_cpointer_t data, xu_size_t mask, xu_size_t index)
{
    return xu_element_hash_cstr((xu_char_t const*)data, mask, index);
}
static xu_long_t xu_element_str_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    // check
    xu_assert_and_check_return_val(element && ldata && rdata, 0);

    // compare it
    return element->flag ? xu_strcmp((xu_char_t const*)ldata, (xu_char_t const*)rdata)
                         : xu_stricmp((xu_char_t const*)ldata, (xu_char_t const*)rdata);
}
static xu_pointer_t xu_element_str_data(xu_element_ref_t element, xu_cpointer_t buff)
{
    // check
    xu_assert_and_check_return_val(buff, xu_null);

    // the element data
    return *((xu_pointer_t*)buff);
}
static xu_char_t const* xu_element_str_cstr(xu_element_ref_t element, xu_cpointer_t data, xu_char_t* cstr,
                                            xu_size_t maxn)
{
    // the c-string
    return (xu_char_t const*)data;
}
static xu_void_t xu_element_str_free(xu_element_ref_t element, xu_pointer_t buff)
{
    // check
    xu_assert_and_check_return(element && buff);

    // exists?
    xu_pointer_t cstr = *((xu_pointer_t*)buff);
    if (cstr)
    {
        // free it
        xu_free(cstr);

        // clear it
        *((xu_pointer_t*)buff) = xu_null;
    }
}
static xu_void_t xu_element_str_dupl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(element && buff);

    // duplicate it
    if (data) *((xu_char_t const**)buff) = xu_strdup((xu_char_t const*)data);
    // clear it
    else
        *((xu_char_t const**)buff) = xu_null;
}
static xu_void_t xu_element_str_repl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(element && element->dupl && buff);

#if 0
    // free it
    if (element->free) element->free(element, buff);

    // dupl it
    element->dupl(element, buff, data);
#else
    // replace it
    xu_pointer_t cstr = *((xu_pointer_t*)buff);
    if (cstr && data)
    {
        // attempt to replace it
        xu_char_t*       p = (xu_char_t*)cstr;
        xu_char_t const* q = (xu_char_t const*)data;
        while (*p && *q)
            *p++ = *q++;

        // not enough space?
        if (!*p && *q)
        {
            // the left size
            xu_size_t left = xu_strlen(q);
            xu_assert(left);

            // the copy size
            xu_size_t copy = p - (xu_char_t*)cstr;

            // grow size
            cstr = xu_ralloc(cstr, copy + left + 1);
            xu_assert(cstr);

            // copy the left data
            xu_memcpy((xu_char_t*)cstr + copy, q, left + 1);

            // update the cstr
            *((xu_pointer_t*)buff) = cstr;
        }
        // end
        else
            *p = '\0';
    }
    // duplicate it
    else if (data)
        element->dupl(element, buff, data);
    // free it
    else if (element->free)
        element->free(element, buff);
    // clear it
    else
        *((xu_char_t const**)buff) = xu_null;
#endif
}
static xu_void_t xu_element_str_copy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(buff);

    // copy it
    *((xu_cpointer_t*)buff) = data;
}
static xu_void_t xu_element_str_nfree(xu_element_ref_t element, xu_pointer_t buff, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && buff);

    // free elements
    if (element->free)
    {
        xu_size_t n = size;
        while (n--)
            element->free(element, (xu_byte_t*)buff + n * sizeof(xu_char_t*));
    }

    // clear
    if (size) xu_memset(buff, 0, size * sizeof(xu_char_t*));
}
static xu_void_t xu_element_str_ndupl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && buff);

    // dupl elements
    if (element->dupl)
        while (size--)
            element->dupl(element, (xu_byte_t*)buff + size * sizeof(xu_char_t*), data);
}
static xu_void_t xu_element_str_nrepl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && buff && data);

    // repl elements
    if (element->repl)
        while (size--)
            element->repl(element, (xu_byte_t*)buff + size * sizeof(xu_char_t*), data);
}
static xu_void_t xu_element_str_ncopy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
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

xu_element_t xu_element_str(xu_bool_t bcase)
{
    // init element
    xu_element_t element = {0};
    element.type         = XU_ELEMENT_TYPE_STR;
    element.flag         = !!bcase;
    element.hash         = xu_element_str_hash;
    element.comp         = xu_element_str_comp;
    element.data         = xu_element_str_data;
    element.cstr         = xu_element_str_cstr;
    element.free         = xu_element_str_free;
    element.dupl         = xu_element_str_dupl;
    element.repl         = xu_element_str_repl;
    element.copy         = xu_element_str_copy;
    element.nfree        = xu_element_str_nfree;
    element.ndupl        = xu_element_str_ndupl;
    element.nrepl        = xu_element_str_nrepl;
    element.ncopy        = xu_element_str_ncopy;
    element.size         = sizeof(xu_char_t*);

    // ok?
    return element;
}
