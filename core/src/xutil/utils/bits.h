#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

#if defined(XU_COMPILER_IS_GCC)
#    include "xutil/utils/impl/bits_gcc.h"
#endif
#if defined(XU_ARCH_x86) || defined(XU_ARCH_x64)
#    include "xutil/utils/impl/bits_x86.h"
#elif defined(XU_ARCH_ARM)
#    include "xutil/utils/impl/bits_arm.h"
#elif defined(XU_ARCH_SH4)
#    include "xutil/utils/impl/bits_sh4.h"
#endif

/* *******************************************************
 *  macros
 */
// 1-bits
#define xu_bits_get_u1(p) (((*(p)) >> 7) & 1)
#define xu_bits_set_u1(p, x)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        *(p) &= 0x7f;                                                                                                  \
        *(p) |= (((x)&0x1) << 7);                                                                                      \
    } while (0)

// 8-bits
#define xu_bits_get_u8(p) (*(p))
#define xu_bits_get_s8(p) (*(p))

#define xu_bits_set_u8(p, x)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        *(p) = (xu_uint8_t)(x);                                                                                        \
    } while (0)
#define xu_bits_set_s8(p, x)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        *(p) = (xu_sint8_t)(x);                                                                                        \
    } while (0)

// 16-bits
#define xu_bits_get_u16_le_impl(p) ((xu_uint16_t)((xu_uint16_t) * ((p) + 1) << 8 | (xu_uint16_t) * (p)))
#define xu_bits_get_s16_le_impl(p) xu_bits_get_u16_le_impl(p)
#define xu_bits_get_u16_be_impl(p) ((xu_uint16_t)(*((p)) << 8 | (xu_uint16_t) * ((p) + 1)))
#define xu_bits_get_s16_be_impl(p) xu_bits_get_u16_be_impl(p)
#define xu_bits_get_u16_ne_impl(p) (*((xu_uint16_t*)(p)))
#define xu_bits_get_s16_ne_impl(p) xu_bits_get_u16_ne_impl(p)

#define xu_bits_set_u16_le_impl(p, x) xu_bits_set_u16_le_inline(p, x)
#define xu_bits_set_s16_le_impl(p, x) xu_bits_set_u16_le_inline(p, x)
#define xu_bits_set_u16_be_impl(p, x) xu_bits_set_u16_be_inline(p, x)
#define xu_bits_set_s16_be_impl(p, x) xu_bits_set_u16_be_inline(p, x)
#define xu_bits_set_u16_ne_impl(p, x)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        *((xu_uint16_t*)(p)) = (xu_uint16_t)(x);                                                                       \
    } while (0)
#define xu_bits_set_s16_ne_impl(p, x) xu_bits_set_u16_ne_impl(p, x)

// 24-bits
#define xu_bits_get_u24_le_impl(p) ((xu_uint32_t)(*((p) + 2) << 16 | *((p) + 1) << 8 | *(p)))
#define xu_bits_get_s24_le_impl(p) ((xu_bits_get_u24_le_impl(p) + 0xff800000) ^ 0xff800000)
#define xu_bits_get_u24_be_impl(p) ((xu_uint32_t)(*(p) << 16 | *((p) + 1) << 8 | *((p) + 2)))
#define xu_bits_get_s24_be_impl(p) ((xu_bits_get_u24_be_impl(p) + 0xff800000) ^ 0xff800000)
#define xu_bits_get_u24_ne_impl(p) ((xu_uint32_t)(*((xu_uint32_t*)(p)) & 0x00ffffff))
#define xu_bits_get_s24_ne_impl(p) ((xu_bits_get_u24_ne_impl(p) + 0xff800000) ^ 0xff800000)

#define xu_bits_set_u24_le_impl(p, x) xu_bits_set_u24_le_inline(p, x)
#define xu_bits_set_s24_le_impl(p, x) xu_bits_set_u24_le_inline(p, x)
#define xu_bits_set_u24_be_impl(p, x) xu_bits_set_u24_be_inline(p, x)
#define xu_bits_set_s24_be_impl(p, x) xu_bits_set_u24_be_inline(p, x)
#define xu_bits_set_u24_ne_impl(p, x)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        *((xu_uint32_t*)(p)) = (xu_uint32_t)(x)&0x00ffffff;                                                            \
    } while (0)
#define xu_bits_set_s24_ne_impl(p, x) xu_bits_set_u24_ne_impl(p, x)

// 32-bits
#define xu_bits_get_u32_le_impl(p) ((xu_uint32_t)(*((p) + 3) << 24 | *((p) + 2) << 16 | *((p) + 1) << 8 | *(p)))
#define xu_bits_get_s32_le_impl(p) xu_bits_get_u32_le_impl(p)
#define xu_bits_get_u32_be_impl(p) ((xu_uint32_t)(*(p) << 24 | *((p) + 1) << 16 | *((p) + 2) << 8 | *((p) + 3)))
#define xu_bits_get_s32_be_impl(p) xu_bits_get_u32_be_impl(p)
#define xu_bits_get_u32_ne_impl(p) (*((xu_uint32_t*)(p)))
#define xu_bits_get_s32_ne_impl(p) xu_bits_get_u32_ne_impl(p)

#define xu_bits_set_u32_le_impl(p, x) xu_bits_set_u32_le_inline(p, x)
#define xu_bits_set_s32_le_impl(p, x) xu_bits_set_u32_le_inline(p, x)
#define xu_bits_set_u32_be_impl(p, x) xu_bits_set_u32_be_inline(p, x)
#define xu_bits_set_s32_be_impl(p, x) xu_bits_set_u32_be_inline(p, x)
#define xu_bits_set_u32_ne_impl(p, x)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        *((xu_uint32_t*)(p)) = (xu_uint32_t)(x);                                                                       \
    } while (0)
#define xu_bits_set_s32_ne_impl(p, x) xu_bits_set_u32_ne_impl(p, x)

