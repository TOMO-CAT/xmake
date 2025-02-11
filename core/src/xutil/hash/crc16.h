#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

// encode value
#define xu_crc16_make_value(mode, crc, value) xu_crc16_make(mode, crc, (xu_byte_t const*)&(value), sizeof(value))

/* *******************************************************
 * interfaces
 */

/*! make crc16 (ANSI)
 *
 * @param data      the input data
 * @param size      the input size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint16_t xu_crc16_make(xu_byte_t const* data, xu_size_t size, xu_uint16_t seed);

/*! make crc16 (ANSI) for cstr
 *
 * @param cstr      the input cstr
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint16_t xu_crc16_make_from_cstr(xu_char_t const* cstr, xu_uint16_t seed);

/*! make crc16 (CCITT)
 *
 * @param data      the input data
 * @param size      the input size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint16_t xu_crc16_ccitt_make(xu_byte_t const* data, xu_size_t size, xu_uint16_t seed);

/*! make crc16 (CCITT) for cstr
 *
 * @param cstr      the input cstr
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint16_t xu_crc16_ccitt_make_from_cstr(xu_char_t const* cstr, xu_uint16_t seed);
