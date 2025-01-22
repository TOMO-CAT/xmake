#include "xutil/libc/string/string.h"

#ifndef XU_CONFIG_LIBC_HAVE_MEMCMP
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/memcmp.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/memcmp.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/memcmp.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_LIBC_HAVE_MEMCMP)
static xu_long_t xu_memcmp_impl(xu_cpointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, 0);

    // done
    return memcmp(s1, s2, n);
}
#elif !defined(XU_LIBC_STRING_IMPL_MEMCMP)
static xu_long_t xu_memcmp_impl(xu_cpointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, 0);

    // equal or empty?
    if (s1 == s2 || !n) return 0;

    // done
    xu_long_t        r  = 0;
    xu_byte_t const* p1 = (xu_byte_t const*)s1;
    xu_byte_t const* p2 = (xu_byte_t const*)s2;
    while (n-- && ((r = ((xu_long_t)(*p1++)) - *p2++) == 0))
        ;
    return r;
}
#endif

/* *******************************************************
 * interfaces
 */
xu_long_t xu_memcmp_(xu_cpointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // done
    return xu_memcmp_impl(s1, s2, n);
}
xu_long_t xu_memcmp(xu_cpointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // check
#ifdef __xu_debug__
    {
        // overflow?
        xu_size_t n1 = xu_pool_data_size(s1);
        if (n1 && n > n1)
        {
            xu_trace_i("[memcmp]: [overflow]: [%p, %lu] ?= [%p, %lu]", s2, n, s1, n1);
            xu_backtrace_dump("[memcmp]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s1, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }

        // overflow?
        xu_size_t n2 = xu_pool_data_size(s2);
        if (n2 && n > n2)
        {
            xu_trace_i("[memcmp]: [overflow]: [%p, %lu(%lu)] ?= [%p, %lu]", s2, n, n2, s1, n1);
            xu_backtrace_dump("[memcmp]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s2, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#endif

    // done
    return xu_memcmp_impl(s1, s2, n);
}
