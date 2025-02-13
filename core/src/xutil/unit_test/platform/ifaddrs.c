#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_ifaddrs_main(xu_int_t argc, xu_char_t** argv)
{
#ifdef __xu_debug__
    // dump ifaddrs
    xu_ifaddrs_dump(xu_ifaddrs());
#endif

    // dump hwaddr
    xu_hwaddr_t hwaddr;
    if (xu_ifaddrs_hwaddr(xu_ifaddrs(), argv[1], xu_false, &hwaddr))
    {
        // trace
        xu_trace_i("name: %s, hwaddr: %{hwaddr}", argv[1], &hwaddr);
    }

    // dump ipaddr4
    xu_ipaddr_t ipaddr4;
    if (xu_ifaddrs_ipaddr(xu_ifaddrs(), argv[1], xu_false, XU_IPADDR_FAMILY_IPV4, &ipaddr4))
    {
        // trace
        xu_trace_i("name: %s, ipaddr4: %{ipaddr}", argv[1], &ipaddr4);
    }

    // dump ipaddr6
    xu_ipaddr_t ipaddr6;
    if (xu_ifaddrs_ipaddr(xu_ifaddrs(), argv[1], xu_false, XU_IPADDR_FAMILY_IPV6, &ipaddr6))
    {
        // trace
        xu_trace_i("name: %s, ipaddr6: %{ipaddr}", argv[1], &ipaddr6);
    }

    return 0;
}
