#include "xutil/platform/ifaddrs.h"
#include "xutil/container/element.h"
#include "xutil/container/list.h"
#include "xutil/macros/malloc.h"

#if defined(XU_CONFIG_POSIX_HAVE_GETIFADDRS) && !defined(XU_CONFIG_OS_WEB) &&                                          \
    !defined(XU_CONFIG_LINUX_HAVE_IFADDRS) && !defined(XU_CONFIG_OS_ANDROID)

#    include <ifaddrs.h>
#    ifdef XU_CONFIG_OS_LINUX
#        include <linux/if.h>
#    else
#        include <net/if.h>
#    endif
#    include "sockaddr.h"
#    include <net/if_dl.h>
#    include <netinet/in.h>

/* *******************************************************
 * private implementation
 */
static xu_void_t xu_ifaddrs_interface_exit(xu_element_ref_t element, xu_pointer_t buff)
{
    // check
    xu_ifaddrs_interface_ref_t interface = (xu_ifaddrs_interface_ref_t)buff;
    if (interface)
    {
        // exit the interface name
        if (interface->name) xu_free(interface->name);
        interface->name = xu_null;
    }
}

/* *******************************************************
 * implementation
 */
xu_ifaddrs_ref_t xu_ifaddrs_init()
{
    // init it
    return (xu_ifaddrs_ref_t)xu_list_init(
        8, xu_element_mem(sizeof(xu_ifaddrs_interface_t), xu_ifaddrs_interface_exit, xu_null));
}
xu_void_t xu_ifaddrs_exit(xu_ifaddrs_ref_t ifaddrs)
{
    // exit it
    if (ifaddrs) xu_list_exit((xu_list_ref_t)ifaddrs);
}
xu_iterator_ref_t xu_ifaddrs_itor(xu_ifaddrs_ref_t ifaddrs, xu_bool_t reload)
{
    // check
    xu_list_ref_t interfaces = (xu_list_ref_t)ifaddrs;
    xu_assert_and_check_return_val(interfaces, xu_null);

    // uses the cached interfaces?
    xu_check_return_val(reload, (xu_iterator_ref_t)interfaces);

    // clear interfaces first
    xu_list_clear(interfaces);

    // query the list of interfaces.
    struct ifaddrs* list = xu_null;
    if (!getifaddrs(&list) && list)
    {
        // done
        struct ifaddrs* item = xu_null;
        for (item = list; item; item = item->ifa_next)
        {
            // check
            xu_check_continue(item->ifa_addr && item->ifa_name);

            /* attempt to get the interface from the cached interfaces
             * and make a new interface if no the cached interface
             */
            xu_ifaddrs_interface_t     interface_new = {0};
            xu_ifaddrs_interface_ref_t interface =
                xu_ifaddrs_interface_find((xu_iterator_ref_t)interfaces, item->ifa_name);
            if (!interface) interface = &interface_new;

            // check
            xu_assert(interface == &interface_new || interface->name);

            // done
            switch (item->ifa_addr->sa_family)
            {
            case AF_INET:
            {
                // the address
                struct sockaddr_storage const* addr = (struct sockaddr_storage const*)item->ifa_addr;

                // save ipaddr4
                xu_ipaddr_t ipaddr4;
                if (!xu_sockaddr_save(&ipaddr4, addr)) break;
                interface->ipaddr4 = ipaddr4.u.ipv4;

                // save flags
                interface->flags |= XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR4;
                if ((item->ifa_flags & IFF_LOOPBACK) || xu_ipaddr_ip_is_loopback(&ipaddr4))
                    interface->flags |= XU_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK;

                // new interface? save it
                if (interface == &interface_new)
                {
                    // save interface name
                    interface->name = xu_strdup(item->ifa_name);
                    xu_assert(interface->name);

                    // save interface
                    xu_list_insert_tail(interfaces, interface);
                }
            }
            break;
            case AF_INET6:
            {
                // the address
                struct sockaddr_storage const* addr = (struct sockaddr_storage const*)item->ifa_addr;

                // save ipaddr6
                xu_ipaddr_t ipaddr6;
                if (!xu_sockaddr_save(&ipaddr6, addr)) break;
                interface->ipaddr6 = ipaddr6.u.ipv6;

                // save flags
                interface->flags |= XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR6;
                if ((item->ifa_flags & IFF_LOOPBACK) || xu_ipaddr_ip_is_loopback(&ipaddr6))
                    interface->flags |= XU_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK;

                // new interface? save it
                if (interface == &interface_new)
                {
                    // save interface name
                    interface->name = xu_strdup(item->ifa_name);
                    xu_assert(interface->name);

                    // save interface
                    xu_list_insert_tail(interfaces, interface);
                }
            }
            case AF_LINK:
            {
                // the address data
                struct sockaddr_dl const* addr = (struct sockaddr_dl const*)item->ifa_addr;
                xu_byte_t const*          base = (xu_byte_t const*)(addr->sdl_data + addr->sdl_nlen);

                // check
                xu_check_break(addr->sdl_alen == sizeof(interface->hwaddr.u8));

                // save flags
                interface->flags |= XU_IFADDRS_INTERFACE_FLAG_HAVE_HWADDR;
                if (item->ifa_flags & IFF_LOOPBACK) interface->flags |= XU_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK;

                // save hwaddr
                xu_memcpy(interface->hwaddr.u8, base, sizeof(interface->hwaddr.u8));

                // new interface? save it
                if (interface == &interface_new)
                {
                    // save interface name
                    interface->name = xu_strdup(item->ifa_name);
                    xu_assert(interface->name);

                    // save interface
                    xu_list_insert_tail(interfaces, interface);
                }
            }
            break;
            default:
            {
                // trace
                xu_trace_d("unknown family: %d", item->ifa_addr->sa_family);
            }
            break;
            }
        }

        // exit the interface list
        freeifaddrs(list);
    }

    // ok?
    return (xu_iterator_ref_t)interfaces;
}

#endif
