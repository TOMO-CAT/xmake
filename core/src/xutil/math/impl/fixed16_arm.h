#pragma once

#include "xutil/prefix.h"

#ifdef XU_ASSEMBLER_IS_GAS
#    if 0
#        define xu_fixed16_mul(x, y) xu_fixed16_mul_asm(x, y)
#    endif
#endif /* XU_ASSEMBLER_IS_GAS */

#if defined(XU_ASSEMBLER_IS_GAS) && !defined(XU_ARCH_ARM64)
static __xu_inline__ xu_fixed16_t xu_fixed16_mul_asm(xu_fixed16_t x, xu_fixed16_t y)
{
    __xu_register__ xu_fixed16_t t;
    __xu_asm__                   __xu_volatile__("smull  %0, %2, %1, %3          \n" // r64 = (l, h) = x * y
                                                 "mov    %0, %0, lsr #16         \n" // to fixed16: r64 >>= 16
                                                 "orr    %0, %0, %2, lsl #16     \n" // x = l = (h << (32 - 16)) | (l >> 16);

                                                 : "=r"(x), "=&r"(y), "=r"(t)
                                                 : "r"(x), "1"(y));
    return x;
}
#endif
