#include "xutil/libm/math.h"

#include <math.h>

xu_double_t xu_exp(xu_double_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_EXP
    return exp(x);
#else
    xu_double_t a = x - (xu_double_t)(xu_long_t)x;
    return (xu_expi(((xu_long_t)x)) * xu_exp1(a));
#endif
}
