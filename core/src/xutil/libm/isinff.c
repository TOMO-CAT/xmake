#include "xutil/libm/math.h"

xu_long_t xu_isinff(xu_float_t x)
{
    xu_ieee_float_t e;
    e.f          = x;
    xu_int32_t t = e.i & 0x7fffffff;
    t ^= 0x7f800000;
    t |= -t;
    return (xu_long_t)(~(t >> 31) & (e.i >> 30));
}
