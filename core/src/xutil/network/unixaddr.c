/* *******************************************************
 *  trace
 */
#define XU_TRACE_MODULE_NAME "unixaddr"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/network/unixaddr.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

/* *******************************************************
 *  interfaces
 */
xu_void_t xu_unixaddr_clear(xu_unixaddr_ref_t unixaddr)
{
    // check
    xu_assert_and_check_return(unixaddr);

    // clear it
    xu_memset(unixaddr->path, 0, sizeof(unixaddr->path));
}
xu_bool_t xu_unixaddr_is_abstract(xu_unixaddr_ref_t unixaddr)
{
    // check
    xu_assert_and_check_return_val(unixaddr, xu_false);

    // is abstract?
    return unixaddr->is_abstract;
}
xu_bool_t xu_unixaddr_is_equal(xu_unixaddr_ref_t unixaddr, xu_unixaddr_ref_t other)
{
    // check
    xu_assert_and_check_return_val(unixaddr && other, xu_false);

    // is equal?
    return xu_strcmp(unixaddr->path, other->path) == 0;
}
xu_char_t const* xu_unixaddr_cstr(xu_unixaddr_ref_t unixaddr, xu_char_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(unixaddr && data && maxn >= XU_UNIXADDR_CSTR_MAXN, xu_null);

    // make it
    xu_long_t size = xu_snprintf(data, maxn - 1, "%s", unixaddr->path);
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
xu_bool_t xu_unixaddr_cstr_set(xu_unixaddr_ref_t unixaddr, xu_char_t const* cstr, xu_bool_t is_abstract)
{
    // check
    xu_assert_and_check_return_val(cstr, xu_false);

    // set is_abstract
    unixaddr->is_abstract = is_abstract;

    // copy and report
    return xu_strlcpy(unixaddr->path, cstr, XU_UNIXADDR_CSTR_MAXN) < XU_UNIXADDR_CSTR_MAXN;
}
