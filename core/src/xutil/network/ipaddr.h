#pragma once

#include "xutil/network/ipv4.h"
#include "xutil/network/ipv6.h"
#include "xutil/network/unixaddr.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */
// the address string data maxn
#define XU_IPADDR_CSTR_MAXN (XU_IPV6_CSTR_MAXN + 20)

/* *******************************************************
 *  types
 * *******************************************************
 */

// the ip address family enum
typedef enum __xu_ipaddr_family_e
{
    XU_IPADDR_FAMILY_NONE = 0,
    XU_IPADDR_FAMILY_IPV4 = 1,
    XU_IPADDR_FAMILY_IPV6 = 2,
    XU_IPADDR_FAMILY_UNIX = 3

} xu_ipaddr_family_e;

// the ip address type
typedef struct __xu_ipaddr_t
{
    // the family
    xu_uint8_t family : 7;

    // have ip?
    xu_uint8_t have_ip : 1;

    // the port
    xu_uint16_t port;

    // the address
    union
    {
        // the ipv4
        xu_ipv4_t ipv4;

        // the ipv6
        xu_ipv6_t ipv6;

        // the unixaddr
        xu_unixaddr_t unixaddr;

    } u;

} xu_ipaddr_t, *xu_ipaddr_ref_t;

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

/*! clear the address
 *
 * @param ipaddr    the address
 */
xu_void_t xu_ipaddr_clear(xu_ipaddr_ref_t ipaddr);

/*! copy address, faster than *ipaddr = *other
 *
 * @param ipaddr    the address
 * @param copied    the copied address
 */
xu_void_t xu_ipaddr_copy(xu_ipaddr_ref_t ipaddr, xu_ipaddr_ref_t copied);

/*! is empty?
 *
 * @param ipaddr    the address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_is_empty(xu_ipaddr_ref_t ipaddr);

/*! is equal?
 *
 * @param ipaddr    the address
 * @param other     the other address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_is_equal(xu_ipaddr_ref_t ipaddr, xu_ipaddr_ref_t other);

/*! get the address string
 *
 * @param ipaddr    the address
 * @param data      the address string data
 * @param maxn      the address string data maxn
 *
 * @return          the address string
 */
xu_char_t const* xu_ipaddr_cstr(xu_ipaddr_ref_t ipaddr, xu_char_t* data, xu_size_t maxn);

/*! set the ip address from string
 *
 * @param ipaddr    the address, only analyze format if be null
 * @param cstr      the address string, init any address if be null
 * @param port      the port, optional
 * @param family    the address family, will analyze family automatically if be none, optional
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_set(xu_ipaddr_ref_t ipaddr, xu_char_t const* cstr, xu_uint16_t port, xu_uint8_t family);

/*! clear ip
 *
 * @param ipaddr    the address
 */
xu_void_t xu_ipaddr_ip_clear(xu_ipaddr_ref_t ipaddr);

/*! the ip is empty?
 *
 * @param ipaddr    the address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_ip_is_empty(xu_ipaddr_ref_t ipaddr);

/*! the ip is any?
 *
 * @param ipaddr    the address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_ip_is_any(xu_ipaddr_ref_t ipaddr);

/*! the ip is loopback?
 *
 * @param ipaddr    the address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_ip_is_loopback(xu_ipaddr_ref_t ipaddr);

/*! the ip is equal?
 *
 * @param ipaddr    the address
 * @param other     the other address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_ip_is_equal(xu_ipaddr_ref_t ipaddr, xu_ipaddr_ref_t other);

/*! get the ip address string
 *
 * @param ipaddr    the address
 * @param data      the address string data
 * @param maxn      the address string data maxn
 *
 * @return          the address string
 */
xu_char_t const* xu_ipaddr_ip_cstr(xu_ipaddr_ref_t ipaddr, xu_char_t* data, xu_size_t maxn);

/*! set the ip address from string
 *
 * @param ipaddr    the address, only analyze format if be null
 * @param cstr      the address string
 * @param family    the address family, will analyze family automatically if be none
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_ip_cstr_set(xu_ipaddr_ref_t ipaddr, xu_char_t const* cstr, xu_uint8_t family);

/*! only set ip address
 *
 * @param ipaddr    the address
 * @param other     the other address with ip data, clear it if be null
 */
xu_void_t xu_ipaddr_ip_set(xu_ipaddr_ref_t ipaddr, xu_ipaddr_ref_t other);

/*! get the ipv4 address
 *
 * @param ipaddr    the address
 *
 * @return          the ipv4
 */
xu_ipv4_ref_t xu_ipaddr_ipv4(xu_ipaddr_ref_t ipaddr);

/*! set the address from ipv4
 *
 * @param ipaddr    the address
 * @param ipv4      the ipv4, clear it if be null
 */
xu_void_t xu_ipaddr_ipv4_set(xu_ipaddr_ref_t ipaddr, xu_ipv4_ref_t ipv4);

/*! get the ipv6 address
 *
 * @param ipaddr    the address
 *
 * @return          the ipv6
 */
xu_ipv6_ref_t xu_ipaddr_ipv6(xu_ipaddr_ref_t ipaddr);

/*! set the address from ipv6
 *
 * @param ipaddr    the address
 * @param ipv6      the ipv6, clear it if be null
 */
xu_void_t xu_ipaddr_ipv6_set(xu_ipaddr_ref_t ipaddr, xu_ipv6_ref_t ipv6);

/*! get the unix socket path
 *
 * @param ipaddr    the address
 *
 * @return          the unixaddr
 */
xu_unixaddr_ref_t xu_ipaddr_unix(xu_ipaddr_ref_t ipaddr);

/*! set the path from unix
 *
 * @param ipaddr    the address
 * @param unixaddr  the unixaddr, clear it if be null
 */
xu_void_t xu_ipaddr_unix_set(xu_ipaddr_ref_t ipaddr, xu_unixaddr_ref_t unixaddr);

/*! set the unix socket address from string
 *
 * @param ipaddr    the address
 * @param cstr      the address string
 * @param is_abstract true if the address is abstract address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_ipaddr_unix_set_cstr(xu_ipaddr_ref_t ipaddr, xu_char_t const* cstr, xu_bool_t is_abstract);

/*! get the address family
 *
 * @param ipaddr    the address
 *
 * @return          the family
 */
xu_size_t xu_ipaddr_family(xu_ipaddr_ref_t ipaddr);

/*! set the address family
 *
 * @param ipaddr    the address
 * @param family    the family
 */
xu_void_t xu_ipaddr_family_set(xu_ipaddr_ref_t ipaddr, xu_size_t family);

/*! get the address port
 *
 * @param ipaddr    the address
 *
 * @return          the port
 */
xu_uint16_t xu_ipaddr_port(xu_ipaddr_ref_t ipaddr);

/*! set the address family
 *
 * @param ipaddr    the address
 * @param port      the port
 */
xu_void_t xu_ipaddr_port_set(xu_ipaddr_ref_t ipaddr, xu_uint16_t port);
