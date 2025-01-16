#pragma once

#include "xutil/prefix.h"

/*! make adler32 hash
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the adler32 value
 */
xu_uint32_t xu_adler32_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed);

/*! make adler32 hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the adler32 value
 */
xu_uint32_t xu_adler32_make_from_cstr(xu_char_t const* cstr, xu_uint32_t seed);
