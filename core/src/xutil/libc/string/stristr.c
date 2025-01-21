#include "xutil/libc/string/string.h"

#ifdef XU_CONFIG_LIBC_HAVE_STRCASESTR
#    include <string.h>
#endif

#ifdef XU_CONFIG_LIBC_HAVE_STRCASESTR
xu_char_t* xu_stristr(xu_char_t const* s1, xu_char_t const* s2)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, xu_null);
    return strcasestr(s1, s2);
}
#else
xu_char_t* xu_stristr(xu_char_t const* s1, xu_char_t const* s2)
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
        if ((*p == *s) || (xu_tolower(*((xu_byte_t*)p)) == xu_tolower(*((xu_byte_t*)s))))
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
