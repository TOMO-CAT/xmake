#include "xutil/libc/string/string.h"

#include "string.h"
#ifndef XU_CONFIG_LIBC_HAVE_STRCMP
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/strcmp.c"
#    elif defined(XU_ARCH_ARM)
#        include "xutil/libc/string/impl/arm/strcmp.c"
#    elif defined(XU_ARCH_SH4)
#        include "xutil/libc/string/impl/sh4/strcmp.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */

#if defined(XU_CONFIG_LIBC_HAVE_STRCMP)
static xu_long_t xu_strcmp_impl(xu_char_t const* s1, xu_char_t const* s2)
{
    if (s1 == s2) return 0;
    xu_assert_and_check_return_val(s1 && s2, -1);
    return strcmp(s1, s2);
}
#elif !defined(XU_LIBC_STRING_IMPL_STRCMP)
static xu_long_t xu_strcmp_impl(xu_char_t const* s1, xu_char_t const* s2)
{
    if (s1 == s2) return 0;
    xu_assert_and_check_return_val(s1 && s2, -1);

    xu_long_t r = 0;
    while (((r = ((xu_long_t)(*((xu_byte_t*)s1))) - *((xu_byte_t*)s2++)) == 0) && *s1++)
        ;
    return r;
}
#endif

/* *******************************************************
 * interfaces
 * *******************************************************
 */

xu_long_t xu_strcmp(xu_char_t const* s1, xu_char_t const* s2)
{
#ifdef __xu_debug__
    {
        // check overflow?
        xu_strlen(s1);
        xu_strlen(s2);
    }
#endif
    return xu_strcmp_impl(s1, s2);
}
