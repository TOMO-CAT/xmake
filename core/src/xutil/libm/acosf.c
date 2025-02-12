#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_float_t xu_acosf(xu_float_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_ACOSF
    return acosf(x);
#else
    return (xu_float_t)xu_acos(x);
#endif
}
