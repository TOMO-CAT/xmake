#include "xutil/hash/murmur.h"
#include "xutil/macros/assert.h"
#include "xutil/libc/string/string.h"

xu_size_t xu_murmur_make(xu_byte_t const* data, xu_size_t size, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_size_t value = seed;

    // generate it
    while (size--)
    {
        value ^= (*data++);
        value *= 0x5bd1e995;
        value ^= value >> 15;
    }
    return value;
}
xu_size_t xu_murmur_make_from_cstr(xu_char_t const* cstr, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_murmur_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
