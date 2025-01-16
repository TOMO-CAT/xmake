#pragma once

#include "xutil/prefix.h"

#if defined(XU_COMPILER_IS_GCC) && XU_COMPILER_VERSION_BE(3, 3)
#    define XU_NAN (__builtin_nanf(""))
#elif defined(XU_COMPILER_IS_GCC)
#    define XU_NAN                                                                                                     \
        (__extension__((union {                                                                                        \
             unsigned   __l __attribute__((__mode__(__SI__)));                                                         \
             xu_float_t __d;                                                                                           \
         }){__l : 0x7fc00000UL})                                                                                       \
             .__d)
#else
#    ifdef XU_WORDS_BIGENDIAN
#        define __xu_nan_bytes                                                                                         \
            {                                                                                                          \
                0x7f, 0xc0, 0, 0                                                                                       \
            }
#    else
#        define __xu_nan_bytes                                                                                         \
            {                                                                                                          \
                0, 0, 0xc0, 0x7f                                                                                       \
            }
#    endif
static union
{
    xu_byte_t  __c[4];
    xu_float_t __d;
} __xu_nan_union = {__xu_nan_bytes};
#    define XU_NAN (__xu_nan_union.__d)
#endif
