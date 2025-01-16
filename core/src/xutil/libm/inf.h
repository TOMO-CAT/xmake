#pragma once

#include "xutil/prefix.h"

#if defined(XU_COMPILER_IS_GCC) && XU_COMPILER_VERSION_BE(3, 3)
#    define XU_INF (__builtin_inff())
#else
#    define XU_INF XU_MAF
#endif
