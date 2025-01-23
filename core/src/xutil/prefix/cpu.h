#pragma once

#include "xutil/config.h"

/* *******************************************************
 *  macros
 */
// check 64-bits
#if defined(__LP64__) || defined(__64BIT__) || defined(_LP64) || defined(__x86_64) || defined(__x86_64__) ||           \
    defined(__amd64) || defined(__amd64__) || defined(__arm64) || defined(__arm64__) || defined(__sparc64__) ||        \
    defined(__PPC64__) || defined(__ppc64__) || defined(__powerpc64__) || defined(__sw_64) ||                          \
    defined(__loongarch64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_ARM64) || defined(_M_IA64) ||         \
    defined(__ia64__) || defined(__ia64) || (defined(__WORDSIZE) && (__WORDSIZE == 64)) ||                             \
    defined(TCC_TARGET_X86_64) || defined(__wasm64) || defined(__wasm64__)
#    define XU_CPU_BITSIZE (64)
#    define XU_CPU_BITBYTE (8)
#    define XU_CPU_BITALIGN (7)
#    define XU_CPU_BIT32 (0)
#    define XU_CPU_BIT64 (1)
#    define XU_CPU_SHIFT (6)
#else
#    define XU_CPU_BITSIZE (32)
#    define XU_CPU_BITBYTE (4)
#    define XU_CPU_BITALIGN (3)
#    define XU_CPU_BIT32 (1)
#    define XU_CPU_BIT64 (0)
#    define XU_CPU_SHIFT (5)
#endif
