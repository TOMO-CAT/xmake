#include "xutil/libc/string/string.h"

xu_char_t* xu_strnrchr(xu_char_t const* s, xu_size_t n, xu_char_t c)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // done
    xu_char_t const* p = s + n - 1;
    while (p >= s && *p)
    {
        if (*p == c) return (xu_char_t*)p;
        p--;
    }
    return xu_null;
}
