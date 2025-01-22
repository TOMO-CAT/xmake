#pragma once

#include "xutil/libm/libm.h"
#include "xutil/macros/assert.h"
#include "xutil/prefix.h"
#ifdef XU_ARCH_ARM
#    include "xutil/math/impl/fixed16_arm.h"
#endif

/* *******************************************************
 * macros
 */

// constant
#define XU_FIXED16_ONE (1 << 16)
#define XU_FIXED16_HALF (1 << 15)
#define XU_FIXED16_MAX (XU_MAXS32)
#define XU_FIXED16_MIN (XU_MINS32)
#define XU_FIXED16_NAN ((xu_int_t)0x80000000)
#define XU_FIXED16_INF (XU_MAXS32)
#define XU_FIXED16_PI (0x3243f)
#define XU_FIXED16_SQRT2 (92682)
#define XU_FIXED16_NEAR0 (1 << 4)

// conversion
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#    ifndef xu_fixed16_to_float
#        define xu_fixed16_to_float(x) ((xu_float_t)((x)*1.5258789e-5))
#    endif
#    ifndef xu_float_to_fixed16
#        define xu_float_to_fixed16(x) ((xu_fixed16_t)((x)*XU_FIXED16_ONE))
#    endif
#endif

#ifdef __xu_debug__
#    define xu_long_to_fixed16(x) xu_long_to_fixed16_check(x)
#    define xu_fixed16_to_long(x) xu_fixed16_to_long_check(x)
#else
#    define xu_long_to_fixed16(x) (xu_fixed16_t)((x) << 16)
#    define xu_fixed16_to_long(x) (xu_long_t)((x) >> 16)
#endif

// round
#define xu_fixed16_round(x) (((x) + XU_FIXED16_HALF) >> 16)

// ceil
#define xu_fixed16_ceil(x) (((x) + XU_FIXED16_ONE - 1) >> 16)

// floor
#define xu_fixed16_floor(x) ((x) >> 16)

// abs
#define xu_fixed16_abs(x) xu_abs(x)

// avg
#define xu_fixed16_avg(x, y) (((x) + (y)) >> 1)

// nearly equal?
#define xu_fixed16_near_eq(x, y) (xu_fixed16_abs((x) - (y)) <= XU_FIXED16_NEAR0)

// mul
#ifndef xu_fixed16_mul
#    ifdef __xu_debug__
#        define xu_fixed16_mul(x, y) xu_fixed16_mul_check(x, y)
#    else
#        define xu_fixed16_mul(x, y) ((xu_fixed16_t)(((xu_hong_t)(x) * (y)) >> 16))
#    endif
#endif

// div
#ifndef xu_fixed16_div
#    ifdef __xu_debug__
#        define xu_fixed16_div(x, y) xu_fixed16_div_check(x, y)
#    else
#        define xu_fixed16_div(x, y) ((xu_fixed16_t)((((xu_hong_t)(x)) << 16) / (y)))
#    endif
#endif

// imul
#ifndef xu_fixed16_imul
#    ifdef __xu_debug__
#        define xu_fixed16_imul(x, y) xu_fixed16_imul_check(x, y)
#    else
#        define xu_fixed16_imul(x, y) ((xu_fixed16_t)((x) * (y)))
#    endif
#endif

// idiv
#ifndef xu_fixed16_idiv
#    ifdef __xu_debug__
#        define xu_fixed16_idiv(x, y) xu_fixed16_idiv_check(x, y)
#    else
#        define xu_fixed16_idiv(x, y) ((xu_fixed16_t)((x) / (y)))
#    endif
#endif

// imuldiv
#ifndef xu_fixed16_imuldiv
#    ifdef __xu_debug__
#        define xu_fixed16_imuldiv(x, y, z) xu_fixed16_imuldiv_check(x, y, z)
#    else
#        define xu_fixed16_imuldiv(x, y, z) ((xu_fixed16_t)(((xu_hong_t)(x) * (y)) / (z)))
#    endif
#endif

