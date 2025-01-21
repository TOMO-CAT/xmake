#include "xutil/libc/string/string.h"

xu_char_t* xu_strnirchr(xu_char_t const* s, xu_size_t n, xu_char_t c)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // done
    xu_byte_t const* p = (xu_byte_t const*)s + n - 1;
    xu_byte_t        b = xu_tolower((xu_byte_t)c);
    while (p >= (xu_byte_t const*)s && *p)
    {
        if (xu_tolower(*p) == b) return (xu_char_t*)p;
        p--;
    }
    return xu_null;
}
