#pragma once

/* *******************************************************
 * includes
 */
#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 */

/*! encode base32
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_base32_encode(xu_byte_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);

/*! decode base32
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_base32_decode(xu_byte_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);
