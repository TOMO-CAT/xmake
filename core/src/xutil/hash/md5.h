#pragma once

#include "xutil/prefix.h"

// data structure for md5 (message data) computation
typedef struct __xu_md5_t
{
    xu_uint32_t i[2];     //!< number of _bits_ handled mod 2^64
    xu_uint32_t sp[4];    //!< scratch buffer
    xu_byte_t   ip[64];   //!< input buffer
    xu_byte_t   data[16]; //!< actual data after xu_md5_exit call

} xu_md5_t;

/*! init md5
 *
 * @param md5           the md5
 * @param pseudo_rand   the pseudo rand
 */
xu_void_t xu_md5_init(xu_md5_t* md5, xu_uint32_t pseudo_rand);

/*! exit md5
 *
 * @param md5           the md5
 * @param data          the data
 * @param size          the size
 */
xu_void_t xu_md5_exit(xu_md5_t* md5, xu_byte_t* data, xu_size_t size);

/*! spak md5
 *
 * @param md5           the md5
 * @param data          the data
 * @param size          the size
 */
xu_void_t xu_md5_spak(xu_md5_t* md5, xu_byte_t const* data, xu_size_t size);

/*! make md5
 *
 * @param ib            the input data
 * @param in            the input size
 * @param ob            the output data
 * @param on            the output size
 *
 * @return              the real size
 */
xu_size_t xu_md5_make(xu_byte_t const* ib, xu_size_t in, xu_byte_t* ob, xu_size_t on);
