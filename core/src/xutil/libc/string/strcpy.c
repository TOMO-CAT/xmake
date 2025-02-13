#include "xutil/libc/string/string.h"
#include "xutil/memory/impl/prefix.h"

#ifndef XU_CONFIG_LIBC_HAVE_STRCPY
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/strcpy.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/strcpy.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/strcpy.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_LIBC_HAVE_STRCPY)
static xu_char_t* xu_strcpy_impl(xu_char_t* s1, xu_char_t const* s2)
{
    xu_assert_and_check_return_val(s1 && s2, xu_null);
    return strcpy(s1, s2);
}
#elif !defined(XU_LIBC_STRING_IMPL_STRCPY)
static xu_char_t* xu_strcpy_impl(xu_char_t* s1, xu_char_t const* s2)
{
    xu_assert_and_check_return_val(s1 && s2, xu_null);

    __xu_register__ xu_char_t* s = s1;
    if (s1 == s2) return s;

#    if 1
    xu_memcpy(s1, s2, xu_strlen(s2) + 1);
#    elif defined(__xu_small__)
    while ((*s++ = *s2++))
    {
    }
#    else
    while (1)
    {
        if (!(s1[0] = s2[0])) break;
        if (!(s1[1] = s2[1])) break;
        if (!(s1[2] = s2[2])) break;
        if (!(s1[3] = s2[3])) break;
        s1 += 4;
        s2 += 4;
    }
#    endif

    return s;
}
#endif

/* *******************************************************
 * interfaces
 */
xu_char_t* xu_strcpy(xu_char_t* s1, xu_char_t const* s2)
{
    // check
#ifdef __xu_debug__
    {
        // overflow dst?
        xu_size_t n2 = xu_strlen(s2);

        // strcpy overflow?
        xu_size_t n1 = xu_pool_data_size(s1);
        if (n1 && n2 + 1 > n1)
        {
            xu_trace_i("[strcpy]: [overflow]: [%p, %lu] => [%p, %lu]", s2, n2, s1, n1);
            xu_backtrace_dump("[strcpy]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s2, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#endif

    // done
    return xu_strcpy_impl(s1, s2);
}
