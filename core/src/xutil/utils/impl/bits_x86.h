#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 */
#ifdef XU_ASSEMBLER_IS_GAS
// swap
#    ifndef xu_bits_swap_u16
#        define xu_bits_swap_u16(x) xu_bits_swap_u16_asm(x)
#    endif
#    ifndef xu_bits_swap_u32
#        define xu_bits_swap_u32(x) xu_bits_swap_u32_asm(x)
#    endif
#    ifndef xu_bits_swap_u64
#        define xu_bits_swap_u64(x) xu_bits_swap_u64_asm(x)
#    endif
#endif

/* *******************************************************
 *  interfaces
 */
#ifdef XU_ASSEMBLER_IS_GAS
// swap
static __xu_inline__ xu_uint16_t xu_bits_swap_u16_asm(xu_uint16_t x)
{
    __xu_asm__ __xu_volatile__("rorw    $8, %w0" : "+r"(x));
    return x;
}

static __xu_inline__ xu_uint32_t xu_bits_swap_u32_asm(xu_uint32_t x)
{
#    if 1
    __xu_asm__ __xu_volatile__("bswap   %0" : "+r"(x));
#    else
    __xu_asm__ __xu_volatile__("rorw   $8,  %w0 \n" //!< swap low 16 bits
                               "rorl   $16, %0  \n" //!< swap x by word
                               "rorw   $8,  %w0"    //!< swap low 16 bits

                               : "+r"(x));
#    endif
    return x;
}

static __xu_inline__ xu_hize_t xu_bits_swap_u64_asm(xu_hize_t x)
{
    __xu_register__ xu_size_t esi, edi;
    __xu_asm__                __xu_volatile__("lodsl\n"
                                                             "bswap  %%eax\n"
                                                             "movl   %%eax, %%ebx\n"
                                                             "lodsl\n"
                                                             "bswap  %%eax\n"
                                                             "stosl\n"
                                                             "movl   %%ebx, %%eax\n"
                                                             "stosl\n"

                                              : "=&S"(esi), "=&D"(edi)
                                              : "0"(&x), "1"(&x)
                                              : "memory", "eax", "ebx");
    return x;
}
#endif
