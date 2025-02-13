#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * includes
 */
#ifdef XU_CONFIG_LIBC_HAVE_SETLOCALE
#    include <locale.h>
#endif

/* *******************************************************
 * macros
 */
#ifdef XU_CONFIG_LIBC_HAVE_SETLOCALE
#    ifdef XU_CONFIG_FORCE_UTF8
#        if defined(XU_CONFIG_OS_WINDOWS)
#            define xu_setlocale()                                                                                     \
                do                                                                                                     \
                {                                                                                                      \
                    if (!setlocale(LC_ALL, ".65001"))                                                                  \
                    {                                                                                                  \
                        xu_trace_w("failed to setlocale to utf-8");                                                    \
                        setlocale(LC_ALL, "");                                                                         \
                    }                                                                                                  \
                } while (0)
#        else
#            define xu_setlocale()                                                                                     \
                do                                                                                                     \
                {                                                                                                      \
                    if (!(setlocale(LC_ALL, "C.UTF-8") || setlocale(LC_ALL, "en_US.UTF-8") ||                          \
                          setlocale(LC_ALL, "zh_CN.UTF-8")))                                                           \
                    {                                                                                                  \
                        xu_trace_w("failed to setlocale to utf-8");                                                    \
                        setlocale(LC_ALL, "");                                                                         \
                    }                                                                                                  \
                } while (0)
#        endif
#    else
#        define xu_setlocale() setlocale(LC_ALL, "")
#    endif
#    define xu_resetlocale() setlocale(LC_ALL, "C")
#else
#    define xu_setlocale() ((void)0)
#    define xu_resetlocale() ((void)0)
#endif
