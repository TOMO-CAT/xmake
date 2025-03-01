#pragma once

#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/network/ipaddr.h"
#include "xutil/prefix.h"
#include "xutil/utils/bits.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

/* *******************************************************
 * types
 */

typedef struct sockaddr_un xu_sockaddr_un_t, *xu_sockaddr_un_ref_t;

/* *******************************************************
 * interfaces
 */

/* save the socket address to the ip address
 *
 * @param ipaddr                the ip address
 * @param saddr                 the socket address
 *
 * @return                      the socket address size
 */
static __xu_inline__ xu_size_t xu_sockaddr_save(xu_ipaddr_ref_t ipaddr, struct sockaddr_storage const* saddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr && saddr, 0);

    // clear address
    xu_ipaddr_clear(ipaddr);

    // done
    xu_size_t size = 0;
    switch (saddr->ss_family)
    {
    case AF_INET:
    {
        // the ipv4 ipaddr
        struct sockaddr_in* addr4 = (struct sockaddr_in*)saddr;

        // save family
        xu_ipaddr_family_set(ipaddr, XU_IPADDR_FAMILY_IPV4);

        // make ipv4
        xu_ipv4_t ipv4;
        ipv4.u32 = (xu_uint32_t)addr4->sin_addr.s_addr;

        // save ipv4
        xu_ipaddr_ipv4_set(ipaddr, &ipv4);

        // save port
        xu_ipaddr_port_set(ipaddr, xu_bits_be_to_ne_u16(addr4->sin_port));

        // save size
        size = sizeof(struct sockaddr_in);
    }
    break;
#ifndef __COSMOPOLITAN__
    case AF_INET6:
    {
        // the ipv6 ipaddr
        struct sockaddr_in6* addr6 = (struct sockaddr_in6*)saddr;

        // check
        xu_assert_static(sizeof(ipaddr->u.ipv6.addr.u8) == sizeof(addr6->sin6_addr.s6_addr));
        xu_assert_static(xu_arrayn(ipaddr->u.ipv6.addr.u8) == xu_arrayn(addr6->sin6_addr.s6_addr));

        // save family
        xu_ipaddr_family_set(ipaddr, XU_IPADDR_FAMILY_IPV6);

        // save port
        xu_ipaddr_port_set(ipaddr, xu_bits_be_to_ne_u16(addr6->sin6_port));

        // make ipv6
        xu_ipv6_t ipv6;
        xu_memcpy(ipv6.addr.u8, addr6->sin6_addr.s6_addr, sizeof(ipv6.addr.u8));

        // save scope id
        ipv6.scope_id = 0;
        if (IN6_IS_ADDR_LINKLOCAL(&addr6->sin6_addr) || IN6_IS_ADDR_MC_LINKLOCAL(&addr6->sin6_addr))
            ipv6.scope_id = addr6->sin6_scope_id;

        // save ipv6
        xu_ipaddr_ipv6_set(ipaddr, &ipv6);

        // save size
        size = sizeof(struct sockaddr_in6);
    }
    break;
#endif
    case AF_UNIX:
    {
        xu_sockaddr_un_ref_t addru = (xu_sockaddr_un_ref_t)saddr;

        // save family
        xu_ipaddr_family_set(ipaddr, XU_IPADDR_FAMILY_UNIX);

        // make unixaddr
        xu_unixaddr_t unixaddr;
        if (addru->sun_path[0])
        {
            // normal unixaddr
            xu_size_t n = xu_strlcpy(unixaddr.path, addru->sun_path, sizeof(addru->sun_path));
            xu_assert_and_check_return_val(n < sizeof(addru->sun_path), 0);
        }
        else
        {
            // abstract unixaddr
            xu_size_t n = xu_strlcpy(unixaddr.path, addru->sun_path + 1, sizeof(addru->sun_path) - 1);
            xu_assert_and_check_return_val(n < sizeof(addru->sun_path) - 1, 0);
            unixaddr.is_abstract = xu_true;
        }

        // save unixaddr
        xu_ipaddr_unix_set(ipaddr, &unixaddr);

        // save size
        size = sizeof(xu_sockaddr_un_t);
    }
    default: xu_assert(0); break;
    }

    // ok?
    return size;
}

