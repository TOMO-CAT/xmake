#include "xutil/libc/string/string.h"
#include "xutil/macros/malloc.h"

xu_char_t* xu_strdup(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // make
    __xu_register__ xu_size_t  n = xu_strlen(s);
    __xu_register__ xu_char_t* p = xu_malloc_cstr(n + 1);
    xu_assert_and_check_return_val(p, xu_null);

    // copy
    xu_memcpy(p, s, n);

    // end
    p[n] = '\0';

    // ok
    return p;
}
