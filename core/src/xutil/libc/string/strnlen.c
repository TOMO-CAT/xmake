#include "xutil/libc/string/string.h"

#include "../../memory/impl/prefix.h"
#ifndef XU_CONFIG_LIBC_HAVE_STRNLEN
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/strnlen.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/strnlen.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/strnlen.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */

#if defined(XU_CONFIG_LIBC_HAVE_STRNLEN)
static xu_size_t xu_strnlen_impl(xu_char_t const* s, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s, 0);

#    ifdef XU_CONFIG_OS_ANDROID
    /* fix the bug for android
     *
     * return -1 if n == (xu_uint32_t)-1
     */
    return strnlen(s, (xu_uint16_t)n);
#    else
    return strnlen(s, n);
#    endif
}
#elif !defined(XU_LIBC_STRING_IMPL_STRNLEN)
static xu_size_t xu_strnlen_impl(xu_char_t const* s, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s, 0);
    if (!n) return 0;

    __xu_register__ xu_char_t const* p = s;

#    ifdef __xu_small__
    while (n-- && *p)
        ++p;
    return p - s;
#    else
    xu_size_t l = n & 0x3;
    n           = (n - l) >> 2;
    while (n--)
    {
        if (!p[0]) return (p - s + 0);
        if (!p[1]) return (p - s + 1);
        if (!p[2]) return (p - s + 2);
        if (!p[3]) return (p - s + 3);
        p += 4;
    }

    while (l-- && *p)
        ++p;
    return p - s;
#    endif
}
#endif

/* *******************************************************
 * interfaces
 * *******************************************************
 */

xu_size_t xu_strnlen(xu_char_t const* s, xu_size_t n)
{
    // check
#ifdef __xu_debug__
    {
        // overflow?
        xu_size_t size = xu_pool_data_size(s);
        if (size)
        {
            // no '\0'?
            xu_size_t real = xu_strnlen_impl(s, size);
            if (s[real])
            {
                xu_trace_i("[strnlen]: [overflow]: [%p, %lu]", s, size);
                xu_backtrace_dump("[strnlen]: [overflow]: ", xu_null, 10);
                xu_pool_data_dump(s, xu_true, "\t[malloc]: [from]: ");
                xu_abort();
            }
        }
    }
#endif

    // done
    return xu_strnlen_impl(s, n);
}
