#pragma once

#include "xutil/prefix.h"

#if !defined(xu_memory_barrier) && defined(XU_ASSEMBLER_IS_GAS) && (defined(XU_ARCH_x86) || defined(XU_ARCH_x64))
#    define xu_memory_barrier() __xu_asm__ __xu_volatile__("" ::: "memory")
#endif
