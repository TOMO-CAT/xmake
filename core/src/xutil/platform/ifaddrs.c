/* *******************************************************
 * trace
 */

#define XU_TRACE_MODULE_NAME "ifaddrs"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/platform/ifaddrs.h"
#include "xutil/algorithm/find_if.h"
#include "xutil/algorithm/for_if.h"
#include "xutil/libc/string/string.h"
#include "xutil/utils/singleton.h"

/* *******************************************************
 * instance implementation
 */

static xu_handle_t xu_ifaddrs_instance_init(xu_cpointer_t* ppriv)
{
    // init it
    return (xu_handle_t)xu_ifaddrs_init();
}
static xu_void_t xu_ifaddrs_instance_exit(xu_handle_t ifaddrs, xu_cpointer_t priv)
{
    // exit it
    xu_ifaddrs_exit((xu_ifaddrs_ref_t)ifaddrs);
}
static xu_bool_t xu_ifaddrs_interface_pred(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t name)
{
    // check
    xu_assert(item);

    // is equal?
    return !xu_stricmp(((xu_ifaddrs_interface_ref_t)item)->name, (xu_char_t const*)name);
}
static xu_ifaddrs_interface_ref_t xu_ifaddrs_interface_find(xu_iterator_ref_t iterator, xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(iterator && name, xu_null);

    // find it
    xu_size_t itor = xu_find_all_if(iterator, xu_ifaddrs_interface_pred, name);
    xu_check_return_val(itor != xu_iterator_tail(iterator), xu_null);

    // ok
    return (xu_ifaddrs_interface_ref_t)xu_iterator_item(iterator, itor);
}

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_POSIX_HAVE_GETIFADDRS) && !defined(XU_CONFIG_OS_WEB)
#    if defined(XU_CONFIG_LINUX_HAVE_IFADDRS) || defined(XU_CONFIG_OS_ANDROID)
#        include "xutil/platform/linux/ifaddrs.c"
#    else
#        include "xutil/platform/posix/ifaddrs.c"
#    endif
#elif defined(XU_CONFIG_LINUX_HAVE_IFADDRS) || defined(XU_CONFIG_OS_ANDROID)
#    include "xutil/platform/linux/ifaddrs2.c"
#else
xu_ifaddrs_ref_t xu_ifaddrs_init()
{
    xu_trace_noimpl();
    return xu_null;
}
xu_void_t         xu_ifaddrs_exit(xu_ifaddrs_ref_t ifaddrs) { xu_trace_noimpl(); }
xu_iterator_ref_t xu_ifaddrs_itor(xu_ifaddrs_ref_t ifaddrs, xu_bool_t reload)
{
    xu_trace_noimpl();
    return xu_null;
}
#endif
xu_ifaddrs_ref_t xu_ifaddrs()
{
    return (xu_ifaddrs_ref_t)xu_singleton_instance(XU_SINGLETON_TYPE_IFADDRS, xu_ifaddrs_instance_init,
                                                   xu_ifaddrs_instance_exit, xu_null, xu_null);
}
xu_ifaddrs_interface_ref_t xu_ifaddrs_interface(xu_ifaddrs_ref_t ifaddrs, xu_char_t const* name, xu_bool_t reload)
{
    // check
    xu_assert_and_check_return_val(ifaddrs && name, xu_null);

    // the iterator
    xu_iterator_ref_t iterator = xu_ifaddrs_itor(ifaddrs, reload);
    xu_assert_and_check_return_val(iterator, xu_null);

    // reload it if the cached interfaces is empty
    if (!reload && !xu_iterator_size(iterator)) iterator = xu_ifaddrs_itor(ifaddrs, xu_true);

    // ok
    return xu_ifaddrs_interface_find(iterator, name);
}
xu_bool_t xu_ifaddrs_hwaddr(xu_ifaddrs_ref_t ifaddrs, xu_char_t const* name, xu_bool_t reload, xu_hwaddr_ref_t hwaddr)
{
    // check
    xu_assert_and_check_return_val(ifaddrs && hwaddr, xu_false);

    // clear it first
    xu_hwaddr_clear(hwaddr);

    // the iterator
    xu_iterator_ref_t iterator = xu_ifaddrs_itor(ifaddrs, reload);
    xu_assert_and_check_return_val(iterator, xu_false);

    // reload it if the cached interfaces is empty
    if (!reload && !xu_iterator_size(iterator)) iterator = xu_ifaddrs_itor(ifaddrs, xu_true);

    // done
    xu_bool_t ok = xu_false;
    xu_for_all_if(xu_ifaddrs_interface_ref_t, iface, iterator, iface)
    {
        // get hwaddr from the given iface name?
        if (name)
        {
            // is this?
            if ((iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_HWADDR) &&
                (iface->name && !xu_strcmp(iface->name, name)))
            {
                // save hwaddr
                xu_hwaddr_copy(hwaddr, &iface->hwaddr);

                // ok
                ok = xu_true;
                break;
            }
        }
        else
        {
            // is this?
            if ((iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_HWADDR) &&
                (iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR) &&
                !(iface->flags & XU_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK))
            {
                // save hwaddr
                xu_hwaddr_copy(hwaddr, &iface->hwaddr);

                // ok
                ok = xu_true;
                break;
            }
        }
    }

    // ok?
    return ok;
}
xu_bool_t xu_ifaddrs_ipaddr(xu_ifaddrs_ref_t ifaddrs, xu_char_t const* name, xu_bool_t reload, xu_size_t family,
                            xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ifaddrs && ipaddr, xu_false);

    // clear it first
    xu_ipaddr_clear(ipaddr);

    // the iterator
    xu_iterator_ref_t iterator = xu_ifaddrs_itor(ifaddrs, reload);
    xu_assert_and_check_return_val(iterator, xu_false);

    // reload it if the cached interfaces is empty
    if (!reload && !xu_iterator_size(iterator)) iterator = xu_ifaddrs_itor(ifaddrs, xu_true);

    xu_bool_t ok = xu_false;
    xu_for_all_if(xu_ifaddrs_interface_ref_t, iface, iterator, iface)
    {
        // is this?
        if ((name || !(iface->flags & XU_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK)) &&
            (iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR) &&
            (!name || (iface->name && !xu_strcmp(iface->name, name))))
        {
            // ipv4?
            if (iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR4 && (!family || family == XU_IPADDR_FAMILY_IPV4))
            {
                // save ipaddr4
                xu_ipaddr_ipv4_set(ipaddr, &iface->ipaddr4);

                // ok
                ok = xu_true;
                break;
            }
            // ipv6?
            else if (iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR6 &&
                     (!family || family == XU_IPADDR_FAMILY_IPV6))
            {
                // save ipaddr6
                xu_ipaddr_ipv6_set(ipaddr, &iface->ipaddr6);

                // ok
                ok = xu_true;
                break;
            }
        }
    }
    return ok;
}
#ifdef __xu_debug__
xu_void_t xu_ifaddrs_dump(xu_ifaddrs_ref_t ifaddrs)
{
    // trace
    xu_trace_i("");

    // done
    xu_for_all_if(xu_ifaddrs_interface_ref_t, iface, xu_ifaddrs_itor(ifaddrs, xu_true), iface)
    {
        // trace
        xu_trace_i("name: %s%s", iface->name,
                   (iface->flags & XU_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK) ? "[loopback]" : "");
        if (iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR4) xu_trace_i("    ipaddr4: %{ipv4}", &iface->ipaddr4);
        if (iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR6) xu_trace_i("    ipaddr6: %{ipv6}", &iface->ipaddr6);
        if (iface->flags & XU_IFADDRS_INTERFACE_FLAG_HAVE_HWADDR) xu_trace_i("    hwaddr: %{hwaddr}", &iface->hwaddr);
    }
}
#endif
