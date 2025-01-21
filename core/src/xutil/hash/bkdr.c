#include "xutil/hash/bkdr.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

xu_size_t xu_bkdr_make(xu_byte_t const* data, xu_size_t size, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_size_t value = 0;
    if (seed) value = value * 131313 + seed;

    // generate it
    while (size--)
        value = (value * 131313) + (*data++);
    return value;
}
xu_size_t xu_bkdr_make_from_cstr(xu_char_t const* cstr, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_bkdr_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
