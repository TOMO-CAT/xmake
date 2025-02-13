#include "xutil/libm/math.h"

xu_float_t xu_exp1f(xu_float_t x)
{
    xu_assert(x >= -1 && x <= 1);
    return (1 + (x) + ((x) * (x)) / 2 + ((x) * (x) * (x)) / 6);
}
