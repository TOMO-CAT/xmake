#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_double_t xu_sin(xu_double_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_SIN
    return sin(x);
#else
    xu_assert(0);
    return 0;
#endif
}
