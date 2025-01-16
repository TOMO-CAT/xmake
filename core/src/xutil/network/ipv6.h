#pragma once

#include "xutil/libc/stdlib/stdlib.h"
#include "xutil/prefix.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */
// the ipv6 string data maxn
#define XU_IPV6_CSTR_MAXN (40 + 20)

/* *******************************************************
 *  types
 * *******************************************************
 */
/*! the ipv6 type
 *
 * <pre>
 * xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
 * xxxx::xxxx:xxxx
 * ::ffff:xxx.xxx.xxx.xxx
 * ::fe80:1%1
 * </pre>
 */
typedef struct __xu_ipv6_t
{
    /// the scope id
    xu_uint32_t scope_id;

    /// the address
    union
    {
        /// u32, little-endian
        xu_uint32_t u32[4];

        /// u16, little-endian
        xu_uint16_t u16[8];

        /// u8
        xu_uint8_t u8[16];

    } addr;

} xu_ipv6_t, *xu_ipv6_ref_t;

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

/*! clear the ipv6
 *
 * @param ipv6      the ipv6
 */
xu_void_t xu_ipv6_clear(xu_ipv6_ref_t ipv6);

/*! is any address?
 *
 * @param ipv6      the ipv6
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv6_is_any(xu_ipv6_ref_t ipv6);

/*! is loopback address?
 *
 * @param ipv6      the ipv6
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv6_is_loopback(xu_ipv6_ref_t ipv6);

/*! is linklocal address?
 *
 * @param ipv6      the ipv6
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv6_is_linklocal(xu_ipv6_ref_t ipv6);

/*! is mc linklocal address?
 *
 * @param ipv6      the ipv6
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv6_is_mc_linklocal(xu_ipv6_ref_t ipv6);

/*! is sitelocal address?
 *
 * @param ipv6      the ipv6
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv6_is_sitelocal(xu_ipv6_ref_t ipv6);

/*! is multicast address?
 *
 * @param ipv6      the ipv6
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv6_is_multicast(xu_ipv6_ref_t ipv6);

/*! is equal?
 *
 * @param ipv6      the ipv6
 * @param other     the other ipv6
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv6_is_equal(xu_ipv6_ref_t ipv6, xu_ipv6_ref_t other);

/*! get the ipv6 string
 *
 * @param ipv6      the ipv6
 * @param data      the ipv6 string data
 * @param maxn      the ipv6 string data maxn
 *
 * @return          the ipv6 string
 */
xu_char_t const* xu_ipv6_cstr(xu_ipv6_ref_t ipv6, xu_char_t* data, xu_size_t maxn);

/*! set the ipv6 from string
 *
 * @param ipv6      the ipv6
 * @param cstr      the ipv6 string
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipv6_cstr_set(xu_ipv6_ref_t ipv6, xu_char_t const* cstr);
