#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_void_t xu_demo_addrinfo_test(xu_char_t const* name)
{
    // get the default address (ipv4)
    xu_ipaddr_t addr = {0};
    xu_char_t   host[256];
    if (xu_addrinfo_addr(name, &addr))
        xu_trace_i("%s: %{ipaddr} => %s", name, &addr, xu_addrinfo_name(&addr, host, sizeof(host)));

    // get the ipv6 address by the hint info
    xu_ipaddr_clear(&addr);
    xu_ipaddr_family_set(&addr, XU_IPADDR_FAMILY_IPV6);
    if (xu_addrinfo_addr(name, &addr))
        xu_trace_i("%s: %{ipaddr} => %s", name, &addr, xu_addrinfo_name(&addr, host, sizeof(host)));
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_addrinfo_main(xu_int_t argc, xu_char_t** argv)
{
    // test address
    xu_demo_addrinfo_test(argv[1]);
    return 0;
}
