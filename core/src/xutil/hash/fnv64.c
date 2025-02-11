#include "xutil/hash/fnv64.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

/* *******************************************************
 * macros
 */

// the pnv64 prime and offset basis
#define XU_FNV64_PRIME (1099511628211ULL)
#define XU_FNV64_OFFSET_BASIS (14695981039346656037ULL)

/* *******************************************************
 * implementation
 */
xu_uint64_t xu_fnv64_make(xu_byte_t const* data, xu_size_t size, xu_uint64_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_uint64_t value = XU_FNV64_OFFSET_BASIS;
    if (seed) value = (value * XU_FNV64_PRIME) ^ seed;

    // generate it
    while (size)
    {
        value *= XU_FNV64_PRIME;
        value ^= (xu_uint64_t)*data++;
        size--;
    }
    return value;
}
xu_uint64_t xu_fnv64_make_from_cstr(xu_char_t const* cstr, xu_uint64_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_fnv64_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
xu_uint64_t xu_fnv64_1a_make(xu_byte_t const* data, xu_size_t size, xu_uint64_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_uint64_t value = XU_FNV64_OFFSET_BASIS;
    if (seed) value = (value * XU_FNV64_PRIME) ^ seed;

    // generate it
    while (size)
    {
        value ^= (xu_uint64_t)*data++;
        value *= XU_FNV64_PRIME;
        size--;
    }
    return value;
}
xu_uint64_t xu_fnv64_1a_make_from_cstr(xu_char_t const* cstr, xu_uint64_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_fnv64_1a_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