// imulsub
#ifndef xu_fixed16_imulsub
#    ifdef __xu_debug__
#        define xu_fixed16_imulsub(x, y, z) xu_fixed16_imulsub_check(x, y, z)
#    else
#        define xu_fixed16_imulsub(x, y, z) ((xu_fixed16_t)(((xu_hong_t)(x) * (y)) - (z)))
#    endif
#endif

// lsh
#ifndef xu_fixed16_lsh
#    define xu_fixed16_lsh(x, y) ((x) << (y))
#endif

// rsh
#ifndef xu_fixed16_rsh
#    define xu_fixed16_rsh(x, y) ((x) >> (y))
#endif

// invert: 1 / x
#ifndef xu_fixed16_invert
#    define xu_fixed16_invert(x) xu_fixed16_div(XU_FIXED16_ONE, x)
#endif

// sqre
#ifndef xu_fixed16_sqre
#    ifdef __xu_debug__
#        define xu_fixed16_sqre(x) xu_fixed16_sqre_check(x)
#    else
#        define xu_fixed16_sqre(x) ((xu_fixed16_t)(((xu_hong_t)(x) * (x)) >> 16))
#    endif
#endif

// sqrt
#ifndef xu_fixed16_sqrt
#    define xu_fixed16_sqrt(x) xu_fixed16_sqrt_int32(x)
#endif

// sin
#ifndef xu_fixed16_sin
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_sin(x) xu_fixed16_sin_float(x)
#    else
#        define xu_fixed16_sin(x) xu_fixed16_sin_int32(x)
#    endif
#endif

// cos
#ifndef xu_fixed16_cos
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_cos(x) xu_fixed16_cos_float(x)
#    else
#        define xu_fixed16_cos(x) xu_fixed16_cos_int32(x)
#    endif
#endif

// sincos
#ifndef xu_fixed16_sincos
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_sincos(x, s, c) xu_fixed16_sincos_float(x, s, c)
#    else
#        define xu_fixed16_sincos(x, s, c) xu_fixed16_sincos_int32(x, s, c)
#    endif
#endif

// tan
#ifndef xu_fixed16_tan
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_tan(x) xu_fixed16_tan_float(x)
#    else
#        define xu_fixed16_tan(x) xu_fixed16_tan_int32(x)
#    endif
#endif

// asin
#ifndef xu_fixed16_asin
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_asin(x) xu_fixed16_asin_float(x)
#    else
#        define xu_fixed16_asin(x) xu_fixed16_asin_int32(x)
#    endif
#endif

// acos
#ifndef xu_fixed16_acos
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_acos(x) xu_fixed16_acos_float(x)
#    else
#        define xu_fixed16_acos(x) xu_fixed16_acos_int32(x)
#    endif
#endif

// atan
#ifndef xu_fixed16_atan
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_atan(x) xu_fixed16_atan_float(x)
#    else
#        define xu_fixed16_atan(x) xu_fixed16_atan_int32(x)
#    endif
#endif

// atan2
#ifndef xu_fixed16_atan2
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_atan2(y, x) xu_fixed16_atan2_float(y, x)
#    else
#        define xu_fixed16_atan2(y, x) xu_fixed16_atan2_int32(y, x)
#    endif
#endif

// exp
#ifndef xu_fixed16_exp
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_exp(x) xu_fixed16_exp_float(x)
#    else
#        define xu_fixed16_exp(x) xu_fixed16_exp_int32(x)
#    endif
#endif

#ifndef xu_fixed16_expi
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_expi(x) xu_fixed16_expi_float(x)
#    else
#        define xu_fixed16_expi(x) xu_assert(0)
#    endif
#endif

#ifndef xu_fixed16_exp1
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#        define xu_fixed16_exp1(x) xu_fixed16_exp1_float(x)
#    else
#        define xu_fixed16_exp1(x) xu_assert(0)
#    endif
#endif

