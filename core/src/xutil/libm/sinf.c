#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_float_t xu_sinf(xu_float_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_SINF
    return sinf(x);
#else
    return (xu_float_t)xu_sin(x);
#endif
}
