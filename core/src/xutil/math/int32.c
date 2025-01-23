#include "xutil/math/int32.h"

/* *******************************************************
 * implementation
 */
xu_int32_t xu_int32_div(xu_int32_t x, xu_int32_t y, xu_int_t nbits)
{
    xu_assert(y);
    xu_check_return_val(x, 0);

    // get sign
    xu_int32_t s = xu_int32_get_sign(x ^ y);
    x            = xu_abs(x);
    y            = xu_abs(y);

    xu_int_t xbits = (xu_int_t)xu_bits_cl0_u32_be(x) - 1;
    xu_int_t ybits = (xu_int_t)xu_bits_cl0_u32_be(y) - 1;
    xu_int_t bits  = nbits - xbits + ybits;

    // underflow?
    if (bits < 0) return 0;

    // overflow?
    if (bits > 31) return xu_int32_set_sign(XU_MAXS32, s);

    x <<= xbits;
    y <<= ybits;

    // do the first one
    xu_int32_t r = 0;
    if ((x -= y) >= 0)
        r = 1;
    else
        x += y;

    // now fall into our switch statement if there are more bits to compute
    if (bits > 0)
    {
        // make room for the rest of the answer bits
        r <<= bits;
        switch (bits)
        {
#define XU_INT32_DIV_CASE(n)                                                                                           \
    case n:                                                                                                            \
        if ((x = (x << 1) - y) >= 0)                                                                                   \
            r |= 1 << (n - 1);                                                                                         \
        else                                                                                                           \
            x += y

            XU_INT32_DIV_CASE(31);
            XU_INT32_DIV_CASE(30);
            XU_INT32_DIV_CASE(29);
            XU_INT32_DIV_CASE(28);
            XU_INT32_DIV_CASE(27);
            XU_INT32_DIV_CASE(26);
            XU_INT32_DIV_CASE(25);
            XU_INT32_DIV_CASE(24);
            XU_INT32_DIV_CASE(23);
            XU_INT32_DIV_CASE(22);
            XU_INT32_DIV_CASE(21);
            XU_INT32_DIV_CASE(20);
            XU_INT32_DIV_CASE(19);
            XU_INT32_DIV_CASE(18);
            XU_INT32_DIV_CASE(17);
            XU_INT32_DIV_CASE(16);
            XU_INT32_DIV_CASE(15);
            XU_INT32_DIV_CASE(14);
            XU_INT32_DIV_CASE(13);
            XU_INT32_DIV_CASE(12);
            XU_INT32_DIV_CASE(11);
            XU_INT32_DIV_CASE(10);
            XU_INT32_DIV_CASE(9);
            XU_INT32_DIV_CASE(8);
            XU_INT32_DIV_CASE(7);
            XU_INT32_DIV_CASE(6);
            XU_INT32_DIV_CASE(5);
            XU_INT32_DIV_CASE(4);
            XU_INT32_DIV_CASE(3);
            XU_INT32_DIV_CASE(2);

            // we merge these last two together, makes gcc make better arm
        default: XU_INT32_DIV_CASE(1);
        }
    }

    if (r < 0) r = XU_MAXS32;
    return xu_int32_set_sign(r, s);
}
