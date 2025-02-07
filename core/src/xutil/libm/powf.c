#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_float_t xu_powf(xu_float_t x, xu_float_t y)
{
#ifdef XU_CONFIG_LIBM_HAVE_POWF
    return powf(x, y);
#else
    return (xu_float_t)xu_pow(x, y);
#endif
}
