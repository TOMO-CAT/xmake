#pragma once

#include "xutil/prefix.h"

/*! notifies the CPU that this is a spinlock wait loop so memory and cache accesses may be optimized.
 * pause may actually stop CPU for some time to save power. Older CPUs decode it as REP NOP, so you don't have to check
 * if its supported. Older CPUs will simply do nothing (NOP) as fast as possible.
 */
#if defined(XU_ASSEMBLER_IS_GAS)
#    if defined(XU_COMPILER_IS_GCC) && defined(XU_ARCH_x86)
// old "as" does not support "pause" opcode
#        define xu_cpu_pause()                                                                                         \
            do                                                                                                         \
            {                                                                                                          \
                __xu_asm__ __xu_volatile__(".byte 0xf3, 0x90");                                                        \
            } while (0)
#    elif (defined(XU_ARCH_x86) || defined(XU_ARCH_x64))
#        if defined(XU_COMPILER_IS_SUNC)
// Sun Studio 12 exits with segmentation fault on '__asm ("pause")'
#            define xu_cpu_pause()                                                                                     \
                do                                                                                                     \
                {                                                                                                      \
                    __xu_asm__ __xu_volatile__("rep;nop" ::: "memory");                                                \
                } while (0)
#        else
#            define xu_cpu_pause()                                                                                     \
                do                                                                                                     \
                {                                                                                                      \
                    __xu_asm__ __xu_volatile__("pause");                                                               \
                } while (0)
#        endif
#    elif 0 // defined(XU_ARCH_ARM), TODO some cross-toolchains may be not supported
#        ifdef __CC_ARM
#            define xu_cpu_pause() __yield()
#        else
#            define xu_cpu_pause()                                                                                     \
                do                                                                                                     \
                {                                                                                                      \
                    __xu_asm__ __volatile__("yield");                                                                  \
                } while (0)
#        endif
#    endif
#endif

/*! the cpu count
 *
 * @return              the cpu count
 */
xu_size_t xu_cpu_count(xu_noarg_t);