// 64-bits
#define xu_bits_get_u64_le_impl(p)                                                                                     \
    ((xu_uint64_t) * ((p) + 7) << 56 | (xu_uint64_t) * ((p) + 6) << 48 | (xu_uint64_t) * ((p) + 5) << 40 |             \
     (xu_uint64_t) * ((p) + 4) << 32 | (xu_uint64_t) * ((p) + 3) << 24 | (xu_uint64_t) * ((p) + 2) << 16 |             \
     (xu_uint64_t) * ((p) + 1) << 8 | (xu_uint64_t) * (p))
#define xu_bits_get_s64_le_impl(p) xu_bits_get_u64_le_impl(p)
#define xu_bits_get_u64_be_impl(p)                                                                                     \
    ((xu_uint64_t) * (p) << 56 | (xu_uint64_t) * ((p) + 1) << 48 | (xu_uint64_t) * ((p) + 2) << 40 |                   \
     (xu_uint64_t) * ((p) + 3) << 32 | (xu_uint64_t) * ((p) + 4) << 24 | (xu_uint64_t) * ((p) + 5) << 16 |             \
     (xu_uint64_t) * ((p) + 6) << 8 | (xu_uint64_t) * ((p) + 7))
#define xu_bits_get_s64_be_impl(p) xu_bits_get_u64_be_impl(p)
#define xu_bits_get_u64_ne_impl(p) (*((xu_uint64_t*)(p)))
#define xu_bits_get_s64_ne_impl(p) xu_bits_get_u64_ne_impl(p)

#define xu_bits_set_u64_le_impl(p, x) xu_bits_set_u64_le_inline(p, x)
#define xu_bits_set_s64_le_impl(p, x) xu_bits_set_u64_le_inline(p, x)
#define xu_bits_set_u64_be_impl(p, x) xu_bits_set_u64_be_inline(p, x)
#define xu_bits_set_s64_be_impl(p, x) xu_bits_set_u64_be_inline(p, x)
#define xu_bits_set_u64_ne_impl(p, x)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        *((xu_uint64_t*)(p)) = (xu_uint64_t)(x);                                                                       \
    } while (0)
#define xu_bits_set_s64_ne_impl(p, x) xu_bits_set_u64_ne_impl(p, x)

// float
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT

#    define xu_bits_get_float_le(p) xu_bits_get_float_le_inline(p)
#    define xu_bits_get_float_be(p) xu_bits_get_float_be_inline(p)

#    define xu_bits_set_float_le(p, x) xu_bits_set_float_le_inline(p, x)
#    define xu_bits_set_float_be(p, x) xu_bits_set_float_be_inline(p, x)

#    define xu_bits_get_double_ble(p) xu_bits_get_double_ble_inline(p)
#    define xu_bits_get_double_bbe(p) xu_bits_get_double_bbe_inline(p)

#    define xu_bits_get_double_lle(p) xu_bits_get_double_lle_inline(p)
#    define xu_bits_get_double_lbe(p) xu_bits_get_double_lbe_inline(p)

#    define xu_bits_set_double_ble(p, x) xu_bits_set_double_ble_inline(p, x)
#    define xu_bits_set_double_bbe(p, x) xu_bits_set_double_bbe_inline(p, x)

#    define xu_bits_set_double_lle(p, x) xu_bits_set_double_lle_inline(p, x)
#    define xu_bits_set_double_lbe(p, x) xu_bits_set_double_lbe_inline(p, x)

#    ifdef XU_FLOAT_BIGENDIAN
#        define xu_bits_get_double_nbe(p) xu_bits_get_double_bbe(p)
#        define xu_bits_get_double_nle(p) xu_bits_get_double_ble(p)

#        define xu_bits_set_double_nbe(p, x) xu_bits_set_double_bbe(p, x)
#        define xu_bits_set_double_nle(p, x) xu_bits_set_double_ble(p, x)
#    else
#        define xu_bits_get_double_nbe(p) xu_bits_get_double_lbe(p)
#        define xu_bits_get_double_nle(p) xu_bits_get_double_lle(p)

#        define xu_bits_set_double_nbe(p, x) xu_bits_set_double_lbe(p, x)
#        define xu_bits_set_double_nle(p, x) xu_bits_set_double_lle(p, x)
#    endif
#    ifdef XU_WORDS_BIGENDIAN
#        define xu_bits_get_float_ne(p) xu_bits_get_float_be(p)
#        define xu_bits_set_float_ne(p, x) xu_bits_set_float_be(p, x)

#        define xu_bits_get_double_nne(p) xu_bits_get_double_nbe(p)
#        define xu_bits_get_double_bne(p) xu_bits_get_double_bbe(p)
#        define xu_bits_get_double_lne(p) xu_bits_get_double_lbe(p)

#        define xu_bits_set_double_nne(p, x) xu_bits_set_double_nbe(p, x)
#        define xu_bits_set_double_bne(p, x) xu_bits_set_double_bbe(p, x)
#        define xu_bits_set_double_lne(p, x) xu_bits_set_double_lbe(p, x)
#    else
#        define xu_bits_get_float_ne(p) xu_bits_get_float_le(p)
#        define xu_bits_set_float_ne(p, x) xu_bits_set_float_le(p, x)

#        define xu_bits_get_double_nne(p) xu_bits_get_double_nle(p)
#        define xu_bits_get_double_bne(p) xu_bits_get_double_ble(p)
#        define xu_bits_get_double_lne(p) xu_bits_get_double_lle(p)

#        define xu_bits_set_double_nne(p, x) xu_bits_set_double_nle(p, x)
#        define xu_bits_set_double_bne(p, x) xu_bits_set_double_ble(p, x)
#        define xu_bits_set_double_lne(p, x) xu_bits_set_double_lle(p, x)
#    endif
#endif

#ifdef XU_UNALIGNED_ACCESS_ENABLE

#    ifdef XU_WORDS_BIGENDIAN
// 16-bits
#        define xu_bits_get_u16_le(p) xu_bits_get_u16_le_impl((xu_byte_t*)(p))
#        define xu_bits_get_s16_le(p) xu_bits_get_s16_le_impl((xu_byte_t*)(p))
#        define xu_bits_get_u16_be(p) xu_bits_get_u16_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_s16_be(p) xu_bits_get_s16_ne_impl((xu_byte_t*)(p))

