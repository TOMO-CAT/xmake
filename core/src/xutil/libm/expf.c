#include "xutil/libm/math.h"
#include <math.h>

xu_float_t xu_expf(xu_float_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_EXPF
    return expf(x);
#else
    xu_float_t a = x - (xu_long_t)x;
    return (xu_expif(((xu_long_t)x)) * xu_exp1f(a));
#endif
}
