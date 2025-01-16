#include "xutil/libm/math.h"

xu_long_t xu_isnan(xu_double_t x)
{
#if 0
    xu_ieee_double_t e; e.d = x;
    xu_int32_t      t = e.i.h & 0x7fffffff;
    t |= (xu_uint32_t)(e.i.l | (xu_uint32_t)(-(xu_int32_t)e.i.l)) >> 31;
    t = 0x7ff00000 - t;
    return (xu_long_t)(((xu_uint32_t)t) >> 31);
#else
    // optimization
    return x != x;
#endif
}