#        define xu_bits_set_u16_le(p, x) xu_bits_set_u16_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s16_le(p, x) xu_bits_set_s16_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_u16_be(p, x) xu_bits_set_u16_ne_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s16_be(p, x) xu_bits_set_s16_ne_impl((xu_byte_t*)(p), x)

// 24-bits
#        define xu_bits_get_u24_le(p) xu_bits_get_u24_le_impl((xu_byte_t*)(p))
#        define xu_bits_get_s24_le(p) xu_bits_get_s24_le_impl((xu_byte_t*)(p))
#        define xu_bits_get_u24_be(p) xu_bits_get_u24_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_s24_be(p) xu_bits_get_s24_ne_impl((xu_byte_t*)(p))

#        define xu_bits_set_u24_le(p, x) xu_bits_set_u24_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s24_le(p, x) xu_bits_set_s24_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_u24_be(p, x) xu_bits_set_u24_ne_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s24_be(p, x) xu_bits_set_s24_ne_impl((xu_byte_t*)(p), x)

// 32-bits
#        define xu_bits_get_u32_le(p) xu_bits_get_u32_le_impl((xu_byte_t*)(p))
#        define xu_bits_get_s32_le(p) xu_bits_get_s32_le_impl((xu_byte_t*)(p))
#        define xu_bits_get_u32_be(p) xu_bits_get_u32_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_s32_be(p) xu_bits_get_s32_ne_impl((xu_byte_t*)(p))

#        define xu_bits_set_u32_le(p, x) xu_bits_set_u32_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s32_le(p, x) xu_bits_set_u32_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_u32_be(p, x) xu_bits_set_u32_ne_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s32_be(p, x) xu_bits_set_s32_ne_impl((xu_byte_t*)(p), x)

// 64-bits
#        define xu_bits_get_u64_le(p) xu_bits_get_u64_le_impl((xu_byte_t*)(p))
#        define xu_bits_get_s64_le(p) xu_bits_get_s64_le_impl((xu_byte_t*)(p))
#        define xu_bits_get_u64_be(p) xu_bits_get_u64_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_s64_be(p) xu_bits_get_s64_ne_impl((xu_byte_t*)(p))

#        define xu_bits_set_u64_le(p, x) xu_bits_set_u64_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s64_le(p, x) xu_bits_set_u64_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_u64_be(p, x) xu_bits_set_u64_ne_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s64_be(p, x) xu_bits_set_s64_ne_impl((xu_byte_t*)(p), x)

#    else

// 16-bits
#        define xu_bits_get_u16_le(p) xu_bits_get_u16_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_s16_le(p) xu_bits_get_s16_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_u16_be(p) xu_bits_get_u16_be_impl((xu_byte_t*)(p))
#        define xu_bits_get_s16_be(p) xu_bits_get_s16_be_impl((xu_byte_t*)(p))

#        define xu_bits_set_u16_le(p, x) xu_bits_set_u16_ne_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s16_le(p, x) xu_bits_set_s16_ne_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_u16_be(p, x) xu_bits_set_u16_be_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s16_be(p, x) xu_bits_set_s16_be_impl((xu_byte_t*)(p), x)

// 24-bits
#        define xu_bits_get_u24_le(p) xu_bits_get_u24_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_s24_le(p) xu_bits_get_s24_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_u24_be(p) xu_bits_get_u24_be_impl((xu_byte_t*)(p))
#        define xu_bits_get_s24_be(p) xu_bits_get_s24_be_impl((xu_byte_t*)(p))

#        define xu_bits_set_u24_le(p, x) xu_bits_set_u24_ne_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s24_le(p, x) xu_bits_set_s24_ne_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_u24_be(p, x) xu_bits_set_u24_be_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s24_be(p, x) xu_bits_set_s24_be_impl((xu_byte_t*)(p), x)

// 32-bits
#        define xu_bits_get_u32_le(p) xu_bits_get_u32_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_s32_le(p) xu_bits_get_s32_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_u32_be(p) xu_bits_get_u32_be_impl((xu_byte_t*)(p))
#        define xu_bits_get_s32_be(p) xu_bits_get_s32_be_impl((xu_byte_t*)(p))

#        define xu_bits_set_u32_le(p, x) xu_bits_set_u32_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s32_le(p, x) xu_bits_set_u32_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_u32_be(p, x) xu_bits_set_u32_be_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s32_be(p, x) xu_bits_set_s32_be_impl((xu_byte_t*)(p), x)

// 64-bits
#        define xu_bits_get_u64_le(p) xu_bits_get_u64_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_s64_le(p) xu_bits_get_s64_ne_impl((xu_byte_t*)(p))
#        define xu_bits_get_u64_be(p) xu_bits_get_u64_be_impl((xu_byte_t*)(p))
#        define xu_bits_get_s64_be(p) xu_bits_get_s64_be_impl((xu_byte_t*)(p))

#        define xu_bits_set_u64_le(p, x) xu_bits_set_u64_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s64_le(p, x) xu_bits_set_u64_le_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_u64_be(p, x) xu_bits_set_u64_be_impl((xu_byte_t*)(p), x)
#        define xu_bits_set_s64_be(p, x) xu_bits_set_s64_be_impl((xu_byte_t*)(p), x)

#    endif /* XU_WORDS_BIGENDIAN */

#else
// 16-bits
#    define xu_bits_get_u16_le(p) xu_bits_get_u16_le_impl((xu_byte_t*)(p))
#    define xu_bits_get_s16_le(p) xu_bits_get_s16_le_impl((xu_byte_t*)(p))
#    define xu_bits_get_u16_be(p) xu_bits_get_u16_be_impl((xu_byte_t*)(p))
#    define xu_bits_get_s16_be(p) xu_bits_get_s16_be_impl((xu_byte_t*)(p))

#    define xu_bits_set_u16_le(p, x) xu_bits_set_u16_le_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_s16_le(p, x) xu_bits_set_s16_le_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_u16_be(p, x) xu_bits_set_u16_be_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_s16_be(p, x) xu_bits_set_s16_be_impl((xu_byte_t*)(p), x)

