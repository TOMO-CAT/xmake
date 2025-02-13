#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

#if defined(XU_COMPILER_IS_GCC) && XU_COMPILER_VERSION_BE(3, 3)
#    define XU_MAF (__builtin_huge_val())
#elif defined(XU_COMPILER_IS_GCC) && XU_COMPILER_VERSION_BE(2, 96)
#    define XU_MAF (__extension__ 0x1.0p2047)
#elif defined(XU_COMPILER_IS_GCC)
#    define XU_MAF                                                                                                     \
        (__extension__((union {                                                                                        \
             unsigned    __l __attribute__((__mode__(__DI__)));                                                        \
             xu_double_t __d;                                                                                          \
         }){__l : 0x7ff0000000000000ULL})                                                                              \
             .__d)
#else
typedef union
{
    xu_byte_t   __c[8];
    xu_double_t __d;
} __xu_maf_t;
#    ifdef XU_WORDS_BIGENDIAN
#        define __xu_maf_bytes                                                                                         \
            {                                                                                                          \
                0x7f, 0xf0, 0, 0, 0, 0, 0, 0                                                                           \
            }
#    else
#        define __xu_maf_bytes                                                                                         \
            {                                                                                                          \
                0, 0, 0, 0, 0, 0, 0xf0, 0x7f                                                                           \
            }
#    endif
static __xu_maf_t __xu_maf = {__xu_maf_bytes};
#    define XU_MAF (__xu_maf.__d)
#endif
