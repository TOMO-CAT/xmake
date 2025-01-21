#include "xutil/libc/string/string.h"

#ifdef XU_CONFIG_LIBC_HAVE_STRSTR
#    include <string.h>
#endif

#ifdef XU_CONFIG_LIBC_HAVE_STRSTR
xu_char_t* xu_strstr(xu_char_t const* s1, xu_char_t const* s2)
{
    xu_assert_and_check_return_val(s1 && s2, xu_null);
    return (xu_char_t*)strstr(s1, s2);
}
#else
xu_char_t* xu_strstr(xu_char_t const* s1, xu_char_t const* s2)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, xu_null);

    // init
    __xu_register__ xu_char_t const* s = s1;
    __xu_register__ xu_char_t const* p = s2;

    // done
    do
    {
        if (!*p) return (xu_char_t*)s1;
        if (*p == *s)
        {
            ++p;
            ++s;
        }
        else
        {
            p = s2;
            if (!*s) return xu_null;
            s = ++s1;
        }

    } while (1);

    // no found
    return xu_null;
}
#endif
