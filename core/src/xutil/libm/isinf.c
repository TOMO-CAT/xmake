#include "xutil/libm/math.h"

xu_long_t xu_isinf(xu_double_t x)
{
    xu_ieee_double_t e;
    e.d          = x;
    xu_int32_t t = e.i.l | ((e.i.h & 0x7fffffff) ^ 0x7ff00000);
    t |= -t;
    return (xu_long_t)(~(t >> 31) & (e.i.h >> 30));
}