// log
#ifndef xu_fixed16_ilog2
#    define xu_fixed16_ilog2(x) xu_fixed16_ilog2_int32(x)
#endif

/* *******************************************************
 * interfaces
 */

/*! compute the invert of the fixed-point value
 *
 * @param x     the fixed-point x-value
 *
 * @return      the result
 */
xu_fixed16_t xu_fixed16_invert_int32(xu_fixed16_t x);

/*! compute the sin and cos value of the fixed-point value
 *
 * @param x     the fixed-point x-value
 * @param s     the sin fixed-point value
 * @param c     the cos fixed-point value
 */
xu_void_t xu_fixed16_sincos_int32(xu_fixed16_t x, xu_fixed16_t* s, xu_fixed16_t* c);

/*! compute the atan2 value of the fixed-point value
 *
 * @param y     the fixed-point y-value
 * @param x     the fixed-point x-value
 *
 * @return      the result
 */
xu_fixed16_t xu_fixed16_atan2_int32(xu_fixed16_t y, xu_fixed16_t x);

/*! compute the asin value of the fixed-point value
 *
 * @param x     the fixed-point x-value
 *
 * @return      the result
 */
xu_fixed16_t xu_fixed16_asin_int32(xu_fixed16_t x);

/*! compute the atan value of the fixed-point value
 *
 * @param x     the fixed-point x-value
 *
 * @return      the result
 */
xu_fixed16_t xu_fixed16_atan_int32(xu_fixed16_t x);

/*! compute the exp value of the fixed-point value
 *
 * @param x     the fixed-point x-value
 *
 * @return      the result
 */
