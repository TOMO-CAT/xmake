#pragma once

#include "xutil/prefix.h"

/*! make murmur hash
 *
 * one-byte-at-a-time hash based on Murmur's mix
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the murmur value
 */
xu_size_t           xu_murmur_make(xu_byte_t const* data, xu_size_t size, xu_size_t seed);

/*! make murmur hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the murmur value
 */
xu_size_t           xu_murmur_make_from_cstr(xu_char_t const* cstr, xu_size_t seed);
