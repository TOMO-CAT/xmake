#pragma once

#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"
#include "xutil/prefix.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 *  types
 */

// the buffer type
typedef struct __xu_buffer_t
{
    /// the buffer data
    xu_byte_t* data;

    /// the buffer size
    xu_size_t size;

    /// the buffer maxn
    xu_size_t maxn;

    /// the static buffer
#ifdef __xu_small__
    xu_byte_t buff[32];
#else
    xu_byte_t buff[64];
#endif

} xu_buffer_t, *xu_buffer_ref_t;

/* *******************************************************
 *  interfaces
 */

/*! init the buffer
 *
 * @param buffer    the buffer
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_buffer_init(xu_buffer_ref_t buffer);

/*! exit the buffer
 *
 * @param buffer    the buffer
 */
xu_void_t xu_buffer_exit(xu_buffer_ref_t buffer);

/*! the buffer data
 *
 * @param buffer    the buffer
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_data(xu_buffer_ref_t buffer);

/*! the buffer data size
 *
 * @param buffer    the buffer
 *
 * @return          the buffer data size
 */
xu_size_t xu_buffer_size(xu_buffer_ref_t buffer);

/*! the buffer data maxn
 *
 * @param buffer    the buffer
 *
 * @return          the buffer data maxn
 */
xu_size_t xu_buffer_maxn(xu_buffer_ref_t buffer);

/*! clear the buffer
 *
 * @param buffer    the buffer
 */
xu_void_t xu_buffer_clear(xu_buffer_ref_t buffer);

/*! resize the buffer size
 *
 * @param buffer    the buffer
 * @param size      the new buffer size
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_resize(xu_buffer_ref_t buffer, xu_size_t size);

/*! memset: b => 0 ... e
 *
 * @param buffer    the buffer
 * @param b         the filled byte
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memset(xu_buffer_ref_t buffer, xu_byte_t b);

/*! memset: b => p ... e
 *
 * @param buffer    the buffer
 * @param p         the start position
 * @param b         the filled byte
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memsetp(xu_buffer_ref_t buffer, xu_size_t p, xu_byte_t b);

/*! memset: b => 0 ... n
 *
 * @param buffer    the buffer
 * @param b         the filled byte
 * @param n         the filled count
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memnset(xu_buffer_ref_t buffer, xu_byte_t b, xu_size_t n);

/*! memset: b => p ... n
 *
 * @param buffer    the buffer
 * @param p         the start position
 * @param b         the filled byte
 * @param n         the filled count
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memnsetp(xu_buffer_ref_t buffer, xu_size_t p, xu_byte_t b, xu_size_t n);

/*! memcpy: b => 0 ...
 *
 * @param buffer    the buffer
 * @param b         the copied buffer
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memcpy(xu_buffer_ref_t buffer, xu_buffer_ref_t b);

/*! memcpy: b => p ...
 *
 * @param buffer    the buffer
 * @param p         the start position
 * @param b         the copied buffer
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memcpyp(xu_buffer_ref_t buffer, xu_size_t p, xu_buffer_ref_t b);

/*! memcpy: b ... n => 0 ...
 *
 * @param buffer    the buffer
 * @param b         the copied buffer
 * @param n         the copied count
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memncpy(xu_buffer_ref_t buffer, xu_byte_t const* b, xu_size_t n);

/*! memcpy: b ... n => p ...
 *
 * @param buffer    the buffer
 * @param p         the start position
 * @param b         the copied buffer
 * @param n         the copied count
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memncpyp(xu_buffer_ref_t buffer, xu_size_t p, xu_byte_t const* b, xu_size_t n);

/*! memmov: b ... e => 0 ...
 *
 * @param buffer    the buffer
 * @param b         the moved start position
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memmov(xu_buffer_ref_t buffer, xu_size_t b);

/*! memmov: b ... e => p ...
 *
 * @param buffer    the buffer
 * @param p         the moved destination position
 * @param b         the moved start position
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memmovp(xu_buffer_ref_t buffer, xu_size_t p, xu_size_t b);

/*! memmov: b ... n => 0 ...
 *
 * @param buffer    the buffer
 * @param b         the moved start position
 * @param n         the moved count
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memnmov(xu_buffer_ref_t buffer, xu_size_t b, xu_size_t n);

/*! memmov: b ... n => p ...
 *
 * @param buffer    the buffer
 * @param p         the moved destination position
 * @param b         the moved start position
 * @param n         the moved count
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memnmovp(xu_buffer_ref_t buffer, xu_size_t p, xu_size_t b, xu_size_t n);

/*! memcat: b +=> e ...
 *
 * @param buffer    the buffer
 * @param b         the concatted buffer
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memcat(xu_buffer_ref_t buffer, xu_buffer_ref_t b);

/*! memcat: b ... n +=> e ...
 *
 * @param buffer    the buffer
 * @param b         the concatted buffer
 * @param n         the concatted count
 *
 * @return          the buffer data address
 */
xu_byte_t* xu_buffer_memncat(xu_buffer_ref_t buffer, xu_byte_t const* b, xu_size_t n);
