#include "xutil/prefix.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "xutil/libc/stdio/stdio.h"
#include "xutil/network/dns/looker.h"
#include "xutil/platform/addrinfo.h"
#include "xutil/platform/posix/sockaddr.h"

/* *******************************************************
 * private implementation
 */
#if defined(XU_ADDRINFO_ADDR_IMPL) && defined(XU_CONFIG_POSIX_HAVE_GETADDRINFO)
static __xu_inline__ xu_int_t xu_addrinfo_ai_family(xu_ipaddr_ref_t addr)
{
    // get the ai family for getaddrinfo
    switch (xu_ipaddr_family(addr))
    {
    case XU_IPADDR_FAMILY_IPV4: return AF_INET;
    case XU_IPADDR_FAMILY_IPV6: return AF_INET6;
    default: return AF_UNSPEC;
    }
}
#endif

/* *******************************************************
 * implementation
 */
#ifdef XU_ADDRINFO_ADDR_IMPL
xu_bool_t xu_addrinfo_addr(xu_char_t const* name, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(name && addr, xu_false);

#    ifndef XU_CONFIG_MICRO_ENABLE
    // attempt to get address using dns looker
    if (xu_ipaddr_family(addr) != XU_IPADDR_FAMILY_IPV6 && xu_dns_looker_done(name, addr)) return xu_true;
#    endif

#    if defined(XU_CONFIG_POSIX_HAVE_GETADDRINFO)
    // done
    xu_bool_t        ok     = xu_false;
    struct addrinfo* answer = xu_null;
    do
    {
        // init hints
        struct addrinfo hints = {0};
        hints.ai_family       = xu_addrinfo_ai_family(addr);
        hints.ai_socktype     = SOCK_STREAM;

        // init service
        xu_char_t   service[32] = {0};
        xu_uint16_t port        = xu_ipaddr_port(addr);
        if (port) xu_snprintf(service, sizeof(service), "%u", port);

        // get address info
        if (getaddrinfo(name, port ? service : xu_null, &hints, &answer)) break;
        xu_assert_and_check_break(answer && answer->ai_addr);

        // save address
        ok = xu_sockaddr_save(addr, (struct sockaddr_storage const*)answer->ai_addr) != 0;

    } while (0);

    // exit answer
    if (answer) freeaddrinfo(answer);
    answer = xu_null;

    // ok?
    return ok;

#    elif defined(XU_CONFIG_POSIX_HAVE_GETHOSTBYNAME)

    // not support ipv6
    xu_assert_and_check_return_val(xu_ipaddr_family(addr) != XU_IPADDR_FAMILY_IPV6, xu_false);

    // get first host address
    struct hostent* hostaddr = gethostbyname(name);
    xu_check_return_val(hostaddr && hostaddr->h_addr && hostaddr->h_addrtype == AF_INET, xu_false);

    // save family
    xu_ipaddr_family_set(addr, XU_IPADDR_FAMILY_IPV4);

    // make ipv4
    xu_ipv4_t ipv4;
    ipv4.u32 = (xu_uint32_t)((struct in_addr const*)hostaddr->h_addr)->s_addr;

    // save ipv4
    xu_ipaddr_ipv4_set(addr, &ipv4);

    // ok
    return xu_true;
#    else
    xu_trace_noimpl();
    return xu_false;
#    endif
}
#endif

#ifdef XU_ADDRINFO_NAME_IMPL
xu_char_t const* xu_addrinfo_name(xu_ipaddr_ref_t addr, xu_char_t* name, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(addr && name && maxn, xu_null);

#    if defined(XU_CONFIG_POSIX_HAVE_GETNAMEINFO)
    // load socket address
    struct sockaddr_storage saddr;
    socklen_t               saddrlen = (socklen_t)xu_sockaddr_load(&saddr, addr);
    xu_assert_and_check_return_val(saddrlen, xu_null);

    // get host name from address
    return !getnameinfo((struct sockaddr const*)&saddr, saddrlen, name, maxn, xu_null, 0, NI_NAMEREQD) ? name : xu_null;
#    elif defined(XU_CONFIG_POSIX_HAVE_GETHOSTBYNAME)

    // done
    struct hostent* hostaddr = xu_null;
    switch (xu_ipaddr_family(addr))
    {
    case XU_IPADDR_FAMILY_IPV4:
    {
        // init ip address
        struct in_addr ipaddr = {0};
        ipaddr.s_addr         = xu_ipaddr_ip_is_any(addr) ? INADDR_ANY : addr->u.ipv4.u32;

        // get host name from address
        hostaddr = gethostbyaddr((xu_char_t const*)&ipaddr, sizeof(ipaddr), AF_INET);
    }
    break;
    case XU_IPADDR_FAMILY_IPV6:
    {
        // init ip address
        struct in6_addr ipaddr;
        xu_memset(&ipaddr, 0, sizeof(ipaddr));

        // save ipv6
        if (xu_ipaddr_ip_is_any(addr))
            ipaddr = in6addr_any;
        else
            xu_memcpy(ipaddr.s6_addr, addr->u.ipv6.addr.u8, sizeof(ipaddr.s6_addr));

        // get host name from address
        hostaddr = gethostbyaddr((xu_char_t const*)&ipaddr, sizeof(ipaddr), AF_INET6);
    }
    break;
    default: break;
    }
    xu_check_return_val(hostaddr && hostaddr->h_name, xu_null);

    // save name
    xu_strlcpy(name, hostaddr->h_name, maxn);

    // ok?
    return name;
#    else
    xu_trace_noimpl();
    return xu_null;
#    endif
}
#endif
