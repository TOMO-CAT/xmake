#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

// encode value
#define xu_crc8_make_value(mode, crc, value) xu_crc8_make(mode, crc, (xu_byte_t const*)&(value), sizeof(value))

/* *******************************************************
 * interfaces
 */

/*! make crc8 (ATM)
 *
 * @param data      the input data
 * @param size      the input size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint8_t xu_crc8_make(xu_byte_t const* data, xu_size_t size, xu_uint8_t seed);

/*! make crc8 (ATM) for cstr
 *
 * @param cstr      the input cstr
 * @param seed      uses this seed if be non-zero
 *
 * @return          the crc value
 */
xu_uint8_t xu_crc8_make_from_cstr(xu_char_t const* cstr, xu_uint8_t seed);
