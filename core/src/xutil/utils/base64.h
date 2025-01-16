#pragma once

#include "xutil/prefix.h"

/*! encode base64
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_base64_encode(xu_byte_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);

/*! encode base64
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_base64_decode(xu_char_t const* ib, xu_size_t in, xu_byte_t* ob, xu_size_t on);
