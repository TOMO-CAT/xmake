#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 */
// abs
#define xu_abs(x) ((x) > 0 ? (x) : -(x))

// max
#define xu_max(x, y) (((x) > (y)) ? (x) : (y))

// min
#define xu_min(x, y) (((x) < (y)) ? (x) : (y))

// max3
#define xu_max3(x, y, z) (((x) > (y)) ? (((x) > (z)) ? (x) : (z)) : (((y) > (z)) ? (y) : (z)))

// min3
#define xu_min3(x, y, z) (((x) < (y)) ? (((x) < (z)) ? (x) : (z)) : (((y) < (z)) ? (y) : (z)))

// the number of entries in the array
#define xu_arrayn(x) ((sizeof((x)) / sizeof((x)[0])))

// ispow2: 1, 2, 4, 8, 16, 32, ...
#define xu_ispow2(x) (!((x) & ((x)-1)) && (x))

// align2
#define xu_align2(x) (((x) + 1) >> 1 << 1)

// align4
#define xu_align4(x) (((x) + 3) >> 2 << 2)

// align8
#define xu_align8(x) (((x) + 7) >> 3 << 3)

// align
#define xu_align(x, b) (((xu_size_t)(x) + ((xu_size_t)(b)-1)) & ~((xu_size_t)(b)-1))

// align u32
#define xu_align_u32(x, b) (((xu_uint32_t)(x) + ((xu_uint32_t)(b)-1)) & ~((xu_uint32_t)(b)-1))

// align u64
#define xu_align_u64(x, b) (((xu_uint64_t)(x) + ((xu_uint64_t)(b)-1)) & ~((xu_uint64_t)(b)-1))

// align by pow2
#define xu_align_pow2(x)                                                                                               \
    (((x) > 1) ? (xu_ispow2(x) ? (x) : ((xu_size_t)1 << (32 - xu_bits_cl0_u32_be((xu_uint32_t)(x))))) : 1)

/*! @def xu_align_cpu
 *
 * align by cpu bytes
 */
#if XU_CPU_BIT64
#    define xu_align_cpu(x) xu_align8(x)
#else
#    define xu_align_cpu(x) xu_align4(x)
#endif

// offsetof
#if defined(XU_COMPILER_IS_GCC) && XU_COMPILER_VERSION_BE(4, 1)
#    define xu_offsetof(s, m) (xu_size_t) __builtin_offsetof(s, m)
#else
#    define xu_offsetof(s, m) (xu_size_t) & (((s const*)0)->m)
#endif

// container of
#define xu_container_of(s, m, p) ((s*)(((xu_byte_t*)(p)) - xu_offsetof(s, m)))

// memsizeof
#define xu_memsizeof(s, m) sizeof(((s const*)0)->m)

// memtailof
#define xu_memtailof(s, m) (xu_offsetof(s, m) + xu_memsizeof(s, m))

// memdiffof: lm - rm
#define xu_memdiffof(s, lm, rm) (xu_memtailof(s, lm) - xu_memtailof(s, rm))

// check the offset and size of member for struct or union
#define xu_memberof_eq(ls, lm, rs, rm)                                                                                 \
    ((xu_offsetof(ls, lm) == xu_offsetof(rs, rm)) && (xu_memsizeof(ls, lm) == xu_memsizeof(rs, rm)))

// pointer to bool
#define xu_p2b(x) ((xu_bool_t)(xu_size_t)(x))

// pointer to u8
#define xu_p2u8(x) ((xu_uint8_t)(xu_size_t)(x))

// pointer to u16
#define xu_p2u16(x) ((xu_uint16_t)(xu_size_t)(x))

// pointer to u32
#define xu_p2u32(x) ((xu_uint32_t)(xu_size_t)(x))

// pointer to u64
#define xu_p2u64(x) ((xu_uint64_t)(xu_size_t)(x))

// pointer to s8
#define xu_p2s8(x) ((xu_sint8_t)(xu_long_t)(x))

// pointer to s16
#define xu_p2s16(x) ((xu_sint16_t)(xu_long_t)(x))

// pointer to s32
#define xu_p2s32(x) ((xu_sint32_t)(xu_long_t)(x))

// pointer to s64
#define xu_p2s64(x) ((xu_sint64_t)(xu_long_t)(x))

// bool to pointer
#define xu_b2p(x) ((xu_pointer_t)(xu_size_t)(x))

// unsigned integer to pointer
#define xu_u2p(x) ((xu_pointer_t)(xu_size_t)(x))

// integer to pointer
#define xu_i2p(x) ((xu_pointer_t)(xu_long_t)(x))

// swap
#define xu_swap(t, l, r)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        t __p = (r);                                                                                                   \
        (r)   = (l);                                                                                                   \
        (l)   = __p;                                                                                                   \
    } while (0)
