#pragma once

#include "xutil/prefix.h"

#ifndef XU_COMPILER_IS_GCC
#    include <stdarg.h>
#endif

/* *******************************************************
 *  macros
 */
#ifdef XU_COMPILER_IS_GCC
#    define xu_va_start(v, l) __builtin_va_start(v, l)
#    define xu_va_end(v) __builtin_va_end(v)
#    define xu_va_arg(v, l) __builtin_va_arg(v, l)
#    define xu_va_copy(v, c) __builtin_va_copy(v, c)
#else
#    define xu_va_start(v, l) va_start(v, l)
#    define xu_va_end(v) va_end(v)
#    define xu_va_arg(v, l) va_arg(v, l)
#    ifndef va_copy
#        define xu_va_copy(v, c) ((v) = (c))
#    else
#        define xu_va_copy(v, c) va_copy(v, c)
#    endif
#endif

/* *******************************************************
 *  types
 */
#ifdef XU_COMPILER_IS_GCC
typedef __builtin_va_list xu_va_list_t;
#else
typedef va_list xu_va_list_t;
#endif
