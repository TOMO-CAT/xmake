#pragma once

#include "xutil/prefix.h"

/*! make fnv32 hash
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv32 value
 */
xu_uint32_t xu_fnv32_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed);

/*! make fnv32 hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv32 value
 */
xu_uint32_t xu_fnv32_make_from_cstr(xu_char_t const* cstr, xu_uint32_t seed);

/*! make fnv32(1a) hash
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv32 value
 */
xu_uint32_t xu_fnv32_1a_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed);

/*! make fnv32(1a) hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv32 value
 */
xu_uint32_t xu_fnv32_1a_make_from_cstr(xu_char_t const* cstr, xu_uint32_t seed);
