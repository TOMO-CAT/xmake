#pragma once

#include "xutil/config.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */

/* abort it, @note ud2 cannot be aborted immediately for multi-thread
 * and it will be not caught for linux exception (ignore int3 signal)
 */
#if (defined(XU_ARCH_x86) || defined(XU_ARCH_x64)) && (!defined(XU_CONFIG_EXCEPTION_ENABLE))
#    if defined(XU_ASSEMBLER_IS_GAS)
//#       define xu_abort_done()                          do { __xu_asm__ __xu_volatile__ ("ud2"); } while (0)
#        define xu_abort_done()                                                                                        \
            do                                                                                                         \
            {                                                                                                          \
                __xu_asm__ __xu_volatile__("int3");                                                                    \
            } while (0)
#    endif
#endif

#ifndef xu_abort_done
#    define xu_abort_done()                                                                                            \
        do                                                                                                             \
        {                                                                                                              \
            *((__xu_volatile__ xu_int_t*)0) = 0;                                                                       \
        } while (0)
#endif
