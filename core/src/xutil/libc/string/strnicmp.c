#include "xutil/libc/string/string.h"

#ifdef XU_CONFIG_LIBC_HAVE_STRNCASECMP
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 */

#ifdef XU_CONFIG_LIBC_HAVE_STRNCASECMP
static xu_long_t xu_strnicmp_impl(xu_char_t const* s1, xu_char_t const* s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, 0);
#    ifdef XU_COMPILER_IS_MSVC
    return _strnicmp(s1, s2, n);
#    else
    return strncasecmp(s1, s2, n);
#    endif
}
#else
static xu_long_t xu_strnicmp_impl(xu_char_t const* s1, xu_char_t const* s2, xu_size_t n)
{
    xu_assert_and_check_return_val(s1 && s2, 0);
    if (s1 == s2 || !n) return 0;

    xu_long_t r = 0;
    while (n && ((s1 == s2) || !(r = ((xu_long_t)(xu_tolower(*((xu_byte_t*)s1)))) - xu_tolower(*((xu_byte_t*)s2)))) &&
           (--n, ++s2, *s1++))
    {
    }
    return r;
}
#endif

/* *******************************************************
 * interfaces
 */

xu_long_t xu_strnicmp(xu_char_t const* s1, xu_char_t const* s2, xu_size_t n)
{
    // done
    return xu_strnicmp_impl(s1, s2, n);
}
