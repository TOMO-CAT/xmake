#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 */

#if defined(XU_ASSEMBLER_IS_GAS) && !defined(XU_ARCH_ARM64)
// swap
#    if XU_ARCH_ARM_VERSION >= 6
#        ifndef xu_bits_swap_u16
#            define xu_bits_swap_u16(x) xu_bits_swap_u16_asm(x)
#        endif
#    endif
#    ifndef xu_bits_swap_u32
#        define xu_bits_swap_u32(x) xu_bits_swap_u32_asm(x)
#    endif
#endif

/* *******************************************************
 *  interfaces
 */
#if defined(XU_ASSEMBLER_IS_GAS) && !defined(XU_ARCH_ARM64)

// swap
#    if (XU_ARCH_ARM_VERSION >= 6)
static __xu_inline__ xu_uint16_t const xu_bits_swap_u16_asm(xu_uint16_t x)
{
    __xu_asm__ __xu_volatile__("rev16 %0, %0" : "+r"(x));
    return x;
}
#    endif

static __xu_inline__ xu_uint32_t const xu_bits_swap_u32_asm(xu_uint32_t x)
{
#    if (XU_ARCH_ARM_VERSION >= 6)
    __xu_asm__("rev %0, %0" : "+r"(x));
#    else
    __xu_register__ xu_uint32_t t;
    __xu_asm__                  __xu_volatile__("eor %1, %0, %0, ror #16 \n"
                                                                 "bic %1, %1, #0xff0000   \n"
                                                                 "mov %0, %0, ror #8      \n"
                                                                 "eor %0, %0, %1, lsr #8  \n"
                                                : "+r"(x), "=&r"(t));
#    endif
    return x;
}

#    if 0
static __xu_inline__ xu_uint32_t xu_bits_get_ubits32_impl_asm(xu_byte_t const* p, xu_size_t b, xu_size_t n)
{
#        ifdef __xu_small__
    __xu_register__ xu_uint32_t x;
    __xu_asm__ __xu_volatile__
    (
        "ldrb r6, [%1], #1\n"
        "ldrb r7, [%1], #1\n"
        "ldrb r8, [%1], #1\n"
        "ldrb r9, [%1], #1\n"
        "ldrb %1, [%1]\n"
        "orr %0, r6, lsl %2\n"
        "sub %2, %2, #8\n"
        "cmp %3, #8\n"
        "orrhi %0, r7, lsl %2\n"
        "sub %2, %2, #8\n"
        "cmp %3, #16\n"
        "orrhi %0, r8, lsl %2\n"
        "sub %2, %2, #8\n"
        "cmp %3, #24\n"
        "orrhi %0, r9, lsl %2\n"
        "rsb %2, %2, #8\n"
        "cmp %3, #32\n"
        "orrhi %0, %1, lsr %2\n"
        "rsb %4, %4, #32\n"
        "mov %0, %0, lsr %4\n"


        : "=&r"(x)
        : "r"(p), "r"(b + 24), "r"(b + n), "r"(n), "0"(0)
        : "r6", "r7", "r8", "r9"
    );

    return x;
#        else
    __xu_register__ xu_uint32_t x;
    __xu_asm__ __xu_volatile__
    (
        "   cmp %3, #32\n"
        "   bhi 1f\n"
        "   cmp %3, #24\n"
        "   bhi 2f\n"
        "   cmp %3, #16\n"
        "   bhi 3f\n"
        "   cmp %3, #8\n"
        "   bhi 4f\n"
        "   ldrb %1, [%1]\n"
        "   orr %0, %1, lsl %2\n"
        "   b   5f\n"
        "1:\n"
        "   ldrb r6, [%1], #1\n"
        "   ldrb r7, [%1], #1\n"
        "   ldrb r8, [%1], #1\n"
        "   ldrb r9, [%1], #1\n"
        "   ldrb %1, [%1]\n"
        "   orr %0, r6, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r7, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r8, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r9, lsl %2\n"
        "   rsb %2, %2, #8\n"
        "   orr %0, %1, lsr %2\n"
        "   b   5f\n"
        "2:\n"
        "   ldrb r6, [%1], #1\n"
        "   ldrb r7, [%1], #1\n"
        "   ldrb r8, [%1], #1\n"
        "   ldrb r9, [%1], #1\n"
        "   orr %0, r6, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r7, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r8, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r9, lsl %2\n"
        "   b   5f\n"
        "3:\n"
        "   ldrb r6, [%1], #1\n"
        "   ldrb r7, [%1], #1\n"
        "   ldrb r8, [%1], #1\n"
        "   orr %0, r6, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r7, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r8, lsl %2\n"
        "   b   5f\n"
        "4:\n"
        "   ldrb r6, [%1], #1\n"
        "   ldrb r7, [%1], #1\n"
        "   orr %0, r6, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r7, lsl %2\n"
        "5:\n"
        "   rsb %4, %4, #32\n"
        "   mov %0, %0, lsr %4\n"

        : "=&r"(x)
        : "r"(p), "r"(b + 24), "r"(b + n), "r"(n), "0"(0)
        : "r6", "r7", "r8", "r9"
    );

    return x;
#        endif
}
#    endif

#endif /* XU_ASSEMBLER_IS_GAS */
