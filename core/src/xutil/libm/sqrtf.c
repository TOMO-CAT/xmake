#include "xutil/libm/math.h"
#include <math.h>

xu_float_t xu_sqrtf(xu_float_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_SQRTF
    return sqrtf(x);
#else
    return (xu_float_t)xu_sqrt(x);
#endif
}
