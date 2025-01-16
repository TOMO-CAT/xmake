#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */
// swap
#ifndef xu_bits_swap_u16
#    define xu_bits_swap_u16(x) xu_bits_swap_u16_asm(x)
#endif
#ifndef xu_bits_swap_u32
#    define xu_bits_swap_u32(x) xu_bits_swap_u32_asm(x)
#endif

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
// swap
static __xu_inline__ xu_uint16_t const xu_bits_swap_u16_asm(xu_uint16_t x)
{
    __xu_asm__("swap.b %0,%0" : "+r"(x));
    return x;
}

static __xu_inline__ xu_uint32_t const xu_bits_swap_u32_asm(xu_uint32_t x)
{
    __xu_asm__("swap.b %0,%0\n"
               "swap.w %0,%0\n"
               "swap.b %0,%0\n"
               : "+r"(x));
    return x;
}
