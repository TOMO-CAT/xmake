#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_double_t xu_sqrt(xu_double_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_SQRT
    return sqrt(x);
#else
    xu_assert(0);
    return 0;
#endif
}
