#include "xutil/libm/math.h"

xu_long_t xu_isnanf(xu_float_t x)
{
#if 0
    xu_ieee_float_t e; e.f = x;
    xu_int32_t      t = e.i & 0x7fffffff;
    t = 0x7f800000 - t;
    return (xu_long_t)(((xu_uint32_t)(t)) >> 31);
#else
    // optimization
    return x != x;
#endif
}
