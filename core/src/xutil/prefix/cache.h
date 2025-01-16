#pragma once

#include "xutil/prefix.h"

// the cpu L1 cache shift, default: (1 << 5) == 32 bytes
#ifndef XU_L1_CACHE_SHIFT
#    define XU_L1_CACHE_SHIFT (5)
#endif

// the cpu L1 cache bytes, default: 32 bytes
#ifndef XU_L1_CACHE_BYTES
#    define XU_L1_CACHE_BYTES (1 << XU_L1_CACHE_SHIFT)
#endif

// the smp cache bytes
#ifndef XU_SMP_CACHE_BYTES
#    define XU_SMP_CACHE_BYTES XU_L1_CACHE_BYTES
#endif

// the cacheline aligned keyword
#ifndef __xu_cacheline_aligned__
#    if defined(XU_COMPILER_IS_GCC)
#        define __xu_cacheline_aligned__ __attribute__((__aligned__(XU_SMP_CACHE_BYTES)))
#    elif defined(XU_COMPILER_IS_MSVC)
#        if XU_SMP_CACHE_BYTES == 4
#            define __xu_cacheline_aligned__ __declspec(align(4))
#        elif XU_SMP_CACHE_BYTES == 8
#            define __xu_cacheline_aligned__ __declspec(align(8))
#        elif XU_SMP_CACHE_BYTES == 16
#            define __xu_cacheline_aligned__ __declspec(align(16))
#        elif XU_SMP_CACHE_BYTES == 32
#            define __xu_cacheline_aligned__ __declspec(align(32))
#        elif XU_SMP_CACHE_BYTES == 64
#            define __xu_cacheline_aligned__ __declspec(align(64))
#        elif XU_SMP_CACHE_BYTES == 128
#            define __xu_cacheline_aligned__ __declspec(align(128))
#        elif XU_SMP_CACHE_BYTES == 256
#            define __xu_cacheline_aligned__ __declspec(align(256))
#        else
#            error unknown cacheline bytes
#        endif
#    else
#        define __xu_cacheline_aligned__
#    endif
#endif
