/* *******************************************************
 *  trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "ipv4"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/network/ipv4.h"
#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/macros/assert.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
xu_void_t xu_ipv4_clear(xu_ipv4_ref_t ipv4)
{
    // check
    xu_assert_and_check_return(ipv4);

    // clear it
    ipv4->u32 = 0;
}
xu_bool_t xu_ipv4_is_any(xu_ipv4_ref_t ipv4)
{
    // check
    xu_assert_and_check_return_val(ipv4, xu_true);

    // is empty?
    return !ipv4->u32;
}
xu_bool_t xu_ipv4_is_loopback(xu_ipv4_ref_t ipv4)
{
    // check
    xu_assert_and_check_return_val(ipv4, xu_false);

    // is loopback?
    return (ipv4->u32 == 0x0100007f);
}
xu_bool_t xu_ipv4_is_equal(xu_ipv4_ref_t ipv4, xu_ipv4_ref_t other)
{
    // check
    xu_assert_and_check_return_val(ipv4 && other, xu_false);

    // is equal?
    return ipv4->u32 == other->u32;
}
xu_char_t const* xu_ipv4_cstr(xu_ipv4_ref_t ipv4, xu_char_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(ipv4 && data && maxn >= XU_IPV4_CSTR_MAXN, xu_null);

    // make it
    xu_long_t size = xu_snprintf(data, maxn - 1, "%u.%u.%u.%u", ipv4->u8[0], ipv4->u8[1], ipv4->u8[2], ipv4->u8[3]);
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
xu_bool_t xu_ipv4_cstr_set(xu_ipv4_ref_t ipv4, xu_char_t const* cstr)
{
    // check
    xu_assert_and_check_return_val(cstr, xu_false);

    // done
    xu_uint32_t      r = 0;
    xu_uint32_t      v = 0;
    xu_char_t        c = '\0';
    xu_size_t        i = 0;
    xu_char_t const* p = cstr;
    do
    {
        // the character
        c = *p++;

        // digit?
        if (xu_isdigit10(c) && v <= 0xff)
        {
            v *= 10;
            v += (xu_uint32_t)(c - '0') & 0xff;
        }
        // '.' or '\0'?
        else if ((c == '.' || !c) && v <= 0xff)
        {
            r |= ((xu_uint32_t)v) << ((i++) << 3);
            v = 0;
        }
        // failed?
        else
        {
            // trace
            xu_trace_d("invalid ipv4 addr: %s", cstr);

            // clear it
            i = 0;
            break;
        }

    } while (c);

    // save it if ok
    if (ipv4) ipv4->u32 = r;

    // ok?
    return i == 4;
}
