#pragma once

#include "xutil/prefix.h"

// data structure for sha (message data) computation
typedef struct __xu_sha_t
{
    xu_uint8_t  digest_len; //!< digest length in 32-bit words
    xu_hize_t   count;      //!< number of bytes in buffer
    xu_uint8_t  buffer[64]; //!< 512-bit buffer of input values used in hash updating
    xu_uint32_t state[8];   //!< current hash value
    xu_void_t (*transform)(xu_uint32_t* state, xu_uint8_t const buffer[64]);

} xu_sha_t;

// the sha mode type
typedef enum __xu_sha_mode_t
{
    XU_SHA_MODE_SHA1_160 = 160,
    XU_SHA_MODE_SHA2_224 = 224,
    XU_SHA_MODE_SHA2_256 = 256

} xu_sha_mode_t;

/*! init sha
 *
 * @param sha           the sha
 * @param mode          the mode
 */
xu_void_t xu_sha_init(xu_sha_t* sha, xu_size_t mode);

/*! exit sha
 *
 * @param sha           the sha
 * @param data          the data
 * @param size          the size
 */
xu_void_t xu_sha_exit(xu_sha_t* sha, xu_byte_t* data, xu_size_t size);

/*! spak sha
 *
 * @param sha           the sha
 * @param data          the data
 * @param size          the size
 */
xu_void_t xu_sha_spak(xu_sha_t* sha, xu_byte_t const* data, xu_size_t size);

/*! make sha
 *
 * @param mode          the mode
 * @param ib            the input data
 * @param ip            the input size
 * @param ob            the output data
 * @param on            the output size
 *
 * @return              the real size
 */
xu_size_t xu_sha_make(xu_size_t mode, xu_byte_t const* ib, xu_size_t ip, xu_byte_t* ob, xu_size_t on);
