#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_float_t xu_cosf(xu_float_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_COSF
    return cosf(x);
#else
    xu_assert(0);
    return 0;
#endif
}
