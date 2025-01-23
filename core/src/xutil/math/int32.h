#pragma once

#include "xutil/macros/assert.h"
#include "xutil/prefix.h"
#include "xutil/utils/bits.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * macros
 */

// sign
#define xu_int32_get_sign(x) xu_int32_get_sign_inline(x)
#define xu_int32_set_sign(x, s) xu_int32_set_sign_inline(x, s)

// bool: is true?
#define xu_int32_nz(x) xu_int32_nz_inline(x)

/* *******************************************************
 * interfaces
 */

// div
xu_int32_t xu_int32_div(xu_int32_t x, xu_int32_t y, xu_int_t nbits);

/* *******************************************************
 * inline
 */

// return -1 if x < 0, else return 0
static __xu_inline__ xu_int32_t xu_int32_get_sign_inline(xu_int32_t x)
{
    xu_int32_t s = ((xu_int32_t)(x) >> 31);
    xu_assert((x < 0 && s == -1) || (x >= 0 && !s));
    return s;
}
// if s == -1, return -x, else s must be 0, and return x.
static __xu_inline__ xu_int32_t xu_int32_set_sign_inline(xu_int32_t x, xu_int32_t s)
{
    xu_assert(s == 0 || s == -1);
    return (x ^ s) - s;
}
// non zero, return 1 if x != 0, else return 0
static __xu_inline__ xu_long_t xu_int32_nz_inline(xu_uint32_t x)
{
    // return (x? 1 : 0);
    return ((x | (0 - x)) >> 31);
}
