#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_float_t xu_asinf(xu_float_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_ASINF
    return asinf(x);
#else
    return (xu_float_t)xu_asin(x);
#endif
}
