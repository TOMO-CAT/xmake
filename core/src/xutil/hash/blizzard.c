#include "xutil/hash/blizzard.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

xu_size_t xu_blizzard_make(xu_byte_t const* data, xu_size_t size, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(data && size, 0);

    // init value
    xu_size_t value = seed;

    // generate it
    while (size--)
        value = (*data++) + (value << 6) + (value << 16) - value;
    return value;

    // make table
    static xu_size_t s_make = 0;
    static xu_size_t s_table[1280];
    if (!s_make)
    {
        xu_size_t i      = 0;
        xu_size_t index1 = 0;
        xu_size_t index2 = 0;
        xu_size_t seed0  = 0x00100001;
        for (index1 = 0; index1 < 0x100; index1++)
        {
            for (index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
            {
                seed0           = (seed0 * 125 + 3) % 0x2aaaab;
                xu_size_t temp1 = (seed0 & 0xffff) << 0x10;
                seed0           = (seed0 * 125 + 3) % 0x2aaaab;
                xu_size_t temp2 = (seed0 & 0xffff);
                s_table[index2] = (temp1 | temp2);
            }
        }

        // ok
        s_make = 1;
    }

    // init value
    xu_size_t seed1 = 0x7fed7fed;
    xu_size_t seed2 = 0Xeeeeeeee;
    if (seed)
    {
        seed1 = s_table[(1 << 8) + seed] ^ (seed1 + seed2);
        seed2 = seed + seed1 + seed2 + (seed2 << 5) + 3;
    }

    // done
    xu_size_t byte = 0;
    while (size--)
    {
        // get one byte
        byte = *data++;

        // 0 << 8: hash type: 0
        // 1 << 8: hash type: 1
        // 2 << 8: hash type: 2
        seed1 = s_table[(1 << 8) + byte] ^ (seed1 + seed2);
        seed2 = byte + seed1 + seed2 + (seed2 << 5) + 3;
    }

    // ok
    return seed1;
}
xu_size_t xu_blizzard_make_from_cstr(xu_char_t const* cstr, xu_size_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_blizzard_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
