#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

// swap
#if XU_COMPILER_VERSION_BE(4, 3)
#    define xu_bits_swap_u32(x) __builtin_bswap32(x)
#    define xu_bits_swap_u64(x) __builtin_bswap64(x)
#endif

// cl0
#if XU_COMPILER_VERSION_BE(4, 1)
#    define xu_bits_cl0_u32_be(x) ((x) ? (xu_size_t)__builtin_clz((xu_uint32_t)(x)) : 32)
#    define xu_bits_cl0_u32_le(x) ((x) ? (xu_size_t)__builtin_ctz((xu_uint32_t)(x)) : 32)
#    define xu_bits_cl0_u64_be(x) ((x) ? (xu_size_t)__builtin_clzll((xu_uint64_t)(x)) : 64)
#    define xu_bits_cl0_u64_le(x) ((x) ? (xu_size_t)__builtin_ctzll((xu_uint64_t)(x)) : 64)
#endif

// cb1
#if XU_COMPILER_VERSION_BE(4, 1)
#    define xu_bits_cb1_u32(x) ((x) ? (xu_size_t)__builtin_popcount((xu_uint32_t)(x)) : 0)
#    define xu_bits_cb1_u64(x) ((x) ? (xu_size_t)__builtin_popcountll((xu_uint64_t)(x)) : 0)
#endif

// fb1
#if XU_COMPILER_VERSION_BE(4, 1)
#    define xu_bits_fb1_u32_le(x) ((x) ? (xu_size_t)__builtin_ffs((xu_uint32_t)(x)) - 1 : 32)
#    define xu_bits_fb1_u64_le(x) ((x) ? (xu_size_t)__builtin_ffsll((xu_uint64_t)(x)) - 1 : 64)
#endif
