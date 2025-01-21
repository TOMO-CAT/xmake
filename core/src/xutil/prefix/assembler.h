#pragma once

#include "xutil/config.h"
#include "xutil/prefix/compiler.h"

#if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_CLANG) || defined(XU_COMPILER_IS_INTEL)
#    define XU_ASSEMBLER_IS_GAS
#endif
