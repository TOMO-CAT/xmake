#pragma once

#include "xutil/config.h"

/* *******************************************************
 *  macros
 */
/* arch
 *
 * gcc builtin macros for gcc -dM -E - < /dev/null
 *
 * .e.g gcc -m64 -dM -E - < /dev/null | grep 64
 * .e.g gcc -m32 -dM -E - < /dev/null | grep 86
 * .e.g gcc -march=armv6 -dM -E - < /dev/null | grep ARM
 */
#if defined(__i386) || defined(__i686) || defined(__i386__) || defined(__i686__) || defined(_M_IX86)
#    define XU_ARCH_x86
#    if defined(__i386) || defined(__i386__)
#        define XU_ARCH_STRING "i386"
#    elif defined(__i686) || defined(__i686__)
#        define XU_ARCH_STRING "i686"
#    elif defined(_M_IX86)
#        if (_M_IX86 == 300)
#            define XU_ARCH_STRING "i386"
#        elif (_M_IX86 == 400)
#            define XU_ARCH_STRING "i486"
#        elif (_M_IX86 == 500 || _M_IX86 == 600)
#            define XU_ARCH_STRING "Pentium"
#        endif
#    else
#        define XU_ARCH_STRING "x86"
#    endif
#elif defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(__ia64__) || defined(__IA64__) ||         \
    defined(__ia64) || defined(_M_IA64) || defined(_M_X64)
#    define XU_ARCH_x64
#    if defined(__x86_64)
#        define XU_ARCH_STRING "x86_64"
#    elif defined(__amd64__) || defined(__amd64)
#        define XU_ARCH_STRING "amd64"
#    elif defined(__ia64__) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64)
#        define XU_ARCH_STRING "ia64"
#    else
#        define XU_ARCH_STRING "x64"
#    endif
#elif defined(__arm__) || defined(__arm64) || defined(__arm64__) || (defined(__aarch64__) && __aarch64__) ||           \
    defined(_M_ARM64) || defined(_M_ARM)
