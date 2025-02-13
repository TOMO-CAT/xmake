/* *******************************************************
 * includes
 */
#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_double_t xu_pow(xu_double_t x, xu_double_t y)
{
#ifdef XU_CONFIG_LIBM_HAVE_POW
    return pow(x, y);
#else
    xu_assert(0);
    return 0;
#endif
}
