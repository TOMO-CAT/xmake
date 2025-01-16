#include "xutil/hash/rs.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

xu_size_t xu_rs_make(xu_byte_t const* data, xu_size_t size, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_size_t value = seed;

    // generate it
    xu_size_t b = 378551;
    xu_size_t a = 63689;
    while (size--)
    {
        value = value * a + (*data++);
        a     = a * b;
    }
    return value;
}
xu_size_t xu_rs_make_from_cstr(xu_char_t const* cstr, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_rs_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
