#include "xutil/libc/string/string.h"

xu_char_t* xu_strnchr(xu_char_t const* s, xu_size_t n, xu_char_t c)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // find
    xu_char_t const* e = s + n;
    while (s < e && *s)
    {
        if (*s == c) return (xu_char_t*)s;
        s++;
    }
    return xu_null;
}