/* load the ip address to socket address
 *
 * @param saddr                 the socket address
 * @param ipaddr                the ip address
 *
 * @return                      the socket address size
 */
static __xu_inline__ xu_size_t xu_sockaddr_load(struct sockaddr_storage* saddr, xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(saddr && ipaddr, 0);

    // clear address
    xu_memset(saddr, 0, sizeof(struct sockaddr_storage));

    // done
    xu_size_t size = 0;
    switch (xu_ipaddr_family(ipaddr))
    {
    case XU_IPADDR_FAMILY_IPV4:
    {
        // the ipv4 ipaddr
        struct sockaddr_in* addr4 = (struct sockaddr_in*)saddr;

        // save family
        addr4->sin_family = AF_INET;

        // save ipv4
        addr4->sin_addr.s_addr = xu_ipaddr_ip_is_any(ipaddr) ? INADDR_ANY : ipaddr->u.ipv4.u32;

        // save port
        addr4->sin_port = xu_bits_ne_to_be_u16(xu_ipaddr_port(ipaddr));

        // save size
        size = sizeof(struct sockaddr_in);
    }
    break;
    case XU_IPADDR_FAMILY_IPV6:
    {
        // the ipv6 ipaddr
        struct sockaddr_in6* addr6 = (struct sockaddr_in6*)saddr;

        // check
        xu_assert_static(sizeof(ipaddr->u.ipv6.addr.u8) == sizeof(addr6->sin6_addr.s6_addr));
        xu_assert_static(xu_arrayn(ipaddr->u.ipv6.addr.u8) == xu_arrayn(addr6->sin6_addr.s6_addr));

        // save family
        addr6->sin6_family = AF_INET6;

        // save ipv6
        if (xu_ipaddr_ip_is_any(ipaddr))
            addr6->sin6_addr = in6addr_any;
        else
            xu_memcpy(addr6->sin6_addr.s6_addr, ipaddr->u.ipv6.addr.u8, sizeof(addr6->sin6_addr.s6_addr));

        // save port
        addr6->sin6_port = xu_bits_ne_to_be_u16(xu_ipaddr_port(ipaddr));

        // save scope id
        if (xu_ipv6_is_linklocal(&ipaddr->u.ipv6) || xu_ipv6_is_mc_linklocal(&ipaddr->u.ipv6))
            addr6->sin6_scope_id = ipaddr->u.ipv6.scope_id;

        // save size
        size = sizeof(struct sockaddr_in6);
    }
    break;
    case XU_IPADDR_FAMILY_UNIX:
    {
        // the unix ipaddr
        xu_sockaddr_un_ref_t addru = (xu_sockaddr_un_ref_t)saddr;

        // save family
        addru->sun_family = AF_UNIX;

        // save unix
        if (!ipaddr->u.unixaddr.is_abstract)
        {
            // normal unixaddr
            xu_size_t n = xu_strlcpy(addru->sun_path, ipaddr->u.unixaddr.path, sizeof(addru->sun_path));
            xu_assert_and_check_return_val(n < sizeof(addru->sun_path), 0);
        }
        else
        {
            // abstract unixaddr
            addru->sun_path[0] = '\0';
            xu_size_t n        = xu_strlcpy(addru->sun_path + 1, ipaddr->u.unixaddr.path, sizeof(addru->sun_path));
            xu_assert_and_check_return_val(n < sizeof(addru->sun_path) - 1, 0);
        }

        // save size
        size = sizeof(xu_sockaddr_un_t);
    }
    break;
    default: xu_assert(0); break;
    }

    // ok?
    return size;
}
