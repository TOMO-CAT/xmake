#pragma once

#include "xutil/prefix.h"

#include "xutil/network/ipaddr.h"

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! get the first dns address from the host name
 *
 * @code

    // get the default address (ipv4)
    xu_ipaddr_t addr = {0};
    if (xu_addrinfo_addr("www.xutil.org", &addr))
        xu_trace_i("%{ipaddr}", &addr);

    // get the ipv6 address by the hint info
    xu_ipaddr_t addr = {0};
    xu_ipaddr_family_set(&addr, XU_IPADDR_FAMILY_IPV6);
    if (xu_addrinfo_addr("www.xutil.org", &addr))
        xu_trace_i("%{ipaddr}", &addr);

 * @endcode
 *
 * @param name      the host name (cannot be null)
 * @param addr      the ip address (we can fill some hint info first)
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_addrinfo_addr(xu_char_t const* name, xu_ipaddr_ref_t addr);

/*! get the host name from the given address
 *
 * @code

    // get the host name by address
    xu_ipaddr_t addr;
    xu_char_t   host[256];
    xu_ipaddr_ip_cstr_set(&addr, "127.0.0.1");
    xu_trace_i("%s", xu_addrinfo_name(&addr, host, sizeof(host)));

 * @endcode
 *
 * @param addr      the ip address (cannot be null)
 * @param name      the host name buffer
 * @param maxn      the host name buffer maxn
 *
 * @return          the host name or xu_null
 */
xu_char_t const* xu_addrinfo_name(xu_ipaddr_ref_t addr, xu_char_t* name, xu_size_t maxn);
