#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_float_t xu_atan2f(xu_float_t y, xu_float_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_ATAN2F
    return atan2f(y, x);
#else
    xu_assert(0);
    return 0;
#endif
}
