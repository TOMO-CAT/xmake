#include "xutil/libc/string/string.h"

#ifndef XU_CONFIG_LIBC_HAVE_MEMMOVE
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/memmov.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/memmov.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/memmov.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */

#if defined(XU_CONFIG_LIBC_HAVE_MEMMOVE)
static xu_pointer_t xu_memmov_impl(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    xu_assert_and_check_return_val(s1 && s2, xu_null);
    return memmove(s1, s2, n);
}
#elif !defined(XU_LIBC_STRING_IMPL_MEMMOV)
static xu_pointer_t xu_memmov_impl(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    xu_assert_and_check_return_val(s1 && s2, xu_null);

    __xu_register__ xu_byte_t*       s = (xu_byte_t*)s1;
    __xu_register__ xu_byte_t const* p = (xu_byte_t const*)s2;

    if (p >= s)
    {
        while (n)
        {
            *s++ = *p++;
            --n;
        }
    }
    else
    {
        while (n)
        {
            --n;
            s[n] = p[n];
        }
    }

    return s1;
}
#endif

/* *******************************************************
 * interfaces
 * *******************************************************
 */

xu_pointer_t xu_memmov_(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // done
    return xu_memmov_impl(s1, s2, n);
}

xu_pointer_t xu_memmov(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // check
#ifdef __xu_debug__
    {
        // overflow dst?
        xu_size_t n1 = xu_pool_data_size(s1);
        if (n1 && n > n1)
        {
            xu_trace_i("[memmov]: [overflow]: [%p, %lu] => [%p, %lu]", s2, n, s1, n1);
            xu_backtrace_dump("[memmov]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s1, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }

        // overflow src?
        xu_size_t n2 = xu_pool_data_size(s2);
        if (n2 && n > n2)
        {
            xu_trace_i("[memmov]: [overflow]: [%p, %lu] => [%p, %lu]", s2, n, s1, n1);
            xu_backtrace_dump("[memmov]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s2, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#endif

    // done
    return xu_memmov_impl(s1, s2, n);
}
