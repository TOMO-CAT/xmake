#pragma once

#include "xutil/prefix.h"

// encode value
#define xu_crc32_make_value(mode, crc, value)       xu_crc32_make(mode, crc, (xu_byte_t const*)&(value), sizeof(value))


/*! make crc32 (IEEE)
 *
 * @param data      the input data
 * @param size      the input size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint32_t         xu_crc32_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed);

/*! make crc32 (IEEE) for cstr
 *
 * @param cstr      the input cstr
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint32_t         xu_crc32_make_from_cstr(xu_char_t const* cstr, xu_uint32_t seed);

/*! make crc32 (IEEE LE)
 *
 * @param data      the input data
 * @param size      the input size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint32_t         xu_crc32_le_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed);

/*! make crc32 (IEEE LE) for cstr
 *
 * @param cstr      the input cstr
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint32_t         xu_crc32_le_make_from_cstr(xu_char_t const* cstr, xu_uint32_t seed);
