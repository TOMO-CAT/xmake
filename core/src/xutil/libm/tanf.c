#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_float_t xu_tanf(xu_float_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_TANF
    return tanf(x);
#else
    return (xu_float_t)xu_tan(x);
#endif
}
