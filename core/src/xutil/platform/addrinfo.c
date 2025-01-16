/* *******************************************************
 * trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "addrinfo"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/platform/addrinfo.h"
#include "xutil/network/network.h"

#if defined(XU_CONFIG_POSIX_HAVE_GETADDRINFO) || defined(XU_CONFIG_POSIX_HAVE_GETHOSTBYNAME)
#    define XU_ADDRINFO_ADDR_IMPL
#    include "xutil/platform/posix/addrinfo.c"
#    undef XU_ADDRINFO_ADDR_IMPL
#else
xu_bool_t xu_addrinfo_addr(xu_char_t const* name, xu_ipaddr_ref_t addr)
{
#    ifndef XU_CONFIG_MICRO_ENABLE
    // attempt to get address using dns looker
    if (xu_ipaddr_family(addr) != XU_IPADDR_FAMILY_IPV6 && xu_dns_looker_done(name, addr)) return xu_true;
#    endif

    // not implemented
    xu_trace_noimpl();
    return xu_false;
}
#endif

#if defined(XU_CONFIG_POSIX_HAVE_GETNAMEINFO) || defined(XU_CONFIG_POSIX_HAVE_GETHOSTBYADDR)
#    define XU_ADDRINFO_NAME_IMPL
#    include "xutil/platform/posix/addrinfo.c"
#    undef XU_ADDRINFO_NAME_IMPL
#else
xu_char_t const* xu_addrinfo_name(xu_ipaddr_ref_t addr, xu_char_t* name, xu_size_t maxn)
{
    xu_trace_noimpl();
    return xu_null;
}
#endif
