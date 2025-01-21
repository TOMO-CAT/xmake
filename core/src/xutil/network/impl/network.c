#include "xutil/network/impl/network.h"
#include "xutil/libc/stdio/printf_object.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/network/dns/cache.h"
#include "xutil/network/dns/server.h"
#include "xutil/network/hwaddr.h"
#include "xutil/network/ipaddr.h"
#include "xutil/network/ipv4.h"
#include "xutil/network/ipv6.h"
#include "xutil/network/unixaddr.h"

#ifndef XU_CONFIG_MICRO_ENABLE
static xu_long_t xu_network_printf_format_ipv4(xu_cpointer_t object, xu_char_t* cstr, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(object && cstr && maxn, -1);

    // the ipv4
    xu_ipv4_ref_t ipv4 = (xu_ipv4_ref_t)object;

    // make it
    cstr = (xu_char_t*)xu_ipv4_cstr(ipv4, cstr, maxn);

    // ok?
    return cstr ? xu_strlen(cstr) : -1;
}
static xu_long_t xu_network_printf_format_ipv6(xu_cpointer_t object, xu_char_t* cstr, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(object && cstr && maxn, -1);

    // the ipv6
    xu_ipv6_ref_t ipv6 = (xu_ipv6_ref_t)object;

    // make it
    cstr = (xu_char_t*)xu_ipv6_cstr(ipv6, cstr, maxn);

    // ok?
    return cstr ? xu_strlen(cstr) : -1;
}
static xu_long_t xu_network_printf_format_unixaddr(xu_cpointer_t object, xu_char_t* cstr, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(object && cstr && maxn, -1);

    // the unixaddr
    xu_unixaddr_ref_t unixaddr = (xu_unixaddr_ref_t)object;

    // make it
    cstr = (xu_char_t*)xu_unixaddr_cstr(unixaddr, cstr, maxn);

    // ok?
    return cstr ? xu_strlen(cstr) : -1;
}
static xu_long_t xu_network_printf_format_ipaddr(xu_cpointer_t object, xu_char_t* cstr, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(object && cstr && maxn, -1);

    // the ipaddr
    xu_ipaddr_ref_t ipaddr = (xu_ipaddr_ref_t)object;

    // make it
    cstr = (xu_char_t*)xu_ipaddr_cstr(ipaddr, cstr, maxn);

    // ok?
    return cstr ? xu_strlen(cstr) : -1;
}
static xu_long_t xu_network_printf_format_hwaddr(xu_cpointer_t object, xu_char_t* cstr, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(object && cstr && maxn, -1);

    // the hwaddr
    xu_hwaddr_ref_t hwaddr = (xu_hwaddr_ref_t)object;

    // make it
    cstr = (xu_char_t*)xu_hwaddr_cstr(hwaddr, cstr, maxn);

    // ok?
    return cstr ? xu_strlen(cstr) : -1;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
xu_bool_t xu_network_init_env()
{
#ifndef XU_CONFIG_MICRO_ENABLE
    // init dns server
    if (!xu_dns_server_init()) return xu_false;

    // init dns cache
    if (!xu_dns_cache_init()) return xu_false;

    // register printf("%{ipv4}", &ipv4);
    xu_printf_object_register("ipv4", xu_network_printf_format_ipv4);

    // register printf("%{ipv6}", &ipv6);
    xu_printf_object_register("ipv6", xu_network_printf_format_ipv6);

    // register printf("%{unixaddr}", &unixaddr);
    xu_printf_object_register("unixaddr", xu_network_printf_format_unixaddr);

    // register printf("%{ipaddr}", &ipaddr);
    xu_printf_object_register("ipaddr", xu_network_printf_format_ipaddr);

    // register printf("%{hwaddr}", &hwaddr);
    xu_printf_object_register("hwaddr", xu_network_printf_format_hwaddr);
#endif

    // ok
    return xu_true;
}
xu_void_t xu_network_exit_env()
{
#ifndef XU_CONFIG_MICRO_ENABLE
    // exit dns cache
    xu_dns_cache_exit();

    // exit dns server
    xu_dns_server_exit();
#endif
}
