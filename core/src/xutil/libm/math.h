#pragma once

#include "xutil/prefix.h"
#include "xutil/macros/assert.h"
#include "xutil/utils/bits.h"

/* *******************************************************
 * interfaces
 */

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT

// is infinite?
xu_long_t xu_isinf(xu_double_t x);
xu_long_t xu_isinff(xu_float_t x);

// is finite?
xu_long_t xu_isfin(xu_double_t x);
xu_long_t xu_isfinf(xu_float_t x);

// is nan?
xu_long_t xu_isnan(xu_double_t x);
xu_long_t xu_isnanf(xu_float_t x);

// sqrt
xu_double_t xu_sqrt(xu_double_t x);
xu_float_t  xu_sqrtf(xu_float_t x);

// sin
xu_double_t xu_sin(xu_double_t x);
xu_float_t  xu_sinf(xu_float_t x);

// cos
xu_double_t xu_cos(xu_double_t x);
xu_float_t  xu_cosf(xu_float_t x);

// tan
xu_double_t xu_tan(xu_double_t x);
xu_float_t  xu_tanf(xu_float_t x);

// atan
xu_double_t xu_atan(xu_double_t x);
xu_float_t  xu_atanf(xu_float_t x);

// exp
xu_double_t xu_exp(xu_double_t x);
xu_float_t  xu_expf(xu_float_t x);

// expi
xu_double_t xu_expi(xu_long_t x);
xu_float_t  xu_expif(xu_long_t x);

// exp1
xu_double_t xu_exp1(xu_double_t x);
xu_float_t  xu_exp1f(xu_float_t x);

// asin
xu_double_t xu_asin(xu_double_t x);
xu_float_t  xu_asinf(xu_float_t x);

// acos
xu_double_t xu_acos(xu_double_t x);
xu_float_t  xu_acosf(xu_float_t x);

// atan2
xu_double_t xu_atan2(xu_double_t y, xu_double_t x);
xu_float_t  xu_atan2f(xu_float_t y, xu_float_t x);

// log2
xu_double_t xu_log2(xu_double_t x);
xu_float_t  xu_log2f(xu_float_t x);

// sincos
xu_void_t xu_sincos(xu_double_t x, xu_double_t* s, xu_double_t* c);
xu_void_t xu_sincosf(xu_float_t x, xu_float_t* s, xu_float_t* c);

// pow
xu_double_t xu_pow(xu_double_t x, xu_double_t y);
xu_float_t  xu_powf(xu_float_t x, xu_float_t y);

// fmod
xu_double_t xu_fmod(xu_double_t x, xu_double_t y);
xu_float_t  xu_fmodf(xu_float_t x, xu_float_t y);
#endif

// ilog2i
xu_uint32_t xu_ilog2i(xu_uint32_t x);

// isqrti
xu_uint32_t xu_isqrti(xu_uint32_t x);
xu_uint32_t xu_isqrti64(xu_uint64_t x);

// idivi8
xu_uint32_t xu_idivi8(xu_uint32_t x, xu_uint8_t y);
