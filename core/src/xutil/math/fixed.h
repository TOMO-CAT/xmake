#pragma once

#include "xutil/math/fixed_16.h"
#include "xutil/prefix.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

#define XU_FIXED_ONE XU_FIXED16_ONE
#define XU_FIXED_HALF XU_FIXED16_HALF
#define XU_FIXED_MAX XU_FIXED16_MAX
#define XU_FIXED_MIN XU_FIXED16_MIN
#define XU_FIXED_NAN XU_FIXED16_NAN
#define XU_FIXED_INF XU_FIXED16_INF
#define XU_FIXED_PI XU_FIXED16_PI
#define XU_FIXED_SQRT2 XU_FIXED16_SQRT2
#define XU_FIXED_NEAR0 XU_FIXED16_NEAR0

// conversion
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#    define xu_fixed_to_float(x) xu_fixed16_to_float(x)
#    define xu_float_to_fixed(x) xu_float_to_fixed16(x)
#endif

#define xu_long_to_fixed(x) xu_long_to_fixed16(x)
#define xu_fixed_to_long(x) xu_fixed16_to_long(x)

#define xu_fixed6_to_fixed(x) xu_fixed6_to_fixed16(x)
#define xu_fixed_to_fixed6(x) xu_fixed16_to_fixed6(x)

#define xu_fixed30_to_fixed(x) xu_fixed30_to_fixed16(x)
#define xu_fixed_to_fixed30(x) xu_fixed16_to_fixed30(x)

// round
#define xu_fixed_round(x) xu_fixed16_round(x)
#define xu_fixed_ceil(x) xu_fixed16_ceil(x)
#define xu_fixed_floor(x) xu_fixed16_floor(x)

// nearly equal?
#define xu_fixed_near_eq(x, y) xu_fixed16_near_eq(x, y)

// operations
#define xu_fixed_abs(x) xu_fixed16_abs(x)
#define xu_fixed_avg(x, y) xu_fixed16_avg(x, y)
#define xu_fixed_lsh(x, y) xu_fixed16_lsh(x, y)
#define xu_fixed_rsh(x, y) xu_fixed16_rsh(x, y)
#define xu_fixed_mul(x, y) xu_fixed16_mul(x, y)
#define xu_fixed_div(x, y) xu_fixed16_div(x, y)
#define xu_fixed_imul(x, y) xu_fixed16_imul(x, y)
#define xu_fixed_idiv(x, y) xu_fixed16_idiv(x, y)
#define xu_fixed_imuldiv(x, y, z) xu_fixed16_imuldiv(x, y, z)
#define xu_fixed_imulsub(x, y, z) xu_fixed16_imulsub(x, y, z)
#define xu_fixed_invert(x) xu_fixed16_invert(x)
#define xu_fixed_sqre(x) xu_fixed16_sqre(x)
#define xu_fixed_sqrt(x) xu_fixed16_sqrt(x)
#define xu_fixed_sin(x) xu_fixed16_sin(x)
#define xu_fixed_cos(x) xu_fixed16_cos(x)
#define xu_fixed_sincos(x, s, c) xu_fixed16_sincos(x, s, c)
#define xu_fixed_tan(x) xu_fixed16_tan(x)
#define xu_fixed_asin(x) xu_fixed16_asin(x)
#define xu_fixed_acos(x) xu_fixed16_acos(x)
#define xu_fixed_atan(x) xu_fixed16_atan(x)
#define xu_fixed_atan2(y, x) xu_fixed16_atan2(y, x)
#define xu_fixed_exp(x) xu_fixed16_exp(x)
#define xu_fixed_exp1(x) xu_fixed16_exp1(x)
#define xu_fixed_expi(x) xu_fixed16_expi(x)
#define xu_fixed_ilog2(x) xu_fixed16_ilog2(x)
