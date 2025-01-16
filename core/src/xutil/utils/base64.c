#include "xutil/utils/base64.h"
#include "xutil/macros/assert.h"
#include "xutil/utils/utils.h"

#define XU_BASE64_OUTPUT_MIN(in) (((in) + 2) / 3 * 4 + 1)

xu_size_t xu_base64_encode(xu_byte_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    // table
    static xu_char_t const table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    // check
    xu_assert_and_check_return_val(ib && ob && !(in >= XU_MAXU32 / 4 || on < XU_BASE64_OUTPUT_MIN(in)), 0);

    // done
    xu_char_t*  op    = ob;
    xu_uint32_t bits  = 0;
    xu_long_t   left  = in;
    xu_long_t   shift = 0;
    while (left)
    {
        bits = (bits << 8) + *ib++;
        left--;
        shift += 8;

        do
        {
            *op++ = table[(bits << 6 >> shift) & 0x3f];
            shift -= 6;
        } while (shift > 6 || (left == 0 && shift > 0));
    }

    // done tail
    while ((op - ob) & 3)
        *op++ = '=';
    *op = '\0';

    // ok?
    return (op - ob);
}
xu_size_t xu_base64_decode(xu_char_t const* ib, xu_size_t in, xu_byte_t* ob, xu_size_t on)
{
    // check
    xu_assert_and_check_return_val(ib && ob, 0);

    // the table
    static xu_byte_t table[] = {0x3e, 0xff, 0xff, 0xff, 0x3f, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
                                0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
                                0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1a, 0x1b,
                                0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                                0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33};

    // done
    xu_int_t   i  = 0;
    xu_int_t   v  = 0;
    xu_byte_t* op = ob;
    xu_size_t  tn = xu_arrayn(table);
    for (i = 0; i < in && ib[i] && ib[i] != '='; i++)
    {
        xu_uint32_t idx = ib[i] - 43;
        if (idx >= tn || table[idx] == 0xff) return 0;

        v = (v << 6) + table[idx];
        if (i & 3)
        {
            if (op - ob < on) *op++ = v >> (6 - 2 * (i & 3));
        }
    }

    // ok?
    return (op - ob);
}
