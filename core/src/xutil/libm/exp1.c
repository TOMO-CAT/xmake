#include "xutil/libm/math.h"

xu_double_t xu_exp1(xu_double_t x)
{
    xu_assert(x >= -1 && x <= 1);
    return (1 + (x) + ((x) * (x)) / 2 + ((x) * (x) * (x)) / 6);
}