// 24-bits
#    define xu_bits_get_u24_le(p) xu_bits_get_u24_le_impl((xu_byte_t*)(p))
#    define xu_bits_get_s24_le(p) xu_bits_get_s24_le_impl((xu_byte_t*)(p))
#    define xu_bits_get_u24_be(p) xu_bits_get_u24_be_impl((xu_byte_t*)(p))
#    define xu_bits_get_s24_be(p) xu_bits_get_s24_be_impl((xu_byte_t*)(p))

#    define xu_bits_set_u24_le(p, x) xu_bits_set_u24_le_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_s24_le(p, x) xu_bits_set_s24_le_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_u24_be(p, x) xu_bits_set_u24_be_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_s24_be(p, x) xu_bits_set_s24_be_impl((xu_byte_t*)(p), x)

// 32-bits
#    define xu_bits_get_u32_le(p) xu_bits_get_u32_le_impl((xu_byte_t*)(p))
#    define xu_bits_get_s32_le(p) xu_bits_get_s32_le_impl((xu_byte_t*)(p))
#    define xu_bits_get_u32_be(p) xu_bits_get_u32_be_impl((xu_byte_t*)(p))
#    define xu_bits_get_s32_be(p) xu_bits_get_s32_be_impl((xu_byte_t*)(p))

#    define xu_bits_set_u32_le(p, x) xu_bits_set_u32_le_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_s32_le(p, x) xu_bits_set_u32_le_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_u32_be(p, x) xu_bits_set_u32_be_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_s32_be(p, x) xu_bits_set_s32_be_impl((xu_byte_t*)(p), x)

// 64-bits
#    define xu_bits_get_u64_le(p) xu_bits_get_u64_le_impl((xu_byte_t*)(p))
#    define xu_bits_get_s64_le(p) xu_bits_get_s64_le_impl((xu_byte_t*)(p))
#    define xu_bits_get_u64_be(p) xu_bits_get_u64_be_impl((xu_byte_t*)(p))
#    define xu_bits_get_s64_be(p) xu_bits_get_s64_be_impl((xu_byte_t*)(p))

#    define xu_bits_set_u64_le(p, x) xu_bits_set_u64_le_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_s64_le(p, x) xu_bits_set_u64_le_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_u64_be(p, x) xu_bits_set_u64_be_impl((xu_byte_t*)(p), x)
#    define xu_bits_set_s64_be(p, x) xu_bits_set_s64_be_impl((xu_byte_t*)(p), x)

#endif /* XU_UNALIGNED_ACCESS_ENABLE */

#ifdef XU_WORDS_BIGENDIAN
#    define xu_bits_get_u16_ne(p) xu_bits_get_u16_be(p)
#    define xu_bits_get_s16_ne(p) xu_bits_get_s16_be(p)
#    define xu_bits_get_u24_ne(p) xu_bits_get_u24_be(p)
#    define xu_bits_get_s24_ne(p) xu_bits_get_s24_be(p)
#    define xu_bits_get_u32_ne(p) xu_bits_get_u32_be(p)
#    define xu_bits_get_s32_ne(p) xu_bits_get_s32_be(p)
#    define xu_bits_get_u64_ne(p) xu_bits_get_u64_be(p)
#    define xu_bits_get_s64_ne(p) xu_bits_get_s64_be(p)

#    define xu_bits_set_u16_ne(p, x) xu_bits_set_u16_be(p, x)
#    define xu_bits_set_s16_ne(p, x) xu_bits_set_s16_be(p, x)
#    define xu_bits_set_u24_ne(p, x) xu_bits_set_u24_be(p, x)
#    define xu_bits_set_s24_ne(p, x) xu_bits_set_s24_be(p, x)
#    define xu_bits_set_u32_ne(p, x) xu_bits_set_u32_be(p, x)
#    define xu_bits_set_s32_ne(p, x) xu_bits_set_s32_be(p, x)
#    define xu_bits_set_u64_ne(p, x) xu_bits_set_u64_be(p, x)
#    define xu_bits_set_s64_ne(p, x) xu_bits_set_s64_be(p, x)

#else
#    define xu_bits_get_u16_ne(p) xu_bits_get_u16_le(p)
#    define xu_bits_get_s16_ne(p) xu_bits_get_s16_le(p)
#    define xu_bits_get_u24_ne(p) xu_bits_get_u24_le(p)
#    define xu_bits_get_s24_ne(p) xu_bits_get_s24_le(p)
#    define xu_bits_get_u32_ne(p) xu_bits_get_u32_le(p)
#    define xu_bits_get_s32_ne(p) xu_bits_get_s32_le(p)
#    define xu_bits_get_u64_ne(p) xu_bits_get_u64_le(p)
#    define xu_bits_get_s64_ne(p) xu_bits_get_s64_le(p)

#    define xu_bits_set_u16_ne(p, x) xu_bits_set_u16_le(p, x)
#    define xu_bits_set_s16_ne(p, x) xu_bits_set_s16_le(p, x)
#    define xu_bits_set_u24_ne(p, x) xu_bits_set_u24_le(p, x)
#    define xu_bits_set_s24_ne(p, x) xu_bits_set_s24_le(p, x)
#    define xu_bits_set_u32_ne(p, x) xu_bits_set_u32_le(p, x)
#    define xu_bits_set_s32_ne(p, x) xu_bits_set_s32_le(p, x)
#    define xu_bits_set_u64_ne(p, x) xu_bits_set_u64_le(p, x)
#    define xu_bits_set_s64_ne(p, x) xu_bits_set_s64_le(p, x)

#endif /* XU_WORDS_BIGENDIAN */

// swap
#ifndef xu_bits_swap_u16
#    define xu_bits_swap_u16(x) xu_bits_swap_u16_inline((xu_uint16_t)(x))
#endif

#ifndef xu_bits_swap_u24
#    define xu_bits_swap_u24(x) xu_bits_swap_u24_inline((xu_uint32_t)(x))
#endif

#ifndef xu_bits_swap_u32
#    define xu_bits_swap_u32(x) xu_bits_swap_u32_inline((xu_uint32_t)(x))
#endif

#ifndef xu_bits_swap_u64
#    define xu_bits_swap_u64(x) xu_bits_swap_u64_inline((xu_uint64_t)(x))
#endif

