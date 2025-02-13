#include "xutil/utils/base32.h"
#include "xutil/libc/libc.h"

/* *******************************************************
 * macros
 */
#define XU_BASE32_OUTPUT_MIN(in) ((((in)*8) / 5) + (((in) % 5) != 0) + 1)

/* *******************************************************
 * implementation
 */
xu_size_t xu_base32_encode(xu_byte_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    // table
    static xu_char_t const table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    // check
    xu_assert_and_check_return_val(ob && !(in >= XU_MAXU32 / 4 || on < XU_BASE32_OUTPUT_MIN(in)), 0);

    // encode
    xu_size_t  i   = 0;
    xu_byte_t  w   = 0;
    xu_size_t  idx = 0;
    xu_char_t* pb  = ob;
    for (; i < in;)
    {
        if (idx > 3)
        {
            w   = (ib[i] & (0xFF >> idx));
            idx = (idx + 5) & 0x07;
            w <<= idx;
            if (i < in - 1) w |= ib[i + 1] >> (8 - idx);
            i++;
        }
        else
        {
            w   = (ib[i] >> (8 - (idx + 5))) & 0x1F;
            idx = (idx + 5) & 0x07;
            if (idx == 0) i++;
        }
        *pb++ = table[w];
    }
    *pb = '\0';
    return (pb - ob);
}
xu_size_t xu_base32_decode(xu_byte_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    // table
    static xu_byte_t const table[43][2] = {
        {'0', 0xFF}, {'1', 0xFF}, {'2', 0x1A}, {'3', 0x1B}, {'4', 0x1C}, {'5', 0x1D}, {'6', 0x1E}, {'7', 0x1F},
        {'8', 0xFF}, {'9', 0xFF}, {':', 0xFF}, {';', 0xFF}, {'<', 0xFF}, {'=', 0xFF}, {'>', 0xFF}, {'?', 0xFF},
        {'@', 0xFF}, {'A', 0x00}, {'B', 0x01}, {'C', 0x02}, {'D', 0x03}, {'E', 0x04}, {'F', 0x05}, {'G', 0x06},
        {'H', 0x07}, {'I', 0x08}, {'J', 0x09}, {'K', 0x0A}, {'L', 0x0B}, {'M', 0x0C}, {'N', 0x0D}, {'O', 0x0E},
        {'P', 0x0F}, {'Q', 0x10}, {'R', 0x11}, {'S', 0x12}, {'T', 0x13}, {'U', 0x14}, {'V', 0x15}, {'W', 0x16},
        {'X', 0x17}, {'Y', 0x18}, {'Z', 0x19}};

    // check
    xu_assert_and_check_return_val(ib && ob && on > (in * 5) / 8, 0);

    // init
    xu_memset(ob, 0, on);

    // decode
    xu_size_t  i   = 0;
    xu_byte_t  w   = 0;
    xu_size_t  idx = 0;
    xu_char_t* op  = ob;
    for (; i < in; ++i)
    {
        // loopup
        xu_int_t lookup = xu_toupper(ib[i]) - '0';
        if (lookup < 0 || lookup >= 43)
            w = 0xff;
        else
            w = table[lookup][1];
        if (w == 0xff) continue;

        if (idx <= 3)
        {
            idx = (idx + 5) & 0x07;
            if (idx == 0)
                *op++ |= w;
            else
                *op |= w << (8 - idx);
        }
        else
        {
            idx = (idx + 5) & 0x07;
            *op++ |= (w >> idx);
            *op |= w << (8 - idx);
        }
    }
    return (op - ob);
}
