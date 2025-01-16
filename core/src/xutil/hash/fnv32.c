#include "xutil/hash/fnv32.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

// the pnv32 prime and offset basis
#define XU_FNV32_PRIME (16777619)
#define XU_FNV32_OFFSET_BASIS (2166136261)

xu_uint32_t xu_fnv32_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_uint32_t value = XU_FNV32_OFFSET_BASIS;
    if (seed) value = (value * XU_FNV32_PRIME) ^ seed;

    // generate it
    while (size)
    {
        value *= XU_FNV32_PRIME;
        value ^= (xu_uint32_t)*data++;
        size--;
    }
    return value;
}
xu_uint32_t xu_fnv32_make_from_cstr(xu_char_t const* cstr, xu_uint32_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_fnv32_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
xu_uint32_t xu_fnv32_1a_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_uint32_t value = XU_FNV32_OFFSET_BASIS;
    if (seed) value = (value * XU_FNV32_PRIME) ^ seed;

    // generate it
    while (size)
    {
        value ^= (xu_uint32_t)*data++;
        value *= XU_FNV32_PRIME;
        size--;
    }
    return value;
}
xu_uint32_t xu_fnv32_1a_make_from_cstr(xu_char_t const* cstr, xu_uint32_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_fnv32_1a_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
