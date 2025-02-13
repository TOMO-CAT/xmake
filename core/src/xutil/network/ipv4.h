#pragma once

#include "xutil/prefix.h"

// the ipv4 string data maxn
#define XU_IPV4_CSTR_MAXN (16)

/*! the ipv4 type
 *
 * xxx.xxx.xxx.xxx
 */
typedef union __xu_ipv4_t
{
    /// u32, little-endian
    xu_uint32_t u32;

    /// u16
    xu_uint16_t u16[2];

    /// u8
    xu_uint8_t u8[4];

} xu_ipv4_t, *xu_ipv4_ref_t;

/* *******************************************************
 * interfaces
 */

/*! clear the ipv4
 *
 * @param ipv4      the ipv4
 */
xu_void_t xu_ipv4_clear(xu_ipv4_ref_t ipv4);

/*! is any address?
 *
 * @param ipv4      the ipv4
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv4_is_any(xu_ipv4_ref_t ipv4);

/*! is loopback address?
 *
 * @param ipv4      the ipv4
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv4_is_loopback(xu_ipv4_ref_t ipv4);

/*! is equal?
 *
 * @param ipv4      the ipv4
 * @param other     the other ipv4
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv4_is_equal(xu_ipv4_ref_t ipv4, xu_ipv4_ref_t other);

/*! get the ipv4 string
 *
 * @param ipv4      the ipv4
 * @param data      the ipv4 data
 * @param maxn      the data maxn
 *
 * @return          the ipv4 address
 */
xu_char_t const* xu_ipv4_cstr(xu_ipv4_ref_t ipv4, xu_char_t* data, xu_size_t maxn);

/*! set the ipv4 from string
 *
 * @param ipv4      the ipv4
 * @param cstr      the ipv4 string
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv4_cstr_set(xu_ipv4_ref_t ipv4, xu_char_t const* cstr);
