#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

#ifdef XU_CONFIG_LIBC_HAVE_STRCASECMP
#    include <string.h>
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */
#ifdef XU_CONFIG_LIBC_HAVE_STRCASECMP
static xu_long_t xu_stricmp_impl(xu_char_t const* s1, xu_char_t const* s2)
{
    if (s1 == s2) return 0;
    xu_assert_and_check_return_val(s1 && s2, -1);
#    ifdef XU_COMPILER_IS_MSVC
    return _stricmp(s1, s2);
#    else
    return strcasecmp(s1, s2);
#    endif
}
#else
static xu_long_t xu_stricmp_impl(xu_char_t const* s1, xu_char_t const* s2)
{
    if (s1 == s2) return 0;
    xu_assert_and_check_return_val(s1 && s2, -1);

    xu_long_t r = 0;
    while (((s1 == s2) || !(r = ((xu_long_t)(xu_tolower(*((xu_byte_t*)s1)))) - xu_tolower(*((xu_byte_t*)s2)))) &&
           (++s2, *s1++))
    {
    }
    return r;
}
#endif

/* *******************************************************
 * interfaces
 * *******************************************************
 */
xu_long_t xu_stricmp(xu_char_t const* s1, xu_char_t const* s2)
{
#ifdef __xu_debug__
    {
        // check overflow?
        xu_strlen(s1);
        xu_strlen(s2);
    }
#endif
    return xu_stricmp_impl(s1, s2);
}
