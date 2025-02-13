#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * implementation
 */
xu_double_t xu_tan(xu_double_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_TAN
    return tan(x);
#else
    xu_assert(0);
    return 0;
#endif
}