#    define XU_ARCH_ARM
#    if defined(__ARM64_ARCH_8__) || defined(_M_ARM64)
#        define XU_ARCH_ARM64
#        define XU_ARCH_ARM_VERSION (8)
#        define XU_ARCH_ARM_v8
#        define XU_ARCH_STRING "arm64"
#    elif defined(__ARM_ARCH_7A__)
#        define XU_ARCH_ARM_VERSION (7)
#        define XU_ARCH_ARM_v7A
#        define XU_ARCH_STRING "armv7a"
#    elif defined(__ARM_ARCH_7__)
#        define XU_ARCH_ARM_VERSION (7)
#        define XU_ARCH_ARM_v7
#        define XU_ARCH_STRING "armv7"
#    elif defined(__ARM_ARCH_6__)
#        define XU_ARCH_ARM_VERSION (6)
#        define XU_ARCH_ARM_v6
#        define XU_ARCH_STRING "armv6"
#    elif defined(__ARM_ARCH_5TE__)
#        define XU_ARCH_ARM_VERSION (5)
#        define XU_ARCH_ARM_v5te
#        define XU_ARCH_STRING "armv5te"
#    elif defined(__ARM_ARCH_5__)
#        define XU_ARCH_ARM_VERSION (5)
#        define XU_ARCH_ARM_v5
#        define XU_ARCH_STRING "armv5"
#    elif defined(__ARM_ARCH_4T__)
#        define XU_ARCH_ARM_VERSION (4)
#        define XU_ARCH_ARM_v4t
#        define XU_ARCH_STRING "armv4t"
#    elif defined(__ARM_ARCH_3__)
#        define XU_ARCH_ARM_VERSION (3)
#        define XU_ARCH_ARM_v3
#        define XU_ARCH_STRING "armv3"
#    elif defined(__ARM_ARCH)
#        define XU_ARCH_ARM_VERSION __ARM_ARCH
#        if __ARM_ARCH >= 8
#            define XU_ARCH_ARM_v8
#            if defined(__arm64) || defined(__arm64__)
#                define XU_ARCH_ARM64
#                define XU_ARCH_STRING "arm64"
#            elif (defined(__aarch64__) && __aarch64__)
#                define XU_ARCH_ARM64
#                define XU_ARCH_STRING "arm64-v8a"
#            else
#                define XU_ARCH_STRING "armv7s"
#            endif
#        elif __ARM_ARCH >= 7
#            define XU_ARCH_ARM_v7
#            define XU_ARCH_STRING "armv7"
#        elif __ARM_ARCH >= 6
#            define XU_ARCH_ARM_v6
#            define XU_ARCH_STRING "armv6"
#        else
#            define XU_ARCH_ARM_v5
#            define XU_ARCH_STRING "armv5"
#        endif
#    elif defined(__aarch64__) && __aarch64__
#        define XU_ARCH_ARM_v8
#        define XU_ARCH_ARM64
#        define XU_ARCH_STRING "arm64-v8a"
#    else
#        error unknown arm arch version
#    endif
#    if !defined(XU_ARCH_ARM64) && (defined(__arm64) || defined(__arm64__) || (defined(__aarch64__) && __aarch64__))
#        define XU_ARCH_ARM64
#        ifndef XU_ARCH_STRING
#            define XU_ARCH_STRING "arm64"
#        endif
#    endif
#    ifndef XU_ARCH_STRING
#        define XU_ARCH_STRING "arm"
#    endif
#    if defined(__thumb__)
#        define XU_ARCH_ARM_THUMB
#        define XU_ARCH_STRING_2 "_thumb"
#    endif
#    if defined(__ARM_NEON__)
#        define XU_ARCH_ARM_NEON
#        define XU_ARCH_STRING_3 "_neon"
#    endif
#elif defined(mips) || defined(_mips) || defined(__mips__)
#    define XU_ARCH_MIPS
#    if defined(_MIPSEB)
#        if (_MIPS_SIM == _ABIO32)
#            define XU_ARCH_STRING "mips"
#        elif (_MIPS_SIM == _ABIN32)
#            define XU_ARCH_STRING "mipsn32"
#        elif (_MIPS_SIM == _ABI64)
#            define XU_ARCH_STRING "mips64"
#        endif
#    elif defined(_MIPSEL)
#        if (_MIPS_SIM == _ABIO32)
#            define XU_ARCH_STRING "mipsel"
#        elif (_MIPS_SIM == _ABIN32)
#            define XU_ARCH_STRING "mipsn32el"
#        elif (_MIPS_SIM == _ABI64)
#            define XU_ARCH_STRING "mips64el"
#        endif
#    endif
#elif defined(__loongarch__)
#    define XU_ARCH_LOONGARCH
#    if defined(__loongarch64)
#        define XU_ARCH_STRING "loong64"
#    elif defined(__loongarch32)
#        define XU_ARCH_STRING "loong32"
#    else
#        error unknown version of LoongArch, please feedback to us.
#    endif
#elif defined(__sw_64)
#    define XU_ARCH_SW_64
#    define XU_ARCH_STRING "sw_64"
#elif defined(__riscv)
#    define XU_ARCH_RISCV
#    if defined(__riscv_xlen) && __riscv_xlen == 64
#        define XU_ARCH_STRING "riscv64"
#    else
#        define XU_ARCH_STRING "riscv32"
#    endif
#elif defined(__hppa__) || defined(__HPPA__) || defined(__hppa)
#    define XU_ARCH_HPPA
#    define XU_ARCH_STRING "hppa"
#elif defined(__m68k__) || defined(__MC68K__) || defined(M68000)
#    define XU_ARCH_M68K
#    define XU_ARCH_STRING "m68k"
#elif defined(__PPC__) || defined(_ARCH_PPC)
#    define XU_ARCH_PPC
#    if (defined(__PPC64__) && __PPC64__ == 1) || defined(_ARCH_PPC64)
#        define XU_ARCH_STRING "ppc64"
#    else
#        define XU_ARCH_STRING "ppc"
#    endif
#elif defined(__s390__)
#    define XU_ARCH_s390
#    define XU_ARCH_STRING "s390"
#elif defined(__alpha__)
#    define XU_ARCH_ALPHA
#    define XU_ARCH_STRING "alpha"
#elif defined(__sparc__) || defined(__sparc)
#    define XU_ARCH_SPARC
#    define XU_ARCH_STRING "sparc"
#elif defined(__sh__)
#    define XU_ARCH_SH
#    if defined(__SH4__)
#        define XU_ARCH_SH4
#        define XU_ARCH_STRING "SH4"
#    elif defined(__SH3__)
#        define XU_ARCH_SH4
#        define XU_ARCH_STRING "SH3"
#    elif defined(__SH2__)
#        define XU_ARCH_SH4
#        define XU_ARCH_STRING "SH2"
#    elif defined(__SH1__)
#        define XU_ARCH_SH1
#        define XU_ARCH_STRING "SH1"
#    else
#        define XU_ARCH_STRING "SH"
#    endif
#elif defined(XU_COMPILER_IS_TINYC)
#    if defined(TCC_TARGET_I386)
#        define XU_ARCH_x86
#        define XU_ARCH_STRING "i386"
#    elif defined(__x86_64__) || defined(TCC_TARGET_X86_64)
#        define XU_ARCH_x64
#        define XU_ARCH_STRING "x86_64"
#    elif defined(TCC_TARGET_ARM)
#        define XU_ARCH_ARM
#        define XU_ARCH_STRING "arm"
#    else
#        error unknown arch for tiny c, please define target like -DTCC_TARGET_I386
#    endif
#elif defined(__wasm) || defined(__wasm__) || defined(__wasm64) || defined(__wasm64__)
#    define XU_ARCH_WASM
#    if defined(__wasm64__) || defined(__wasm64) || defined(_LP64) || defined(__LP64__)
#        define XU_ARCH_WASM64
#        define XU_ARCH_STRING "wasm64"
#    else
#        define XU_ARCH_WASM32
#        define XU_ARCH_STRING "wasm32"
#    endif
#else
#    error unknown arch
#    define XU_ARCH_STRING "unknown_arch"
#endif

