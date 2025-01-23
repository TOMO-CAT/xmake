/* *******************************************************
 *  trace
 */
#define XU_TRACE_MODULE_NAME "ipv6"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/network/ipv6.h"
#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/network/ipaddr.h"
#include "xutil/network/ipv4.h"
#include "xutil/platform/ifaddrs.h"
#include "xutil/utils/bits.h"

/* *******************************************************
 *  interfaces
 */
xu_void_t xu_ipv6_clear(xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert_and_check_return(ipv6);

    // clear it
    xu_memset(ipv6, 0, sizeof(xu_ipv6_t));
}
xu_bool_t xu_ipv6_is_any(xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert_and_check_return_val(ipv6, xu_true);

    // is any?
    return !(ipv6->addr.u32[0] || ipv6->addr.u32[1] || ipv6->addr.u32[2] || ipv6->addr.u32[3]);
}
xu_bool_t xu_ipv6_is_loopback(xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert_and_check_return_val(ipv6, xu_true);

    // is loopback?
    return !(ipv6->addr.u32[0] || ipv6->addr.u32[1] || ipv6->addr.u32[2]) && (ipv6->addr.u32[3] == 0x01000000);
}
xu_bool_t xu_ipv6_is_linklocal(xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert_and_check_return_val(ipv6, xu_true);

    // is linklocal?
    return (ipv6->addr.u8[0] == 0xfe) && ((ipv6->addr.u8[1] & 0xc0) == 0x80);
}
xu_bool_t xu_ipv6_is_mc_linklocal(xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert_and_check_return_val(ipv6, xu_true);

    // is mc linklocal?
    return xu_ipv6_is_multicast(ipv6) && ((ipv6->addr.u8[1] & 0x0f) == 0x02);
}
xu_bool_t xu_ipv6_is_sitelocal(xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert_and_check_return_val(ipv6, xu_true);

    // is sitelocal?
    return (ipv6->addr.u8[0] == 0xfe) && ((ipv6->addr.u8[1] & 0xc0) == 0xc0);
}
xu_bool_t xu_ipv6_is_multicast(xu_ipv6_ref_t ipv6)
{
    // check
    xu_assert_and_check_return_val(ipv6, xu_true);

    // is multicast?
    return (ipv6->addr.u8[0] == 0xff);
}
xu_bool_t xu_ipv6_is_equal(xu_ipv6_ref_t ipv6, xu_ipv6_ref_t other)
{
    // check
    xu_assert_and_check_return_val(ipv6 && other, xu_false);

    // have scope id?
    xu_bool_t have_scope_id = xu_ipv6_is_linklocal(ipv6) || xu_ipv6_is_mc_linklocal(ipv6);

    // is equal?
    return (!have_scope_id || (ipv6->scope_id == other->scope_id)) && (ipv6->addr.u32[0] == other->addr.u32[0]) &&
           (ipv6->addr.u32[1] == other->addr.u32[1]) && (ipv6->addr.u32[2] == other->addr.u32[2]) &&
           (ipv6->addr.u32[3] == other->addr.u32[3]);
}
xu_char_t const* xu_ipv6_cstr(xu_ipv6_ref_t ipv6, xu_char_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(ipv6 && data && maxn >= XU_IPV6_CSTR_MAXN, xu_null);

    // make scope_id
    xu_char_t scope_id[20]  = {0};
    xu_bool_t have_scope_id = xu_ipv6_is_linklocal(ipv6) || xu_ipv6_is_mc_linklocal(ipv6);
    if (have_scope_id) xu_snprintf(scope_id, sizeof(scope_id) - 1, "%%%u", ipv6->scope_id);

    // make ipv6
    xu_long_t size = xu_snprintf(
        data, maxn - 1, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x%s", xu_bits_swap_u16(ipv6->addr.u16[0]),
        xu_bits_swap_u16(ipv6->addr.u16[1]), xu_bits_swap_u16(ipv6->addr.u16[2]), xu_bits_swap_u16(ipv6->addr.u16[3]),
        xu_bits_swap_u16(ipv6->addr.u16[4]), xu_bits_swap_u16(ipv6->addr.u16[5]), xu_bits_swap_u16(ipv6->addr.u16[6]),
        xu_bits_swap_u16(ipv6->addr.u16[7]), have_scope_id ? scope_id : "");
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
xu_bool_t xu_ipv6_cstr_set(xu_ipv6_ref_t ipv6, xu_char_t const* cstr)
{
    // check
    xu_assert_and_check_return_val(cstr, xu_false);

    // ipv4: ::ffff:xxx.xxx.xxx.xxx?
    if (!xu_strnicmp(cstr, "::ffff:", 7))
    {
        // attempt to make ipv6 from ipv4
        xu_ipv4_t ipv4;
        if (xu_ipv4_cstr_set(&ipv4, cstr + 7))
        {
            // make ipv6
            ipv6->addr.u32[0] = 0;
            ipv6->addr.u32[1] = 0;
            ipv6->addr.u32[2] = 0xffff0000;
            ipv6->addr.u32[3] = ipv4.u32;

            // ok
            return xu_true;
        }
    }
    // ipv4: ::xxx.xxx.xxx.xxx?
    else if (!xu_strnicmp(cstr, "::", 2))
    {
        // attempt to make ipv6 from ipv4
        xu_ipv4_t ipv4;
        if (xu_ipv4_cstr_set(&ipv4, cstr + 2))
        {
            // make ipv6
            ipv6->addr.u32[0] = 0;
            ipv6->addr.u32[1] = 0;
            ipv6->addr.u32[2] = 0;
            ipv6->addr.u32[3] = ipv4.u32;

            // ok
            return xu_true;
        }
    }

    // done
    xu_uint32_t      v    = 0;
    xu_size_t        i    = 0;
    xu_char_t        c    = '\0';
    xu_char_t const* p    = cstr;
    xu_bool_t        ok   = xu_true;
    xu_bool_t        stub = xu_false;
    xu_char_t        prev = '\0';
    xu_ipv6_t        temp = {0};
    do
    {
        // save previous character
        prev = c;

        // the character
        c = *p++;

        // digit?
        if (xu_isdigit16(c) && v <= 0xffff)
        {
            // update value
            if (xu_isdigit10(c))
                v = (v << 4) + (c - '0');
            else if (c > ('a' - 1) && c < ('f' + 1))
                v = (v << 4) + (c - 'a') + 10;
            else if (c > ('A' - 1) && c < ('F' + 1))
                v = (v << 4) + (c - 'A') + 10;
            else
            {
                // abort
                xu_assert(0);

                // failed
                ok = xu_false;
                break;
            }
        }
        // "::"?
        else if (c == ':' && *p == ':' && p[1] != ':' && !stub)
        {
            // save value
            temp.addr.u16[i++] = xu_bits_swap_u16(v);

            // clear value
            v = 0;

            // clear previous value
            prev = '\0';

            // find the left count of ':'
            xu_long_t        n = 0;
            xu_char_t const* q = p;
            while (*q)
            {
                if (*q == ':') n++;
                q++;
            }
            xu_check_break_state(n <= 6, ok, xu_false);

            // compute the stub count
            n = 8 - n - i;
            xu_check_break_state(n > 0, ok, xu_false);

            // save the stub value
            while (n-- > 0)
                temp.addr.u16[i++] = 0;

            // only one "::"
            stub = xu_true;

            // skip ':'
            p++;
        }
        // ':' or '\0' or '%'?
        else if (i < 8 && ((c == ':' && *p != ':') || !c) && v <= 0xffff && prev)
        {
            // save value
            temp.addr.u16[i++] = xu_bits_swap_u16(v);

            // clear value
            v = 0;

            // clear previous value
            prev = '\0';
        }
        // "%xxx"?
        else if (i == 7 && c == '%' && *p)
        {
            // save value
            temp.addr.u16[i++] = xu_bits_swap_u16(v);

            // is scope id?
            if (xu_isdigit(*p))
            {
                // save the scope id
                temp.scope_id = xu_atoi(p);

                // trace
                xu_trace_d("scope_id: %u", temp.scope_id);
            }
#ifndef XU_CONFIG_MICRO_ENABLE
            // is interface name?
            else
            {
                // trace
                xu_trace_d("name: %s", p);

                // get the scope id from the interface name
                xu_ipaddr_t ipaddr;
                if (xu_ifaddrs_ipaddr(xu_ifaddrs(), p, xu_false, XU_IPADDR_FAMILY_IPV6, &ipaddr))
                {
                    // trace
                    xu_trace_d("scope_id: %u", ipaddr.u.ipv6.scope_id);

                    // save the scope id
                    temp.scope_id = ipaddr.u.ipv6.scope_id;
                }
                // clear the scope id
                else
                    temp.scope_id = 0;
            }
#else
            // clear the scope id
            else
                temp.scope_id = 0;
#endif

            // end
            break;
        }
        // failed?
        else
        {
            ok = xu_false;
            break;
        }

    } while (c);

    // failed
    if (i != 8) ok = xu_false;

    // save it if ok
    if (ok && ipv6) *ipv6 = temp;

    // trace
    //    xu_assertf(ok, "invalid addr: %s", cstr);

    // ok?
    return ok;
}
