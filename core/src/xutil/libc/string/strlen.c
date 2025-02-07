#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/memory/impl/prefix.h"

#ifndef XU_CONFIG_LIBC_HAVE_STRLEN
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/strlen.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/strlen.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/strlen.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 *  implementation
 */
#if defined(XU_CONFIG_LIBC_HAVE_STRLEN)
static xu_size_t xu_strlen_impl(xu_char_t const* s)
{
    xu_assert_and_check_return_val(s, 0);
    return strlen(s);
}
#elif !defined(XU_LIBC_STRING_IMPL_STRLEN)
static xu_size_t xu_strlen_impl(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    __xu_register__ xu_char_t const* p = s;
#    ifdef __xu_small__
    while (*p)
        p++;
    return (p - s);
#    else
    while (1)
    {
        if (!p[0]) return (p - s + 0);
        if (!p[1]) return (p - s + 1);
        if (!p[2]) return (p - s + 2);
        if (!p[3]) return (p - s + 3);
        p += 4;
    }
    return 0;
#    endif
}
#endif

/* *******************************************************
 *  interfaces
 */
xu_size_t xu_strlen(xu_char_t const* s)
{
    // check
#ifdef __xu_debug__
    {
        // overflow?
        xu_size_t size = xu_pool_data_size(s);
        if (size)
        {
            // no '\0'?
            xu_size_t real = xu_strnlen(s, size);
            if (s[real])
            {
                xu_trace_i("[strlen]: [overflow]: [%p, %lu]", s, size);
                xu_backtrace_dump("[strlen]: [overflow]: ", xu_null, 10);
                xu_pool_data_dump(s, xu_true, "\t[malloc]: [from]: ");
                xu_abort();
            }
        }
    }
#endif

    // done
    return xu_strlen_impl(s);
}
