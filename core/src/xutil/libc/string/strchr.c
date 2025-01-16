#include "xutil/libc/string/string.h"

#ifdef XU_CONFIG_LIBC_HAVE_STRCHR
#    include <string.h>
#endif

#ifdef XU_CONFIG_LIBC_HAVE_STRCHR
xu_char_t* xu_strchr(xu_char_t const* s, xu_char_t c)
{
    xu_assert(s);
    return (xu_char_t*)strchr(s, c);
}
#else
xu_char_t* xu_strchr(xu_char_t const* s, xu_char_t c)
{
    xu_assert_and_check_return_val(s, xu_null);
    while (*s)
    {
        if (*s == c) return (xu_char_t*)s;
        s++;
    }
    return xu_null;
}
#endif
