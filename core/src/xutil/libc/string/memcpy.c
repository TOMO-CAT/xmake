#include "xutil/libc/string/string.h"

#include "xutil/macros/assert.h"
#include "xutil/memory/impl/prefix.h"
#ifndef XU_CONFIG_LIBC_HAVE_MEMCPY
#    if defined(XU_ARCH_x86)
#        include "xutil/memory/string/impl/x86/memcpy.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/memory/string/impl/arm/memcpy.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/memory/string/impl/sh4/memcpy.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */

#if defined(XU_CONFIG_LIBC_HAVE_MEMCPY)
static xu_pointer_t xu_memcpy_impl(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    xu_assert_and_check_return_val(s1 && s2, xu_null);
    return memcpy(s1, s2, n);
}
#elif !defined(XU_LIBC_STRING_IMPL_MEMCPY)
static xu_pointer_t xu_memcpy_impl(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, xu_null);

#    ifdef __xu_small__
    __xu_register__ xu_byte_t*       p1 = (xu_byte_t*)s1;
    __xu_register__ xu_byte_t const* p2 = (xu_byte_t const*)s2;
    if (p1 == p2 || !n) return s1;
    while (n--)
        *p1++ = *p2++;
    return s1;
#    else
    __xu_register__ xu_byte_t*       p1 = (xu_byte_t*)s1;
    __xu_register__ xu_byte_t const* p2 = (xu_byte_t const*)s2;
    if (p1 == p2 || !n) return s1;

    xu_size_t l = n & 0x3;
    n           = (n - l) >> 2;
    while (n--)
    {
        p1[0] = p2[0];
        p1[1] = p2[1];
        p1[2] = p2[2];
        p1[3] = p2[3];
        p1 += 4;
        p2 += 4;
    }
    while (l--)
        *p1++ = *p2++;
    return s1;
#    endif /* __xu_small__ */
}
#endif

/* *******************************************************
 * interfaces
 * *******************************************************
 */
xu_pointer_t xu_memcpy_(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // done
    return xu_memcpy_impl(s1, s2, n);
}
xu_pointer_t xu_memcpy(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n)
{
    // check
#ifdef __xu_debug__
    {
        // overflow dst?
        xu_size_t n1 = xu_pool_data_size(s1);
        if (n1 && n > n1)
        {
            xu_trace_i("[memcpy]: [overflow]: [%p, %lu] => [%p, %lu]", s2, n, s1, n1);
            xu_backtrace_dump("[memcpy]: [overflow]: [dst]: ", xu_null, 10);
            xu_pool_data_dump(s1, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }

        // overflow src?
        xu_size_t n2 = xu_pool_data_size(s2);
        if (n2 && n > n2)
        {
            xu_trace_i("[memcpy]: [overflow]: [%p, %lu(%lu)] => [%p, %lu]", s2, n, n2, s1, n1);
            xu_backtrace_dump("[memcpy]: [overflow]: [src] ", xu_null, 10);
            xu_pool_data_dump(s2, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }

        // overlap?
        if (((xu_byte_t*)s2 >= (xu_byte_t*)s1 && (xu_byte_t*)s2 < (xu_byte_t*)s1 + n) ||
            ((xu_byte_t*)s1 >= (xu_byte_t*)s2 && (xu_byte_t*)s1 < (xu_byte_t*)s2 + n))
        {
            xu_trace_i("[memcpy]: [overlap]: [%p, %lu] => [%p, %lu]", s2, n, s1, n);
            xu_backtrace_dump("[memcpy]: [overlap]: ", xu_null, 10);
            xu_pool_data_dump(s1, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#endif

    // done
    return xu_memcpy_impl(s1, s2, n);
}