#ifdef XU_WORDS_BIGENDIAN
#    define xu_bits_be_to_ne_u16(x) ((xu_uint16_t)(x))
#    define xu_bits_le_to_ne_u16(x) xu_bits_swap_u16(x)
#    define xu_bits_be_to_ne_u24(x) ((xu_uint32_t)(x)&0x00ffffff)
#    define xu_bits_le_to_ne_u24(x) xu_bits_swap_u24(x)
#    define xu_bits_be_to_ne_u32(x) ((xu_uint32_t)(x))
#    define xu_bits_le_to_ne_u32(x) xu_bits_swap_u32(x)
#    define xu_bits_be_to_ne_u64(x) ((xu_uint64_t)(x))
#    define xu_bits_le_to_ne_u64(x) xu_bits_swap_u64(x)
#else
#    define xu_bits_be_to_ne_u16(x) xu_bits_swap_u16(x)
#    define xu_bits_le_to_ne_u16(x) ((xu_uint16_t)(x))
#    define xu_bits_be_to_ne_u24(x) xu_bits_swap_u24(x)
#    define xu_bits_le_to_ne_u24(x) ((xu_uint32_t)(x)&0x00ffffff)
#    define xu_bits_be_to_ne_u32(x) xu_bits_swap_u32(x)
#    define xu_bits_le_to_ne_u32(x) ((xu_uint32_t)(x))
#    define xu_bits_be_to_ne_u64(x) xu_bits_swap_u64(x)
#    define xu_bits_le_to_ne_u64(x) ((xu_uint64_t)(x))
#endif

#define xu_bits_ne_to_be_u16(x) xu_bits_be_to_ne_u16(x)
#define xu_bits_ne_to_le_u16(x) xu_bits_le_to_ne_u16(x)
#define xu_bits_ne_to_be_u24(x) xu_bits_be_to_ne_u24(x)
#define xu_bits_ne_to_le_u24(x) xu_bits_le_to_ne_u24(x)
#define xu_bits_ne_to_be_u32(x) xu_bits_be_to_ne_u32(x)
#define xu_bits_ne_to_le_u32(x) xu_bits_le_to_ne_u32(x)
#define xu_bits_ne_to_be_u64(x) xu_bits_be_to_ne_u64(x)
#define xu_bits_ne_to_le_u64(x) xu_bits_le_to_ne_u64(x)

// cl0, count leading bit 0
#ifndef xu_bits_cl0_u32_be
#    define xu_bits_cl0_u32_be(x) xu_bits_cl0_u32_be_inline(x)
#endif
#ifndef xu_bits_cl0_u32_le
#    define xu_bits_cl0_u32_le(x) xu_bits_cl0_u32_le_inline(x)
#endif
#ifndef xu_bits_cl0_u64_be
#    define xu_bits_cl0_u64_be(x) xu_bits_cl0_u64_be_inline(x)
#endif
#ifndef xu_bits_cl0_u64_le
#    define xu_bits_cl0_u64_le(x) xu_bits_cl0_u64_le_inline(x)
#endif

// cl1, count leading bit 1
#ifndef xu_bits_cl1_u32_be
#    define xu_bits_cl1_u32_be(x) xu_bits_cl0_u32_be(~(xu_uint32_t)(x))
#endif
#ifndef xu_bits_cl1_u32_le
#    define xu_bits_cl1_u32_le(x) xu_bits_cl0_u32_le(~(xu_uint32_t)(x))
#endif
#ifndef xu_bits_cl1_u64_be
#    define xu_bits_cl1_u64_be(x) xu_bits_cl0_u64_be(~(xu_uint64_t)(x))
#endif
#ifndef xu_bits_cl1_u64_le
#    define xu_bits_cl1_u64_le(x) xu_bits_cl0_u64_le(~(xu_uint64_t)(x))
#endif

// cb1, count bit 1
#ifndef xu_bits_cb1_u32
#    define xu_bits_cb1_u32(x) xu_bits_cb1_u32_inline(x)
#endif
#ifndef xu_bits_cb1_u64
#    define xu_bits_cb1_u64(x) xu_bits_cb1_u64_inline(x)
#endif

// cb0, count bit 0
#ifndef xu_bits_cb0_u32
#    define xu_bits_cb0_u32(x) ((x) ? (xu_size_t)xu_bits_cb1_u32(~(xu_uint32_t)(x)) : 32)
#endif
#ifndef xu_bits_cb0_u64
#    define xu_bits_cb0_u64(x) ((x) ? (xu_size_t)xu_bits_cb1_u64(~(xu_uint64_t)(x)) : 64)
#endif

/* fb0, find the first bit 0
 *
 * find bit zero by little endian, fb0(...11101101) == 1
 * find bit zero by big endian, fb0(...11101101) == 27
 */
#ifndef xu_bits_fb0_u32_be
#    define xu_bits_fb0_u32_be(x) ((x) ? xu_bits_cl0_u32_be(~(xu_uint32_t)(x)) : 0)
#endif
#ifndef xu_bits_fb0_u32_le
#    define xu_bits_fb0_u32_le(x) ((x) ? xu_bits_cl0_u32_le(~(xu_uint32_t)(x)) : 0)
#endif
#ifndef xu_bits_fb0_u64_be
#    define xu_bits_fb0_u64_be(x) ((x) ? xu_bits_cl0_u64_be(~(xu_uint64_t)(x)) : 0)
#endif
#ifndef xu_bits_fb0_u64_le
#    define xu_bits_fb0_u64_le(x) ((x) ? xu_bits_cl0_u64_le(~(xu_uint64_t)(x)) : 0)
#endif

// fb1, find the first bit 1
#ifndef xu_bits_fb1_u32_be
#    define xu_bits_fb1_u32_be(x) ((x) ? xu_bits_cl0_u32_be(x) : 32)
#endif
#ifndef xu_bits_fb1_u32_le
#    define xu_bits_fb1_u32_le(x) ((x) ? xu_bits_cl0_u32_le(x) : 32)
#endif
#ifndef xu_bits_fb1_u64_be
#    define xu_bits_fb1_u64_be(x) ((x) ? xu_bits_cl0_u64_be(x) : 64)
#endif
#ifndef xu_bits_fb1_u64_le
#    define xu_bits_fb1_u64_le(x) ((x) ? xu_bits_cl0_u64_le(x) : 64)
#endif

