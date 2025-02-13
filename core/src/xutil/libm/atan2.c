#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_double_t xu_atan2(xu_double_t y, xu_double_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_ATAN2
    return atan2(y, x);
#else
    xu_assert(0);
    return 0;
#endif
}
