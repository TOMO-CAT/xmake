#include "xutil/libc/string/string.h"

xu_char_t* xu_strichr(xu_char_t const* s, xu_char_t c)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // init
    xu_byte_t const* p = (xu_byte_t const*)s;
    xu_byte_t        b = xu_tolower((xu_byte_t)c);

    // find
    while (*p)
    {
        if (xu_tolower(*p) == b) return (xu_char_t*)p;
        p++;
    }

    return xu_null;
}
