#include "xutil/libc/string/string.h"

#ifndef XU_CONFIG_LIBC_HAVE_MEMSET
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/memset.c"
#    elif defined(XU_ARCH_x64)
#        include "xutil/libc/string/impl/x86/memset.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/memset.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/memset.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */

#if defined(XU_CONFIG_LIBC_HAVE_MEMSET)
static xu_pointer_t xu_memset_impl(xu_pointer_t s, xu_byte_t c, xu_size_t n)
{
    xu_assert_and_check_return_val(s, xu_null);
    return memset(s, c, n);
}
#elif !defined(XU_LIBC_STRING_IMPL_MEMSET_U8)
static xu_pointer_t xu_memset_impl(xu_pointer_t s, xu_byte_t c, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // no size?
    xu_check_return_val(n, s);

    // init
    __xu_register__ xu_byte_t* p = (xu_byte_t*)s;

    // done
#    ifdef __xu_small__
    while (n--)
        *p++ = c;
#    else
    xu_size_t l = n & 0x3;
    n           = (n - l) >> 2;
    while (n--)
    {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }

    while (l--)
        *p++ = c;
#    endif
    return s;
}
#endif

#if !defined(XU_LIBC_STRING_IMPL_MEMSET_U16) && !defined(XU_CONFIG_MICRO_ENABLE)
static xu_pointer_t xu_memset_u16_impl(xu_pointer_t s, xu_uint16_t c, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // no size?
    xu_check_return_val(n, s);

    // must be aligned by 2-bytes
    xu_assert(!(((xu_size_t)s) & 0x1));

    // init
    __xu_register__ xu_uint16_t* p = (xu_uint16_t*)s;

    // done
#    ifdef __xu_small__
    while (n--)
        *p++ = c;
#    else
    xu_size_t l = n & 0x3;
    n           = (n - l) >> 2;
    while (n--)
    {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }

    while (l--)
        *p++ = c;
#    endif

    // ok?
    return s;
}
#endif

#if !defined(XU_LIBC_STRING_IMPL_MEMSET_U24) && !defined(XU_CONFIG_MICRO_ENABLE)
static xu_pointer_t xu_memset_u24_impl(xu_pointer_t s, xu_uint32_t c, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // no size?
    xu_check_return_val(n, s);

    // init
    __xu_register__ xu_byte_t* p = (xu_byte_t*)s;
    __xu_register__ xu_byte_t* e = p + (n * 3);

    // done
#    ifdef __xu_small__
    for (; p < e; p += 3)
        xu_bits_set_u24_ne(p, c);
#    else
    xu_size_t l = n & 0x3;
    n -= l;
    while (p < e)
    {
        xu_bits_set_u24_ne(p + 0, c);
        xu_bits_set_u24_ne(p + 3, c);
        xu_bits_set_u24_ne(p + 6, c);
        xu_bits_set_u24_ne(p + 9, c);
        p += 12;
    }

    while (l--)
    {
        xu_bits_set_u24_ne(p, c);
        p += 3;
    }
#    endif

    // ok?
    return s;
}
#endif

#if !defined(XU_LIBC_STRING_IMPL_MEMSET_U32) && !defined(XU_CONFIG_MICRO_ENABLE)
static xu_pointer_t xu_memset_u32_impl(xu_pointer_t s, xu_uint32_t c, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // no size?
    xu_check_return_val(n, s);

    // must be aligned by 4-bytes
    xu_assert(!(((xu_size_t)s) & 0x3));

    // init
    __xu_register__ xu_uint32_t* p = (xu_uint32_t*)s;

    // done
#    ifdef __xu_small__
    while (n--)
        *p++ = c;
#    else
    xu_size_t l = n & 0x3;
    n           = (n - l) >> 2;
    while (n--)
    {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }

    while (l--)
        *p++ = c;
#    endif

    // ok?
    return s;
}
#endif

#if !defined(XU_LIBC_STRING_IMPL_MEMSET_U64) && !defined(XU_CONFIG_MICRO_ENABLE)
static xu_pointer_t xu_memset_u64_impl(xu_pointer_t s, xu_uint64_t c, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // no size?
    xu_check_return_val(n, s);

    // must be aligned by 8-bytes
    xu_assert(!(((xu_size_t)s) & 0x7));

    // init
    __xu_register__ xu_uint64_t* p = (xu_uint64_t*)s;

    // done
#    ifdef __xu_small__
    while (n--)
        *p++ = c;
#    else
    xu_size_t l = n & 0x3;
    n           = (n - l) >> 2;
    while (n--)
    {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }

    while (l--)
        *p++ = c;
#    endif

    // ok?
    return s;
}
#endif

/* *******************************************************
 * interfaces
 * *******************************************************
 */

xu_pointer_t xu_memset_(xu_pointer_t s, xu_byte_t c, xu_size_t n)
{
    // done
    return xu_memset_impl(s, c, n);
}
xu_pointer_t xu_memset(xu_pointer_t s, xu_byte_t c, xu_size_t n)
{
    // check
#ifdef __xu_debug__
    {
        // overflow?
        xu_size_t size = xu_pool_data_size(s);
        if (size && n > size)
        {
            xu_trace_i("[memset]: [overflow]: [%#x x %lu] => [%p, %lu]", c, n, s, size);
            xu_backtrace_dump("[memset]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#endif

    // done
    return xu_memset_impl(s, c, n);
}
#ifndef XU_CONFIG_MICRO_ENABLE
xu_pointer_t xu_memset_u16(xu_pointer_t s, xu_uint16_t c, xu_size_t n)
{
    // check
#    ifdef __xu_debug__
    {
        // overflow?
        xu_size_t size = xu_pool_data_size(s);
        if (size && (n << 1) > size)
        {
            xu_trace_i("[memset_u16]: [overflow]: [%#x x %lu x 2] => [%p, %lu]", c, n, s, size);
            xu_backtrace_dump("[memset_u16]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#    endif

    // done
    return xu_memset_u16_impl(s, c, n);
}
xu_pointer_t xu_memset_u24(xu_pointer_t s, xu_uint32_t c, xu_size_t n)
{
    // check
#    ifdef __xu_debug__
    {
        // overflow?
        xu_size_t size = xu_pool_data_size(s);
        if (size && (n * 3) > size)
        {
            xu_trace_i("[memset_u24]: [overflow]: [%#x x %lu x 3] => [%p, %lu]", c, n, s, size);
            xu_pool_data_dump(s, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#    endif

    // done
    return xu_memset_u24_impl(s, c, n);
}
xu_pointer_t xu_memset_u32(xu_pointer_t s, xu_uint32_t c, xu_size_t n)
{
    // check
#    ifdef __xu_debug__
    {
        // overflow?
        xu_size_t size = xu_pool_data_size(s);
        if (size && (n << 2) > size)
        {
            xu_trace_i("[memset_u32]: [overflow]: [%#x x %lu x 4] => [%p, %lu]", c, n, s, size);
            xu_backtrace_dump("[memset_u32]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#    endif

    // done
    return xu_memset_u32_impl(s, c, n);
}
xu_pointer_t xu_memset_u64(xu_pointer_t s, xu_uint64_t c, xu_size_t n)
{
    // check
#    ifdef __xu_debug__
    {
        // overflow?
        xu_size_t size = xu_pool_data_size(s);
        if (size && (n << 3) > size)
        {
            xu_trace_i("[memset_u64]: [overflow]: [%#llx x %lu x 4] => [%p, %lu]", c, n, s, size);
            xu_backtrace_dump("[memset_u64]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#    endif

    // done
    return xu_memset_u64_impl(s, c, n);
}
#endif
