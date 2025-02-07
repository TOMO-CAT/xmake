/* *******************************************************
 * includes
 */
#include "xutil/libc/string/string.h"
#ifdef XU_CONFIG_LIBC_HAVE_STRCAT
#    include <string.h>
#endif

/* *******************************************************
 * interfaces
 */

#ifdef XU_CONFIG_LIBC_HAVE_STRCAT
xu_char_t* xu_strcat(xu_char_t* s1, xu_char_t const* s2)
{
    xu_assert_and_check_return_val(s1 && s2, xu_null);
    return strcat(s1, s2);
}
#else
xu_char_t* xu_strcat(xu_char_t* s1, xu_char_t const* s2)
{
    xu_assert_and_check_return_val(s1 && s2, xu_null);

    __xu_register__ xu_char_t* s = s1;
    while (*s++)
        {}
    --s;
    while (!(*s++ = *s2++))
        {}
    return s1;
}
#endif
