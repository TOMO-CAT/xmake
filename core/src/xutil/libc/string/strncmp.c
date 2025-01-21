#include "xutil/libc/string/string.h"

#ifndef XU_CONFIG_LIBC_HAVE_STRNCMP
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/strncmp.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/strncmp.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/strncmp.c"
#    endif
#else
#    include <string.h>
#endif

#if defined(XU_CONFIG_LIBC_HAVE_STRNCMP)
static xu_long_t xu_strncmp_impl(xu_char_t const* s1, xu_char_t const* s2, xu_size_t n)
{
    if (s1 == s2 || !n) return 0;
    xu_assert_and_check_return_val(s1 && s2, -1);
    return strncmp(s1, s2, n);
}
#elif !defined(XU_LIBC_STRING_IMPL_STRNCMP)
static xu_long_t xu_strncmp_impl(xu_char_t const* s1, xu_char_t const* s2, xu_size_t n)
{
    if (s1 == s2 || !n) return 0;
    xu_assert_and_check_return_val(s1 && s2, -1);

    xu_long_t r = 0;
    while (n-- && ((r = ((xu_long_t)(*((xu_byte_t*)s1))) - *((xu_byte_t*)s2++)) == 0) && *s1++)
        ;
    return r;
}
#endif

xu_long_t xu_strncmp(xu_char_t const* s1, xu_char_t const* s2, xu_size_t n) { return xu_strncmp_impl(s1, s2, n); }
