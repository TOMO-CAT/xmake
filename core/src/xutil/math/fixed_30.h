#pragma once

#include "xutil/macros/assert.h"
#include "xutil/math/int32.h"
#include "xutil/prefix.h"

#ifdef XU_ARCH_ARM
#    include "xutil/math/impl/fixed16_arm.h"
#endif

/* *******************************************************
 * macros
 */

// constant
#define XU_FIXED30_ONE (1 << 30)
#define XU_FIXED30_HALF (1 << 29)
#define XU_FIXED30_MAX (XU_MAXS32)
#define XU_FIXED30_MIN (-XU_FIXED30_MAX)
#define XU_FIXED30_NAN ((xu_int_t)0x80000000)
#define XU_FIXED30_INF (XU_MAXS32)
#define XU_FIXED30_SQRT2 (0x5a827999)

// conversion
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#    ifndef xu_fixed30_to_float
#        define xu_fixed30_to_float(x) (((x)*0.00000000093132257f))
#    endif
#    ifndef xu_float_to_fixed30
#        ifdef __xu_debug__
#            define xu_float_to_fixed30(x) xu_float_to_fixed30_check(x)
#        else
#            define xu_float_to_fixed30(x) ((xu_fixed30_t)((x)*XU_FIXED30_ONE))
#        endif
#    endif
#endif

#ifdef __xu_debug__
#    define xu_fixed16_to_fixed30(x) xu_fixed16_to_fixed30_check(x)
#else
#    define xu_fixed16_to_fixed30(x) ((x) << 14)
#endif
#define xu_fixed30_to_fixed16(x) ((x) >> 14)

// abs
#define xu_fixed30_abs(x) xu_abs(x)

// avg
#define xu_fixed30_avg(x, y) (((x) + (y)) >> 1)

// mul
#ifndef xu_fixed30_mul
#    if 1
#        define xu_fixed30_mul(x, y) xu_fixed30_mul_int64(x, y)
#    elif defined(XU_CONFIG_TYPE_HAVE_FLOAT)
#        define xu_fixed30_mul(x, y) xu_fixed30_mul_float(x, y)
#    else
#        define xu_fixed30_mul(x, y) xu_fixed30_mul_int32(x, y)
#    endif
#endif

// div
#ifndef xu_fixed30_div
#    if 1
#        define xu_fixed30_div(x, y) xu_fixed30_div_int64(x, y)
#    elif defined(XU_CONFIG_TYPE_HAVE_FLOAT)
#        define xu_fixed30_div(x, y) xu_fixed30_div_float(x, y)
#    else
#        define xu_fixed30_div(x, y) xu_int32_div(x, y, 30)
#    endif
#endif

// sqre
#ifndef xu_fixed30_sqre
#    if 1
#        define xu_fixed30_sqre(x) xu_fixed30_sqre_int64(x)
#    elif defined(XU_CONFIG_TYPE_HAVE_FLOAT)
#        define xu_fixed30_sqre(x) xu_fixed30_sqre_float(x)
#    else
#        define xu_fixed30_sqre(x) xu_fixed30_sqre_int32(x)
#    endif
#endif

// sqrt
#ifndef xu_fixed30_sqrt
#    define xu_fixed30_sqrt(x) xu_fixed30_sqrt_int32(x)
#endif

/* *******************************************************
 * inlines
 */

#ifdef __xu_debug__
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
static __xu_inline__ xu_fixed30_t xu_float_to_fixed30_check(xu_float_t x)
{
    // check overflow, [-2., 2.]
    xu_assert(x >= -2. && x <= 2.);
    return ((xu_fixed30_t)((x)*XU_FIXED30_ONE));
}
#    endif
static __xu_inline__ xu_fixed30_t xu_fixed16_to_fixed30_check(xu_fixed16_t x)
{
    // check overflow, [-2, 2]
    xu_assert((x >> 16) >= -2 && (x >> 16) <= 2);
    return (x << 14);
}
#endif

static __xu_inline__ xu_fixed30_t xu_fixed30_mul_int64(xu_fixed30_t x, xu_fixed30_t y)
{
    return (xu_fixed30_t)((xu_hong_t)x * y >> 30);
}
static __xu_inline__ xu_fixed30_t xu_fixed30_div_int64(xu_fixed30_t x, xu_fixed30_t y)
{
    xu_assert(y);
    return (xu_fixed30_t)((((xu_hong_t)x) << 30) / y);
}
static __xu_inline__ xu_fixed30_t xu_fixed30_sqre_int64(xu_fixed30_t x)
{
    return (xu_fixed30_t)((xu_hong_t)x * x >> 30);
}

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
static __xu_inline__ xu_fixed30_t xu_fixed30_mul_float(xu_fixed30_t x, xu_fixed30_t y)
{
    xu_float_t f = xu_fixed30_to_float(x) * xu_fixed30_to_float(y);
    return xu_float_to_fixed30(f);
}
static __xu_inline__ xu_fixed30_t xu_fixed30_div_float(xu_fixed30_t x, xu_fixed30_t y)
{
    xu_assert(y);
    return xu_float_to_fixed30((xu_float_t)x / y);
}
static __xu_inline__ xu_fixed30_t xu_fixed30_sqre_float(xu_fixed30_t x)
{
    xu_float_t f = xu_fixed30_to_float(x);
    f *= f;
    return xu_float_to_fixed30(f);
}
#endif

static __xu_inline__ xu_fixed30_t xu_fixed30_mul_int32(xu_fixed30_t x, xu_fixed30_t y)
{
    // get sign
    xu_int32_t s = xu_int32_get_sign(x ^ y);
    x            = xu_fixed30_abs(x);
    y            = xu_fixed30_abs(y);

    xu_uint32_t xh = x >> 16;
    xu_uint32_t xl = x & 0xffff;
    xu_uint32_t yh = y >> 16;
    xu_uint32_t yl = y & 0xffff;

    xu_uint32_t xyh  = xh * yh;
    xu_uint32_t xyl  = xl * yl;
    xu_uint32_t xyhl = xh * yl + xl * yh;

    xu_uint32_t lo = xyl + (xyhl << 16);
    xu_uint32_t hi = xyh + (xyhl >> 16) + (lo < xyl);

    // check overflow
    xu_assert(!(hi >> 29));

    xu_uint32_t r = (hi << 2) + (lo >> 30);
    return xu_int32_set_sign(r, s);
}

static __xu_inline__ xu_fixed30_t xu_fixed30_sqre_int32(xu_fixed30_t x)
{
    x = xu_fixed30_abs(x);

    xu_uint32_t xh = x >> 16;
    xu_uint32_t xl = x & 0xffff;

    xu_uint32_t xxh  = xh * xh;
    xu_uint32_t xxl  = xl * xl;
    xu_uint32_t xxhl = (xh * xl) << 1;

    xu_uint32_t lo = xxl + (xxhl << 16);
    xu_uint32_t hi = xxh + (xxhl >> 16) + (lo < xxl);

    // check overflow
    xu_assert(!(hi >> 29));

    return ((hi << 2) + (lo >> 30));
}
static __xu_inline__ xu_fixed30_t xu_fixed30_sqrt_int32(xu_fixed30_t x)
{
    xu_assert(x > 0);
    return (x > 0 ? (xu_isqrti(x) << 15) : 0);
}
