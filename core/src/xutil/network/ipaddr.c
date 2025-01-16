#include "xutil/network/ipaddr.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

/* *******************************************************
 *  private implementation
 * *******************************************************
 */

static __xu_inline__ xu_bool_t xu_ipaddr_ipv6_to_ipv4(xu_ipv6_ref_t ipv6, xu_ipv4_ref_t ipv4)
{
    // check
    xu_assert(ipv6 && ipv4);

    // is ipv4?
    if (!ipv6->addr.u32[0] && !ipv6->addr.u32[1] && ipv6->addr.u32[2] == 0xffff0000)
    {
        // make ipv4
        ipv4->u32 = ipv6->addr.u32[3];

        // ok
        return xu_true;
    }

    // failed
    return xu_false;
}
static __xu_inline__ xu_bool_t xu_ipaddr_ipv4_to_ipv6(xu_ipv4_ref_t ipv4, xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert(ipv6 && ipv4);

    // make ipv6
    ipv6->addr.u32[0] = 0;
    ipv6->addr.u32[1] = 0;
    ipv6->addr.u32[2] = 0xffff0000;
    ipv6->addr.u32[3] = ipv4->u32;
    ipv6->scope_id    = 0;

    // ok
    return xu_true;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_void_t xu_ipaddr_clear(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return(ipaddr);

    // clear it
    ipaddr->family  = XU_IPADDR_FAMILY_NONE;
    ipaddr->have_ip = 0;
    ipaddr->port    = 0;
}
xu_void_t xu_ipaddr_copy(xu_ipaddr_ref_t ipaddr, xu_ipaddr_ref_t copied)
{
    // check
    xu_assert_and_check_return(ipaddr && copied);

    // no ip? only copy port and family
    if (!copied->have_ip)
    {
        ipaddr->port   = copied->port;
        ipaddr->family = copied->family;
    }
    // attempt to copy ipv4 fast
    else if (copied->family == XU_IPADDR_FAMILY_IPV4)
    {
        ipaddr->port    = copied->port;
        ipaddr->have_ip = 1;
        ipaddr->family  = XU_IPADDR_FAMILY_IPV4;
        ipaddr->u.ipv4  = copied->u.ipv4;
    }
    // copy it
    else
        *ipaddr = *copied;
}
xu_bool_t xu_ipaddr_is_empty(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_true);

    // no ip?
    return xu_ipaddr_ip_is_empty(ipaddr);
}
xu_bool_t xu_ipaddr_is_equal(xu_ipaddr_ref_t ipaddr, xu_ipaddr_ref_t other)
{
    // check
    xu_assert_and_check_return_val(ipaddr && other, xu_false);

    // port is equal?
    if (ipaddr->port != other->port) return xu_false;

    // ip is equal?
    return xu_ipaddr_ip_is_equal(ipaddr, other);
}
xu_char_t const* xu_ipaddr_cstr(xu_ipaddr_ref_t ipaddr, xu_char_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(ipaddr && data && maxn >= XU_IPADDR_CSTR_MAXN, xu_null);

    // is empty?
    if (xu_ipaddr_is_empty(ipaddr))
    {
        // make it
        xu_long_t size = xu_snprintf(data, maxn - 1, "0.0.0.0:0");
        if (size >= 0) data[size] = '\0';
    }
    // ip is empty?
    else if (xu_ipaddr_ip_is_empty(ipaddr))
    {
        // make it
        xu_long_t size = xu_snprintf(data, maxn - 1, "0.0.0.0:%u", ipaddr->port);
        if (size >= 0) data[size] = '\0';
    }
    else if (ipaddr->family == XU_IPADDR_FAMILY_UNIX)
    {
        // check (again)
        xu_assert_and_check_return_val(maxn >= XU_UNIXADDR_CSTR_MAXN, xu_null);

        // make it
        xu_unixaddr_cstr(&ipaddr->u.unixaddr, data, maxn);
    }
    else
    {
        // make it
        xu_char_t buff[XU_IPADDR_CSTR_MAXN];
        xu_bool_t ipv6 = ipaddr->family == XU_IPADDR_FAMILY_IPV6;
        xu_long_t size = xu_snprintf(data, maxn - 1, "%s%s%s:%u", ipv6 ? "[" : "",
                                     xu_ipaddr_ip_cstr(ipaddr, buff, sizeof(buff)), ipv6 ? "]" : "", ipaddr->port);
        if (size >= 0) data[size] = '\0';
    }

    // ok
    return data;
}
xu_bool_t xu_ipaddr_set(xu_ipaddr_ref_t ipaddr, xu_char_t const* cstr, xu_uint16_t port, xu_uint8_t family)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_false);

    // save port
    xu_ipaddr_port_set(ipaddr, family != XU_IPADDR_FAMILY_UNIX ? port : 0);

    // save ip address and family
    return xu_ipaddr_ip_cstr_set(ipaddr, cstr, family);
}
xu_void_t xu_ipaddr_ip_clear(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return(ipaddr);

    // clear ip
    ipaddr->have_ip = 0;
}
xu_bool_t xu_ipaddr_ip_is_empty(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_true);

    // is empty?
    return !ipaddr->have_ip;
}
xu_bool_t xu_ipaddr_ip_is_any(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_true);

    // is empty? ok
    xu_check_return_val(ipaddr->have_ip, xu_true);

    // done
    xu_bool_t is_any = xu_true;
    switch (ipaddr->family)
    {
    case XU_IPADDR_FAMILY_IPV4: is_any = xu_ipv4_is_any(&ipaddr->u.ipv4); break;
    case XU_IPADDR_FAMILY_IPV6: is_any = xu_ipv6_is_any(&ipaddr->u.ipv6); break;
    case XU_IPADDR_FAMILY_UNIX: is_any = xu_false; break;
    default: break;
    }

    // is any?
    return is_any;
}
xu_bool_t xu_ipaddr_ip_is_loopback(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_true);

    // done
    xu_bool_t is_loopback = xu_false;
    switch (ipaddr->family)
    {
    case XU_IPADDR_FAMILY_IPV4: is_loopback = xu_ipv4_is_loopback(&ipaddr->u.ipv4); break;
    case XU_IPADDR_FAMILY_IPV6: is_loopback = xu_ipv6_is_loopback(&ipaddr->u.ipv6); break;
    case XU_IPADDR_FAMILY_UNIX: is_loopback = xu_false; break;
    default: break;
    }

    // is loopback?
    return is_loopback;
}
xu_bool_t xu_ipaddr_ip_is_equal(xu_ipaddr_ref_t ipaddr, xu_ipaddr_ref_t other)
{
    // check
    xu_assert_and_check_return_val(ipaddr && other, xu_false);

    // both empty?
    if (!ipaddr->have_ip && !other->have_ip) return xu_true;
    // only one is empty?
    else if (ipaddr->have_ip != other->have_ip)
        return xu_false;
    // both ipv4?
    else if (ipaddr->family == XU_IPADDR_FAMILY_IPV4 && other->family == XU_IPADDR_FAMILY_IPV4)
    {
        // is equal?
        return xu_ipv4_is_equal(&ipaddr->u.ipv4, &other->u.ipv4);
    }
    // both ipv6?
    else if (ipaddr->family == XU_IPADDR_FAMILY_IPV6 && other->family == XU_IPADDR_FAMILY_IPV6)
    {
        // is equal?
        return xu_ipv6_is_equal(&ipaddr->u.ipv6, &other->u.ipv6);
    }
    // both unixaddr?
    else if (ipaddr->family == XU_IPADDR_FAMILY_UNIX && other->family == XU_IPADDR_FAMILY_UNIX)
    {
        // is equal?
        return xu_unixaddr_is_equal(&ipaddr->u.unixaddr, &other->u.unixaddr);
    }
    // only one is unixaddr?
    else if (ipaddr->family == XU_IPADDR_FAMILY_UNIX || other->family == XU_IPADDR_FAMILY_UNIX)
        return xu_false;
    // ipaddr is ipv6?
    else if (ipaddr->family == XU_IPADDR_FAMILY_IPV6)
    {
        // is equal?
        xu_ipv4_t ipv4;
        return xu_ipaddr_ipv6_to_ipv4(&ipaddr->u.ipv6, &ipv4) && xu_ipv4_is_equal(&ipv4, &other->u.ipv4);
    }
    // other is ipv6?
    else if (other->family == XU_IPADDR_FAMILY_IPV6)
    {
        // is equal?
        xu_ipv4_t ipv4;
        return xu_ipaddr_ipv6_to_ipv4(&other->u.ipv6, &ipv4) && xu_ipv4_is_equal(&ipaddr->u.ipv4, &ipv4);
    }

    // failed
    xu_assert(0);
    return xu_false;
}
xu_char_t const* xu_ipaddr_ip_cstr(xu_ipaddr_ref_t ipaddr, xu_char_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(ipaddr && data && maxn, xu_null);

    // done
    xu_char_t const* cstr = xu_null;
    switch (ipaddr->family)
    {
    case XU_IPADDR_FAMILY_IPV4:
    {
        // make ipv4 cstr
        if (ipaddr->have_ip)
            cstr = xu_ipv4_cstr(&ipaddr->u.ipv4, data, maxn);
        else
        {
            // check
            xu_assert(maxn >= XU_IPV4_CSTR_MAXN);

            // make empty cstr
            xu_long_t size = xu_snprintf(data, maxn - 1, "0.0.0.0");
            if (size >= 0) data[size] = '\0';

            // ok
            cstr = data;
        }
    }
    break;
    case XU_IPADDR_FAMILY_IPV6:
    {
        // make ipv6 cstr
        if (ipaddr->have_ip)
            cstr = xu_ipv6_cstr(&ipaddr->u.ipv6, data, maxn);
        else
        {
            // check
            xu_assert(maxn >= XU_IPV6_CSTR_MAXN);

            // make empty cstr
            xu_long_t size = xu_snprintf(data, maxn - 1, "::");
            if (size >= 0) data[size] = '\0';

            // ok
            cstr = data;
        }
    }
    break;
    case XU_IPADDR_FAMILY_UNIX:
    {
        // make unixaddr cstr
        if (ipaddr->have_ip)
            cstr = xu_unixaddr_cstr(&ipaddr->u.unixaddr, data, maxn);
        else
        {
            // check
            xu_assert(maxn >= XU_UNIXADDR_CSTR_MAXN);

            // make empty cstr
            xu_memset(data, 0, maxn);

            // ok
            cstr = data;
        }
    }
    break;
    default: xu_assert(0); break;
    }

    // ok?
    return cstr;
}
xu_bool_t xu_ipaddr_ip_cstr_set(xu_ipaddr_ref_t ipaddr, xu_char_t const* cstr, xu_uint8_t family)
{
    // no ip? clear it fast
    if (!cstr)
    {
        // check
        xu_assert(ipaddr);

        // clear it
        ipaddr->family  = family;
        ipaddr->have_ip = 0;
        return xu_true;
    }

    // done
    xu_bool_t   ok = xu_false;
    xu_ipaddr_t temp;
    switch (family)
    {
    case XU_IPADDR_FAMILY_IPV4:
    {
        // make ipv4
        ok = xu_ipv4_cstr_set(&temp.u.ipv4, cstr);

        // make family
        if (ok) temp.family = family;
    }
    break;
    case XU_IPADDR_FAMILY_IPV6:
    {
        // make ipv6
        ok = xu_ipv6_cstr_set(&temp.u.ipv6, cstr);

        // make family
        if (ok) temp.family = family;
    }
    break;
    case XU_IPADDR_FAMILY_UNIX:
    {
        // make unixaddr
        ok = xu_unixaddr_cstr_set(&temp.u.unixaddr, cstr, xu_false);

        // make family
        if (ok) temp.family = family;
    }
    break;
    default:
    {
        // attempt to make ipv4
        if ((ok = xu_ipv4_cstr_set(&temp.u.ipv4, cstr))) temp.family = XU_IPADDR_FAMILY_IPV4;
        // make ipv6
        else if ((ok = xu_ipv6_cstr_set(&temp.u.ipv6, cstr)))
            temp.family = XU_IPADDR_FAMILY_IPV6;
    }
    break;
    }

    // ok? save it
    if (ok && ipaddr)
    {
        // save port
        temp.port = ipaddr->port;

        // have ip?
        temp.have_ip = 1;

        // save ipaddr
        xu_ipaddr_copy(ipaddr, &temp);
    }

    // ok?
    return ok;
}
xu_void_t xu_ipaddr_ip_set(xu_ipaddr_ref_t ipaddr, xu_ipaddr_ref_t other)
{
    // check
    xu_assert_and_check_return(ipaddr);

    // no ip? clear it
    if (!other)
    {
        ipaddr->have_ip = 0;
        return;
    }

    // done
    switch (other->family)
    {
    case XU_IPADDR_FAMILY_IPV4:
    {
        // save ipv4
        xu_ipaddr_ipv4_set(ipaddr, &other->u.ipv4);

        // save state
        ipaddr->have_ip = 1;
    }
    break;
    case XU_IPADDR_FAMILY_IPV6:
    {
        // save ipv6
        xu_ipaddr_ipv6_set(ipaddr, &other->u.ipv6);

        // save state
        ipaddr->have_ip = 1;
    }
    break;
    case XU_IPADDR_FAMILY_UNIX:
    {
        // save unixaddr
        xu_ipaddr_unix_set(ipaddr, &other->u.unixaddr);

        // save state
        ipaddr->have_ip = 1;
    }
    default: xu_assert(0); break;
    }
}
xu_ipv4_ref_t xu_ipaddr_ipv4(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_null);

    // no ip?
    xu_check_return_val(ipaddr->have_ip, xu_null);

    // done
    xu_ipv4_ref_t ipv4 = xu_null;
    switch (ipaddr->family)
    {
    case XU_IPADDR_FAMILY_IPV4: ipv4 = &ipaddr->u.ipv4; break;
    case XU_IPADDR_FAMILY_IPV6:
    {
        xu_ipv4_t temp;
        if (xu_ipaddr_ipv6_to_ipv4(&ipaddr->u.ipv6, &temp))
        {
            ipaddr->family = XU_IPADDR_FAMILY_IPV4;
            ipaddr->u.ipv4 = temp;
            ipv4           = &ipaddr->u.ipv4;
        }
    }
    break;
    case XU_IPADDR_FAMILY_UNIX: break;
    default: xu_assert(0); break;
    }

    // ok?
    return ipv4;
}
xu_void_t xu_ipaddr_ipv4_set(xu_ipaddr_ref_t ipaddr, xu_ipv4_ref_t ipv4)
{
    // check
    xu_assert_and_check_return(ipaddr);

    // no ipv4? clear it
    if (!ipv4)
    {
        ipaddr->have_ip = 0;
        return;
    }

    // save it
    ipaddr->family  = XU_IPADDR_FAMILY_IPV4;
    ipaddr->have_ip = 1;
    ipaddr->u.ipv4  = *ipv4;
}
xu_ipv6_ref_t xu_ipaddr_ipv6(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_null);

    // no ip?
    xu_check_return_val(ipaddr->have_ip, xu_null);

    // done
    xu_ipv6_ref_t ipv6 = xu_null;
    switch (ipaddr->family)
    {
    case XU_IPADDR_FAMILY_IPV4:
    {
        xu_ipv6_t temp;
        if (xu_ipaddr_ipv4_to_ipv6(&ipaddr->u.ipv4, &temp))
        {
            ipaddr->family = XU_IPADDR_FAMILY_IPV6;
            ipaddr->u.ipv6 = temp;
            ipv6           = &ipaddr->u.ipv6;
        }
    }
    break;
    case XU_IPADDR_FAMILY_IPV6: ipv6 = &ipaddr->u.ipv6; break;
    case XU_IPADDR_FAMILY_UNIX: break;
    default: xu_assert(0); break;
    }

    // ok?
    return ipv6;
}
xu_void_t xu_ipaddr_ipv6_set(xu_ipaddr_ref_t ipaddr, xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert_and_check_return(ipaddr && ipv6);

    // no ipv6? clear it
    if (!ipv6)
    {
        ipaddr->have_ip = 0;
        return;
    }

    // save it
    ipaddr->family  = XU_IPADDR_FAMILY_IPV6;
    ipaddr->u.ipv6  = *ipv6;
    ipaddr->have_ip = 1;
}
xu_unixaddr_ref_t xu_ipaddr_unix(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_null);

    // no ip?
    xu_check_return_val(ipaddr->have_ip, xu_null);

    // done
    if (ipaddr->family == XU_IPADDR_FAMILY_UNIX) return &ipaddr->u.unixaddr;

    return xu_null;
}
xu_void_t xu_ipaddr_unix_set(xu_ipaddr_ref_t ipaddr, xu_unixaddr_ref_t unixaddr)
{
    // check
    xu_assert_and_check_return(ipaddr && unixaddr);

    // no ipv6? clear it
    if (!unixaddr)
    {
        ipaddr->have_ip = 0;
        return;
    }

    // save it
    ipaddr->family     = XU_IPADDR_FAMILY_UNIX;
    ipaddr->u.unixaddr = *unixaddr;
    ipaddr->have_ip    = 1;
}
xu_bool_t xu_ipaddr_unix_set_cstr(xu_ipaddr_ref_t ipaddr, xu_char_t const* cstr, xu_bool_t is_abstract)
{
    // check
    xu_assert_and_check_return_val(ipaddr, xu_false);

    // make unixaddr
    xu_ipaddr_t temp;
    if (!xu_unixaddr_cstr_set(&temp.u.unixaddr, cstr, is_abstract)) return xu_false;

    // make family
    temp.family = XU_IPADDR_FAMILY_UNIX;

    // have ip?
    temp.have_ip = 1;

    // Set port to 0 = not used
    temp.port = 0;

    // save ipaddr
    xu_ipaddr_copy(ipaddr, &temp);
    return xu_true;
}
xu_size_t xu_ipaddr_family(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, XU_IPADDR_FAMILY_NONE);

    // the family
    return ipaddr->family;
}
xu_void_t xu_ipaddr_family_set(xu_ipaddr_ref_t ipaddr, xu_size_t family)
{
    // check
    xu_assert_and_check_return(ipaddr);

    // ipv4 => ipv6?
    if (ipaddr->family == XU_IPADDR_FAMILY_IPV4 && family == XU_IPADDR_FAMILY_IPV6)
    {
        xu_ipv6_t temp;
        if (xu_ipaddr_ipv4_to_ipv6(&ipaddr->u.ipv4, &temp))
        {
            ipaddr->family = XU_IPADDR_FAMILY_IPV6;
            ipaddr->u.ipv6 = temp;
        }
        else
        {
            // check
            xu_assert(0);
        }
    }
    // ipv6 => ipv4?
    else if (ipaddr->family == XU_IPADDR_FAMILY_IPV4 && family == XU_IPADDR_FAMILY_IPV6)
    {
        xu_ipv4_t temp;
        if (xu_ipaddr_ipv6_to_ipv4(&ipaddr->u.ipv6, &temp))
        {
            ipaddr->family = XU_IPADDR_FAMILY_IPV4;
            ipaddr->u.ipv4 = temp;
        }
        else
        {
            // check
            xu_assert(0);
        }
    }
    // unixaddr cannot be converted
    else if ((ipaddr->family == XU_IPADDR_FAMILY_UNIX) != (family == XU_IPADDR_FAMILY_UNIX) &&
             family != XU_IPADDR_FAMILY_NONE)
    {
        // check
        xu_assert(0);
    }
    else
        ipaddr->family = family;

    // no family? clear ip
    if (!ipaddr->family) ipaddr->have_ip = 0;
}
xu_uint16_t xu_ipaddr_port(xu_ipaddr_ref_t ipaddr)
{
    // check
    xu_assert_and_check_return_val(ipaddr, 0);

    // get port
    return ipaddr->port;
}
xu_void_t xu_ipaddr_port_set(xu_ipaddr_ref_t ipaddr, xu_uint16_t port)
{
    // check
    xu_assert_and_check_return(ipaddr);

    // set port
    ipaddr->port = port;
}
