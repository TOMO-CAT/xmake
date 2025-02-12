#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************s
 * implementation
 */
xu_void_t xu_sincosf(xu_float_t x, xu_float_t* s, xu_float_t* c)
{
#ifdef XU_CONFIG_LIBM_HAVE_SINCOSF
    sincosf(x, s, c);
#else
    if (s) *s = xu_sinf(x);
    if (c) *c = xu_cosf(x);
#endif
}
