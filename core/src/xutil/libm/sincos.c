#include "xutil/libm/math.h"
#include <math.h>

/* *******************************************************
 * declaration
 */
#ifdef XU_CONFIG_LIBM_HAVE_SINCOS
extern xu_void_t sincos(xu_double_t x, xu_double_t* s, xu_double_t* c);
#endif

/* *******************************************************
 * implementation
 */
xu_void_t xu_sincos(xu_double_t x, xu_double_t* s, xu_double_t* c)
{
#ifdef XU_CONFIG_LIBM_HAVE_SINCOS
    sincos(x, s, c);
#else
    if (s) *s = xu_sin(x);
    if (c) *c = xu_cos(x);
#endif
}
