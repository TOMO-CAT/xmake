#include "xutil/container/element.h"
#include "xutil/container/element/hash.h"
#include "xutil/hash/adler32.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */
static xu_size_t xu_element_mem_hash(xu_element_ref_t element, xu_cpointer_t data, xu_size_t mask, xu_size_t index)
{
    return xu_element_hash_data((xu_byte_t const*)data, element->size, mask, index);
}
static xu_long_t xu_element_mem_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    // check
    xu_assert_and_check_return_val(element && element->size && ldata && rdata, 0);

    // comp
    return xu_memcmp(ldata, rdata, element->size);
}
static xu_pointer_t xu_element_mem_data(xu_element_ref_t element, xu_cpointer_t buff)
{
    // check
    xu_assert_and_check_return_val(element && buff, xu_null);

    // the element data
    return (xu_pointer_t)buff;
}
static xu_char_t const* xu_element_mem_cstr(xu_element_ref_t element, xu_cpointer_t data, xu_char_t* cstr,
                                            xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(element && element->size && cstr && data, xu_null);

    // make info
    xu_long_t size = xu_snprintf(cstr, maxn, "0x%x", xu_adler32_make((xu_byte_t const*)data, element->size, 0));
    if (size >= 0) cstr[size] = '\0';

    // ok?
    return cstr;
}
static xu_void_t xu_element_mem_free(xu_element_ref_t element, xu_pointer_t buff)
{
    // check
    xu_assert_and_check_return(element && element->size && buff);

    // clear it
    xu_memset(buff, 0, element->size);
}
static xu_void_t xu_element_mem_dupl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(element && element->size && buff && data);

    // copy element
    xu_memcpy(buff, data, element->size);
}
static xu_void_t xu_element_mem_repl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(element && element->size && buff && data);

    // the free is hooked? free it
    if (element->free != xu_element_mem_free && element->free) element->free(element, buff);

    // copy element
    xu_memcpy(buff, data, element->size);
}
static xu_void_t xu_element_mem_copy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data)
{
    // check
    xu_assert_and_check_return(element && element->size && buff && data);

    // copy element
    xu_memcpy(buff, data, element->size);
}
static xu_void_t xu_element_mem_nfree(xu_element_ref_t element, xu_pointer_t buff, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && element->size && buff);

    // the free is hooked? free it
    if (element->free != xu_element_mem_free && element->free)
    {
        xu_size_t n = size;
        while (n--)
            element->free(element, (xu_byte_t*)buff + n * element->size);
    }

    // clear
    if (size) xu_memset(buff, 0, size * element->size);
}
static xu_void_t xu_element_mem_ndupl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && element->size && buff && data);

    // copy elements
    if (element->ncopy) element->ncopy(element, buff, data, size);
}
static xu_void_t xu_element_mem_nrepl(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && element->size && buff && data);

    // free elements
    if (element->nfree) element->nfree(element, buff, size);

    // copy elements
    if (element->ncopy) element->ncopy(element, buff, data, size);
}
static xu_void_t xu_element_mem_ncopy(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(element && element->size && buff && data);

    // copy elements
    while (size--)
        xu_memcpy((xu_byte_t*)buff + size * element->size, data, element->size);
}

/* *******************************************************
 * interfaces
 * *******************************************************
 */

xu_element_t xu_element_mem(xu_size_t size, xu_element_free_func_t free, xu_cpointer_t priv)
{
    // check
    xu_assert(size < XU_MAXU16);

    // init element
    xu_element_t element = {0};
    element.type         = XU_ELEMENT_TYPE_MEM;
    element.flag         = 0;
    element.hash         = xu_element_mem_hash;
    element.comp         = xu_element_mem_comp;
    element.data         = xu_element_mem_data;
    element.cstr         = xu_element_mem_cstr;
    element.free         = free ? free : xu_element_mem_free;
    element.dupl         = xu_element_mem_dupl;
    element.repl         = xu_element_mem_repl;
    element.copy         = xu_element_mem_copy;
    element.nfree        = xu_element_mem_nfree;
    element.ndupl        = xu_element_mem_ndupl;
    element.nrepl        = xu_element_mem_nrepl;
    element.ncopy        = xu_element_mem_ncopy;
    element.size         = (xu_uint16_t)size;
    element.priv         = priv;

    // ok?
    return element;
}