xu_fixed16_t xu_fixed16_exp_int32(xu_fixed16_t x);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __xu_inline__ xu_fixed16_t xu_long_to_fixed16_check(xu_long_t x)
{
    // check overflow
    xu_assert(x == (xu_int16_t)x);

    // ok
    return (xu_fixed16_t)(x << 16);
}
static __xu_inline__ xu_long_t xu_fixed16_to_long_check(xu_fixed16_t x)
{
    // check overflow
    xu_assert(x >= XU_FIXED16_MIN && x <= XU_FIXED16_MAX);

    // ok
    return (x >> 16);
}
static __xu_inline__ xu_fixed16_t xu_fixed16_mul_check(xu_fixed16_t x, xu_fixed16_t y)
{
    // done
    xu_hong_t v = (((xu_hong_t)x * y) >> 16);

    // check overflow
    xu_assert(v == (xu_int32_t)v);

    // ok
    return (xu_fixed16_t)v;
}
static __xu_inline__ xu_fixed16_t xu_fixed16_div_check(xu_fixed16_t x, xu_fixed16_t y)
{
    // check
    xu_assert(y);

    // done
    xu_hong_t v = ((((xu_hong_t)x) << 16) / y);

    // check overflow
    xu_assert(v == (xu_int32_t)v);

    // ok
    return (xu_fixed16_t)v;
}
static __xu_inline__ xu_fixed16_t xu_fixed16_sqre_check(xu_fixed16_t x)
{
    // done
    xu_hong_t v = (((xu_hong_t)x * x) >> 16);

    // check overflow
    xu_assert(v == (xu_int32_t)v);

    // ok
    return (xu_fixed16_t)v;
}
static __xu_inline__ xu_fixed16_t xu_fixed16_imul_check(xu_fixed16_t x, xu_long_t y)
{
    // done
    xu_hong_t v = ((xu_hong_t)x * y);

    // check overflow
    xu_assert(v == (xu_int32_t)v);

    // ok
    return (xu_fixed16_t)v;
}
static __xu_inline__ xu_fixed16_t xu_fixed16_idiv_check(xu_fixed16_t x, xu_long_t y)
{
    // check
    xu_assert(y);

    // ok
    return (xu_fixed16_t)(x / y);
}
static __xu_inline__ xu_fixed16_t xu_fixed16_imuldiv_check(xu_fixed16_t x, xu_long_t y, xu_long_t z)
{
    // done
    xu_hong_t v = ((xu_hong_t)x * y) / z;

    // check overflow
    xu_assert(v == (xu_int32_t)v);

    // ok
    return (xu_fixed16_t)v;
}
static __xu_inline__ xu_fixed16_t xu_fixed16_imulsub_check(xu_fixed16_t x, xu_long_t y, xu_long_t z)
{
    // done
    xu_hong_t v = ((xu_hong_t)x * y) - z;

    // check overflow
    xu_assert(v == (xu_int32_t)v);

    // ok
    return (xu_fixed16_t)v;
}
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
static __xu_inline__ xu_fixed16_t xu_fixed16_sin_float(xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_sinf(xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_fixed16_t xu_fixed16_cos_float(xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_cosf(xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_void_t xu_fixed16_sincos_float(xu_fixed16_t x, xu_fixed16_t* s, xu_fixed16_t* c)
{
    xu_float_t sf, cf;
    xu_sincosf(xu_fixed16_to_float(x), &sf, &cf);
    if (s) *s = xu_float_to_fixed16(sf);
    if (s) *c = xu_float_to_fixed16(cf);
}
static __xu_inline__ xu_fixed16_t xu_fixed16_tan_float(xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_tanf(xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_fixed16_t xu_fixed16_asin_float(xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_asinf(xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_fixed16_t xu_fixed16_acos_float(xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_acosf(xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_fixed16_t xu_fixed16_atan_float(xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_atanf(xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_fixed16_t xu_fixed16_atan2_float(xu_fixed16_t y, xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_atan2f(xu_fixed16_to_float(y), xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_fixed16_t xu_fixed16_exp_float(xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_expf(xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_fixed16_t xu_fixed16_exp1_float(xu_fixed16_t x)
{
    return xu_float_to_fixed16(xu_exp1f(xu_fixed16_to_float(x)));
}
static __xu_inline__ xu_fixed16_t xu_fixed16_expi_float(xu_long_t x) { return xu_float_to_fixed16(xu_expif(x)); }
#endif
static __xu_inline__ xu_fixed16_t xu_fixed16_sqrt_int32(xu_fixed16_t x)
{
    xu_assert(x > 0);
    return (x > 0 ? (xu_isqrti(x) << 8) : 0);
}
static __xu_inline__ xu_uint32_t xu_fixed16_ilog2_int32(xu_fixed16_t x)
{
    xu_assert(x > 0);
    xu_uint32_t lg = xu_ilog2i(x);
    return (lg > 16 ? (lg - 16) : 0);
}
static __xu_inline__ xu_fixed16_t xu_fixed16_sin_int32(xu_fixed16_t x)
{
    xu_fixed16_t s = 0;
    xu_fixed16_sincos_int32(x, &s, xu_null);
    return s;
}
static __xu_inline__ xu_fixed16_t xu_fixed16_cos_int32(xu_fixed16_t x)
{
    xu_fixed16_t c = 0;
    xu_fixed16_sincos_int32(x, xu_null, &c);
    return c;
}
static __xu_inline__ xu_fixed16_t xu_fixed16_tan_int32(xu_fixed16_t x)
{
    xu_fixed16_t s = 0;
    xu_fixed16_t c = 0;
    xu_fixed16_sincos_int32(x, &s, &c);
    return xu_fixed16_div(s, c);
}
static __xu_inline__ xu_fixed16_t xu_fixed16_acos_int32(xu_fixed16_t x)
{
    // asin + acos = pi / 2
    xu_fixed16_t z = xu_fixed16_asin_int32(x);
    return ((XU_FIXED16_PI >> 1) - z);
}
