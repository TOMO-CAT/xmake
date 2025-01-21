#include "xutil/hash/ap.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

xu_size_t xu_ap_make(xu_byte_t const* data, xu_size_t size, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_size_t v = 0xAAAAAAAA;
    if (seed) v ^= seed;

    // generate it
    xu_size_t        i = 0;
    xu_byte_t const* p = data;
    for (i = 0; i < size; i++, p++)
    {
        v ^= (!(i & 1)) ? ((v << 7) ^ ((*p) * (v >> 3))) : (~(((v << 11) + (*p)) ^ (v >> 5)));
    }
    return v;
}
xu_size_t xu_ap_make_from_cstr(xu_char_t const* cstr, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_ap_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
