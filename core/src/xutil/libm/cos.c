#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************s
 * implementation
 */
xu_double_t xu_cos(xu_double_t x)
{
#ifdef XU_CONFIG_LIBM_HAVE_COS
    return cos(x);
#else
    xu_assert(0);
    return 0;
#endif
}