// only for xu_size_t
#if XU_CPU_BIT64

#    define xu_bits_swap(x) xu_bits_swap_u64(x)

#    define xu_bits_cl0_be(x) xu_bits_cl0_u64_be(x)
#    define xu_bits_cl0_le(x) xu_bits_cl0_u64_le(x)
#    define xu_bits_cl1_be(x) xu_bits_cl1_u64_be(x)
#    define xu_bits_cl1_le(x) xu_bits_cl1_u64_le(x)

#    define xu_bits_fb0_be(x) xu_bits_fb0_u64_be(x)
#    define xu_bits_fb0_le(x) xu_bits_fb0_u64_le(x)
#    define xu_bits_fb1_be(x) xu_bits_fb1_u64_be(x)
#    define xu_bits_fb1_le(x) xu_bits_fb1_u64_le(x)

#    define xu_bits_cb0(x) xu_bits_cb0_u64(x)
#    define xu_bits_cb1(x) xu_bits_cb1_u64(x)

#elif XU_CPU_BIT32

#    define xu_bits_swap(x) xu_bits_swap_u32(x)

#    define xu_bits_cl0_be(x) xu_bits_cl0_u32_be(x)
#    define xu_bits_cl0_le(x) xu_bits_cl0_u32_le(x)
#    define xu_bits_cl1_be(x) xu_bits_cl1_u32_be(x)
#    define xu_bits_cl1_le(x) xu_bits_cl1_u32_le(x)

#    define xu_bits_fb0_be(x) xu_bits_fb0_u32_be(x)
#    define xu_bits_fb0_le(x) xu_bits_fb0_u32_le(x)
#    define xu_bits_fb1_be(x) xu_bits_fb1_u32_be(x)
#    define xu_bits_fb1_le(x) xu_bits_fb1_u32_le(x)

#    define xu_bits_cb0(x) xu_bits_cb0_u32(x)
#    define xu_bits_cb1(x) xu_bits_cb1_u32(x)

#endif

/* *******************************************************
 * interfaces
 */

/*! get ubits32 integer
 *
 * @param p     the data pointer
 * @param b     the start bits
 * @param n     the bits count
 *
 * @return      the ubits32 integer
 */
xu_uint32_t xu_bits_get_ubits32(xu_byte_t const* p, xu_size_t b, xu_size_t n);

/*! get sbits32 integer
 *
 * @param p     the data pointer
 * @param b     the start bits
 * @param n     the bits count
 *
 * @return      the ubits32 integer
 */
xu_sint32_t xu_bits_get_sbits32(xu_byte_t const* p, xu_size_t b, xu_size_t n);

/*! set ubits32 integer
 *
 * @param p     the data pointer
 * @param b     the start bits
 * @param x     the value
 * @param n     the bits count
 */
xu_void_t xu_bits_set_ubits32(xu_byte_t* p, xu_size_t b, xu_uint32_t x, xu_size_t n);

/*! set ubits32 integer
 *
 * @param p     the data pointer
 * @param b     the start bits
 * @param x     the value
 * @param n     the bits count
 */
xu_void_t xu_bits_set_sbits32(xu_byte_t* p, xu_size_t b, xu_sint32_t x, xu_size_t n);

/*! get ubits64 integer
 *
 * @param p     the data pointer
 * @param b     the start bits
 * @param n     the bits count
 *
 * @return      the ubits64 integer
 */
xu_uint64_t xu_bits_get_ubits64(xu_byte_t const* p, xu_size_t b, xu_size_t n);

/*! get sbits64 integer
 *
 * @param p     the data pointer
 * @param b     the start bits
 * @param n     the bits count
 *
 * @return      the ubits64 integer
 */
xu_sint64_t xu_bits_get_sbits64(xu_byte_t const* p, xu_size_t b, xu_size_t n);

/* *******************************************************
 * bits
 */

static __xu_inline__ xu_void_t xu_bits_set_u16_le_inline(xu_byte_t* p, xu_uint16_t x)
{
    p[0] = (xu_byte_t)x;
    p[1] = (xu_byte_t)(x >> 8);
}
static __xu_inline__ xu_void_t xu_bits_set_u16_be_inline(xu_byte_t* p, xu_uint16_t x)
{
    p[0] = (xu_byte_t)(x >> 8);
    p[1] = (xu_byte_t)x;
}
static __xu_inline__ xu_void_t xu_bits_set_u24_le_inline(xu_byte_t* p, xu_uint32_t x)
{
    p[0] = (xu_byte_t)x;
    p[1] = (xu_byte_t)(x >> 8);
    p[2] = (xu_byte_t)(x >> 16);
}
static __xu_inline__ xu_void_t xu_bits_set_u24_be_inline(xu_byte_t* p, xu_uint32_t x)
{
    p[0] = (xu_byte_t)(x >> 16);
    p[1] = (xu_byte_t)(x >> 8);
    p[2] = (xu_byte_t)x;
}
static __xu_inline__ xu_void_t xu_bits_set_u32_le_inline(xu_byte_t* p, xu_uint32_t x)
{
    p[0] = (xu_byte_t)x;
    p[1] = (xu_byte_t)(x >> 8);
    p[2] = (xu_byte_t)(x >> 16);
    p[3] = (xu_byte_t)(x >> 24);
}
static __xu_inline__ xu_void_t xu_bits_set_u32_be_inline(xu_byte_t* p, xu_uint32_t x)
{
    p[0] = (xu_byte_t)(x >> 24);
    p[1] = (xu_byte_t)(x >> 16);
    p[2] = (xu_byte_t)(x >> 8);
    p[3] = (xu_byte_t)x;
}
static __xu_inline__ xu_void_t xu_bits_set_u64_le_inline(xu_byte_t* p, xu_uint64_t x)
{
    p[0] = (xu_byte_t)x;
    p[1] = (xu_byte_t)(x >> 8);
    p[2] = (xu_byte_t)(x >> 16);
    p[3] = (xu_byte_t)(x >> 24);
    p[4] = (xu_byte_t)(x >> 32);
    p[5] = (xu_byte_t)(x >> 40);
    p[6] = (xu_byte_t)(x >> 48);
    p[7] = (xu_byte_t)(x >> 56);
}
static __xu_inline__ xu_void_t xu_bits_set_u64_be_inline(xu_byte_t* p, xu_uint64_t x)
{
    p[0] = (xu_byte_t)(x >> 56);
    p[1] = (xu_byte_t)(x >> 48);
    p[2] = (xu_byte_t)(x >> 40);
    p[3] = (xu_byte_t)(x >> 32);
    p[4] = (xu_byte_t)(x >> 24);
    p[5] = (xu_byte_t)(x >> 16);
    p[6] = (xu_byte_t)(x >> 8);
    p[7] = (xu_byte_t)x;
}

