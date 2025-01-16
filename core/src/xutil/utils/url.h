#pragma once

#include "xutil/prefix.h"

/*! encode the url, not encode: -_. and ' ' => '+'
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_url_encode(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);

/*! decode the url
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_url_decode(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);

/*! encode the url, not encode: -_.!~*'();/?:@&=+$,#
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_url_encode2(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);

/*! decode the url
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_url_decode2(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);

/*! encode the url arguments, not encode: -_.!~*'()
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_url_encode_args(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);

/*! decode the url arguments
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
xu_size_t xu_url_decode_args(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on);