// sse
#if defined(XU_ARCH_x86) || defined(XU_ARCH_x64)
#    if defined(__SSE__)
#        define XU_ARCH_SSE
#        define XU_ARCH_STRING_2 "_sse"
#    endif
#    if defined(__SSE2__)
#        define XU_ARCH_SSE2
#        undef XU_ARCH_STRING_2
#        define XU_ARCH_STRING_2 "_sse2"
#    endif
#    if defined(__SSE3__)
#        define XU_ARCH_SSE3
#        undef XU_ARCH_STRING_2
#        define XU_ARCH_STRING_2 "_sse3"
#    endif
#endif

// vfp
#if defined(__VFP_FP__) || (defined(XU_COMPILER_IS_TINYC) && defined(TCC_ARM_VFP))
#    define XU_ARCH_VFP
#    define XU_ARCH_STRING_4 "_vfp"
#endif

// elf
#if defined(__ELF__) || (defined(XU_COMPILER_IS_TINYC) && !defined(TCC_ARM_PE))
#    define XU_ARCH_ELF
#    define XU_ARCH_STRING_5 "_elf"
#endif

// mach
#if defined(__MACH__)
#    define XU_ARCH_MACH
#    define XU_ARCH_STRING_5 "_mach"
#endif

#ifndef XU_ARCH_STRING_1
#    define XU_ARCH_STRING_1 ""
#endif

#ifndef XU_ARCH_STRING_2
#    define XU_ARCH_STRING_2 ""
#endif

#ifndef XU_ARCH_STRING_3
#    define XU_ARCH_STRING_3 ""
#endif

#ifndef XU_ARCH_STRING_4
#    define XU_ARCH_STRING_4 ""
#endif

#ifndef XU_ARCH_STRING_5
#    define XU_ARCH_STRING_5 ""
#endif

// version string
#ifndef XU_ARCH_VERSION_STRING
#    define XU_ARCH_VERSION_STRING                                                                                     \
        __xu_mstrcat6__(XU_ARCH_STRING, XU_ARCH_STRING_1, XU_ARCH_STRING_2, XU_ARCH_STRING_3, XU_ARCH_STRING_4,        \
                        XU_ARCH_STRING_5)
#endif