/* *******************************************************
 * swap
 */

// swap
static __xu_inline__ xu_uint16_t xu_bits_swap_u16_inline(xu_uint16_t x)
{
    x = (x >> 8) | (x << 8);
    return x;
}
static __xu_inline__ xu_uint32_t xu_bits_swap_u24_inline(xu_uint32_t x)
{
    return (x >> 16) | (x & 0x0000ff00) | (x << 16);
}
static __xu_inline__ xu_uint32_t xu_bits_swap_u32_inline(xu_uint32_t x)
{
    x = ((x << 8) & 0xff00ff00) | ((x >> 8) & 0x00ff00ff);
    x = (x >> 16) | (x << 16);
    return x;
}
static __xu_inline__ xu_hize_t xu_bits_swap_u64_inline(xu_hize_t x)
{
    union
    {
        xu_hize_t   u64;
        xu_uint32_t u32[2];

    } w, r;

    w.u64 = x;

    r.u32[0] = xu_bits_swap_u32(w.u32[1]);
    r.u32[1] = xu_bits_swap_u32(w.u32[0]);

    return r.u64;
}

/* *******************************************************
 * cl0
 */
static __xu_inline__ xu_size_t xu_bits_cl0_u32_be_inline(xu_uint32_t x)
{
    xu_check_return_val(x, 32);

    xu_size_t n = 31;
    if (x & 0xffff0000)
    {
        n -= 16;
        x >>= 16;
    }
    if (x & 0xff00)
    {
        n -= 8;
        x >>= 8;
    }
    if (x & 0xf0)
    {
        n -= 4;
        x >>= 4;
    }
    if (x & 0xc)
    {
        n -= 2;
        x >>= 2;
    }
    if (x & 0x2)
    {
        n--;
    }

    return n;
}
static __xu_inline__ xu_size_t xu_bits_cl0_u32_le_inline(xu_uint32_t x)
{
    xu_check_return_val(x, 32);

    xu_size_t n = 31;
    if (x & 0x0000ffff)
    {
        n -= 16;
    }
    else
        x >>= 16;
    if (x & 0x00ff)
    {
        n -= 8;
    }
    else
        x >>= 8;
    if (x & 0x0f)
    {
        n -= 4;
    }
    else
        x >>= 4;
    if (x & 0x3)
    {
        n -= 2;
    }
    else
        x >>= 2;
    if (x & 0x1)
    {
        n--;
    }

    return n;
}
static __xu_inline__ xu_size_t xu_bits_cl0_u64_be_inline(xu_uint64_t x)
{
    xu_check_return_val(x, 64);

    xu_size_t n = xu_bits_cl0_u32_be((xu_uint32_t)(x >> 32));
    if (n == 32) n += xu_bits_cl0_u32_be((xu_uint32_t)x);

    return n;
}
static __xu_inline__ xu_size_t xu_bits_cl0_u64_le_inline(xu_uint64_t x)
{
    xu_check_return_val(x, 64);

    xu_size_t n = xu_bits_cl0_u32_le((xu_uint32_t)x);
    if (n == 32) n += xu_bits_cl0_u32_le((xu_uint32_t)(x >> 32));

    return n;
}

/* *******************************************************
 * cb1
 */
static __xu_inline__ xu_size_t xu_bits_cb1_u32_inline(xu_uint32_t x)
{
    xu_check_return_val(x, 0);

#if 0
    /*
     * 0x55555555 = 01010101010101010101010101010101
     * 0x33333333 = 00110011001100110011001100110011
     * 0x0f0f0f0f = 00001111000011110000111100001111
     * 0x00ff00ff = 00000000111111110000000011111111
     * 0x0000ffff = 00000000000000001111111111111111
     */

    x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x & 0x0f0f0f0f) + ((x >> 4) & 0x0f0f0f0f);
    x = (x & 0x00ff00ff) + ((x >> 8) & 0x00ff00ff);
    x = (x & 0x0000ffff) + ((x >> 16) & 0x0000ffff);
#elif 0
    // mit hackmem count
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x &= 0x7f;
#elif 0
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4) & 0x0f0f0f0f);
    x = (x * 0x01010101) >> 24;
#elif 0
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4) & 0x0f0f0f0f) % 255;
#else
    x = x - ((x >> 1) & 0x77777777) - ((x >> 2) & 0x33333333) - ((x >> 3) & 0x11111111);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x = (x * 0x01010101) >> 24;
#endif

    return x;
}
static __xu_inline__ xu_size_t xu_bits_cb1_u64_inline(xu_uint64_t x)
{
    xu_check_return_val(x, 0);

#if 0
    x = x - ((x >> 1) & 0x5555555555555555L);
    x = (x & 0x3333333333333333L) + ((x >> 2) & 0x3333333333333333L);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fL;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    x &= 0x7f;
#else
    x = x - ((x >> 1) & 0x7777777777777777ULL) - ((x >> 2) & 0x3333333333333333ULL) -
        ((x >> 3) & 0x1111111111111111ULL);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    x = (x * 0x0101010101010101ULL) >> 56;
#endif

    return (xu_size_t)x;
}

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
/* *******************************************************
 * float
 */
static __xu_inline__ xu_float_t xu_bits_get_float_le_inline(xu_byte_t const* p)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && !defined(XU_WORDS_BIGENDIAN)
    return *((xu_float_t*)p);
