#pragma once

#include "xutil/container/iterator.h"
#include "xutil/network/hwaddr.h"
#include "xutil/network/ipaddr.h"
#include "xutil/network/ipv4.h"
#include "xutil/network/ipv6.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  types
 */

// the ifaddrs interface flag enum
typedef enum __xu_ifaddrs_interface_flag_e
{
    XU_IFADDRS_INTERFACE_FLAG_NONE         = 0,
    XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR4 = 1,
    XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR6 = 2,
    XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR =
        XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR4 | XU_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR6,
    XU_IFADDRS_INTERFACE_FLAG_HAVE_HWADDR = 4,
    XU_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK = 8

} xu_ifaddrs_interface_flag_e;

// the ifaddrs interface type
typedef struct __xu_ifaddrs_interface_t
{
    /// the interface name
    xu_char_t const* name;

    /// the interface flags
    xu_uint32_t flags;

    // the hardware address
    xu_hwaddr_t hwaddr;

    // the ipv4 address
    xu_ipv4_t ipaddr4;

    // the ipv6 address
    xu_ipv6_t ipaddr6;

} xu_ifaddrs_interface_t, *xu_ifaddrs_interface_ref_t;

// the ifaddrs type
typedef __xu_typeref__(ifaddrs);

/* *******************************************************
 *  interfaces
 */

/*! the ifaddrs instance
 *
 * @return                  the ifaddrs
 */
xu_ifaddrs_ref_t xu_ifaddrs(xu_noarg_t);

/*! init ifaddrs
 *
 * @return                  the ifaddrs
 */
xu_ifaddrs_ref_t xu_ifaddrs_init(xu_noarg_t);

/*! exit ifaddrs
 *
 * @param ifaddrs           the ifaddrs
 */
xu_void_t xu_ifaddrs_exit(xu_ifaddrs_ref_t ifaddrs);

/*! the ifaddrs interface iterator
 *
 * @code
 * xu_for_all_if (xu_ifaddrs_interface_ref_t, interface, xu_ifaddrs_itor(ifaddrs, xu_false), interface)
 * {
 *     // ...
 * }
 * @endcode
 *
 * @param ifaddrs           the ifaddrs
 * @param reload            force to reload the ifaddrs list, will cache list if be false
 *
 * @return                  the interface iterator
 */
xu_iterator_ref_t xu_ifaddrs_itor(xu_ifaddrs_ref_t ifaddrs, xu_bool_t reload);

/*! get the interface from the given interface name
 *
 * @param ifaddrs           the ifaddrs
 * @param name              the interface name
 * @param reload            force to reload the ifaddrs list, will cache list if be false
 *
 * @return                  xu_true or xu_false
 */
xu_ifaddrs_interface_ref_t xu_ifaddrs_interface(xu_ifaddrs_ref_t ifaddrs, xu_char_t const* name, xu_bool_t reload);

/*! the hardware address from the given interface name
 *
 * @param ifaddrs           the ifaddrs
 * @param name              the interface name, get the first ether address if be null
 * @param reload            force to reload the ifaddrs list, will cache list if be false
 * @param hwaddr            the hardware address
 *
 * @return                  xu_true or xu_false
 */
xu_bool_t xu_ifaddrs_hwaddr(xu_ifaddrs_ref_t ifaddrs, xu_char_t const* name, xu_bool_t reload, xu_hwaddr_ref_t hwaddr);

/*! the hardware address from the given interface name
 *
 * @param ifaddrs           the ifaddrs
 * @param name              the interface name, get the first ether address if be null
 * @param reload            force to reload the ifaddrs list, will cache list if be false
 * @param family            the address family
 * @param ipaddr            the ip address
 *
 * @return                  xu_true or xu_false
 */
xu_bool_t xu_ifaddrs_ipaddr(xu_ifaddrs_ref_t ifaddrs, xu_char_t const* name, xu_bool_t reload, xu_size_t family,
                            xu_ipaddr_ref_t ipaddr);

#ifdef __xu_debug__
/*! dump the ifaddrs
 *
 * @param ifaddrs           the ifaddrs
 */
xu_void_t xu_ifaddrs_dump(xu_ifaddrs_ref_t ifaddrs);
#endif
