#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 */

/*! make fnv64 hash
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv64 value
 */
xu_uint64_t xu_fnv64_make(xu_byte_t const* data, xu_size_t size, xu_uint64_t seed);

/*! make fnv64 hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv64 value
 */
xu_uint64_t xu_fnv64_make_from_cstr(xu_char_t const* cstr, xu_uint64_t seed);

/*! make fnv64(1a) hash
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv64 value
 */
xu_uint64_t xu_fnv64_1a_make(xu_byte_t const* data, xu_size_t size, xu_uint64_t seed);

/*! make fnv64(1a) hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv64 value
 */
xu_uint64_t xu_fnv64_1a_make_from_cstr(xu_char_t const* cstr, xu_uint64_t seed);
