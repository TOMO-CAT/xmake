#include "xutil/libc/string/string.h"
#include "xutil/memory/impl/prefix.h"

#ifndef XU_CONFIG_LIBC_HAVE_STRNCPY
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/strncpy.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/strncpy.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/strncpy.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_LIBC_HAVE_STRNCPY)
static xu_char_t* xu_strncpy_impl(xu_char_t* s1, xu_char_t const* s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, xu_null);

    // copy it
    return strncpy(s1, s2, n);
}
#elif !defined(XU_LIBC_STRING_IMPL_STRNCPY)
static xu_char_t* xu_strncpy_impl(xu_char_t* s1, xu_char_t const* s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, s1);

    // no size or same?
    xu_check_return_val(n && s1 != s2, s1);

    // copy it
    xu_char_t* s = s1;
    while (n)
    {
        if ((*s = *s2)) s2++;
        ++s;
        --n;
    }
    return s1;
}
#endif

/* *******************************************************
 * interfaces
 */

xu_char_t* xu_strncpy(xu_char_t* s1, xu_char_t const* s2, xu_size_t n)
{
    // check
#ifdef __xu_debug__
    {
        // overflow dst?
        xu_strlen(s2);

        // strncpy overflow?
        xu_size_t n1 = xu_pool_data_size(s1);
        if (n1 && n + 1 > n1)
        {
            xu_trace_i("[strncpy]: [overflow]: [%p, %lu] => [%p, %lu]", s2, n, s1, n1);
            xu_backtrace_dump("[strncpy]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s2, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#endif

    // done
    return xu_strncpy_impl(s1, s2, n);
}