#    else
    xu_ieee_float_t conv;
    conv.i = xu_bits_get_u32_le(p);
    return conv.f;
#    endif
}
static __xu_inline__ xu_float_t xu_bits_get_float_be_inline(xu_byte_t const* p)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && defined(XU_WORDS_BIGENDIAN)
    return *((xu_float_t*)p);
#    else
    xu_ieee_float_t conv;
    conv.i = xu_bits_get_u32_be(p);
    return conv.f;
#    endif
}
static __xu_inline__ xu_void_t xu_bits_set_float_le_inline(xu_byte_t* p, xu_float_t x)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && !defined(XU_WORDS_BIGENDIAN)
    *((xu_float_t*)p) = x;
#    else
    xu_ieee_float_t conv;
    conv.f = x;
    xu_bits_set_u32_le(p, conv.i);
#    endif
}
static __xu_inline__ xu_void_t xu_bits_set_float_be_inline(xu_byte_t* p, xu_float_t x)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && defined(XU_WORDS_BIGENDIAN)
    *((xu_float_t*)p) = x;
#    else
    xu_ieee_float_t conv;
    conv.f = x;
    xu_bits_set_u32_be(p, conv.i);
#    endif
}
/* *******************************************************
 * double
 */
static __xu_inline__ xu_double_t xu_bits_get_double_bbe_inline(xu_byte_t const* p)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && defined(XU_FLOAT_BIGENDIAN) && defined(XU_WORDS_BIGENDIAN)
    return *((xu_double_t*)p);
#    else
    union
    {
        xu_uint32_t i[2];
        xu_double_t d;

    } conv;

    conv.i[1] = xu_bits_get_u32_be(p);
    conv.i[0] = xu_bits_get_u32_be(p + 4);

    return conv.d;
#    endif
}
static __xu_inline__ xu_double_t xu_bits_get_double_ble_inline(xu_byte_t const* p)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && defined(XU_FLOAT_BIGENDIAN) && !defined(XU_WORDS_BIGENDIAN)
    return *((xu_double_t*)p);
#    else
    union
    {
        xu_uint32_t i[2];
        xu_double_t d;

    } conv;

    conv.i[1] = xu_bits_get_u32_le(p);
    conv.i[0] = xu_bits_get_u32_le(p + 4);

    return conv.d;
#    endif
}
static __xu_inline__ xu_double_t xu_bits_get_double_lbe_inline(xu_byte_t const* p)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && !defined(XU_FLOAT_BIGENDIAN) && defined(XU_WORDS_BIGENDIAN)
    return *((xu_double_t*)p);
#    else
    union
    {
        xu_uint32_t i[2];
        xu_double_t d;

    } conv;

    conv.i[0] = xu_bits_get_u32_be(p);
    conv.i[1] = xu_bits_get_u32_be(p + 4);

    return conv.d;
#    endif
}
static __xu_inline__ xu_double_t xu_bits_get_double_lle_inline(xu_byte_t const* p)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && !defined(XU_FLOAT_BIGENDIAN) && !defined(XU_WORDS_BIGENDIAN)
    return *((xu_double_t*)p);
#    else
    union
    {
        xu_uint32_t i[2];
        xu_double_t d;

    } conv;

    conv.i[0] = xu_bits_get_u32_le(p);
    conv.i[1] = xu_bits_get_u32_le(p + 4);
    return conv.d;
#    endif
}
// big double endian & big words endian
// 7 6 5 4 3 2 1 0
static __xu_inline__ xu_void_t xu_bits_set_double_bbe_inline(xu_byte_t* p, xu_double_t x)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && defined(XU_FLOAT_BIGENDIAN) && defined(XU_WORDS_BIGENDIAN)
    *((xu_double_t*)p) = x;
#    else
    union
    {
        xu_uint32_t i[2];
        xu_double_t d;

    } conv;

    conv.d = x;

    xu_bits_set_u32_be(p, conv.i[1]);
    xu_bits_set_u32_be(p + 4, conv.i[0]);
#    endif
}
// big double endian & little words endian
// 4 5 6 7 0 1 2 3
static __xu_inline__ xu_void_t xu_bits_set_double_ble_inline(xu_byte_t* p, xu_double_t x)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && defined(XU_FLOAT_BIGENDIAN) && !defined(XU_WORDS_BIGENDIAN)
    *((xu_double_t*)p) = x;
#    else
    union
    {
        xu_uint32_t i[2];
        xu_double_t d;

    } conv;

    conv.d = x;

    xu_bits_set_u32_le(p, conv.i[1]);
    xu_bits_set_u32_le(p + 4, conv.i[0]);
#    endif
}
// little double endian & big words endian
// 3 2 1 0 7 6 5 4
static __xu_inline__ xu_void_t xu_bits_set_double_lbe_inline(xu_byte_t* p, xu_double_t x)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && !defined(XU_FLOAT_BIGENDIAN) && defined(XU_WORDS_BIGENDIAN)
    *((xu_double_t*)p) = x;
#    else
    union
    {
        xu_uint32_t i[2];
        xu_double_t d;

    } conv;

    conv.d = x;

    xu_bits_set_u32_be(p, conv.i[0]);
    xu_bits_set_u32_be(p + 4, conv.i[1]);
#    endif
}
// little double endian & little words endian
// 0 1 2 3 4 5 6 7
static __xu_inline__ xu_void_t xu_bits_set_double_lle_inline(xu_byte_t* p, xu_double_t x)
{
#    if defined(XU_UNALIGNED_ACCESS_ENABLE) && !defined(XU_FLOAT_BIGENDIAN) && !defined(XU_WORDS_BIGENDIAN)
    *((xu_double_t*)p) = x;
#    else
    union
    {
        xu_uint32_t i[2];
        xu_double_t d;

    } conv;

    conv.d = x;

    xu_bits_set_u32_le(p, conv.i[0]);
    xu_bits_set_u32_le(p + 4, conv.i[1]);
#    endif
}

#endif /* XU_CONFIG_TYPE_HAVE_FLOAT */
