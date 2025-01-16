#include "xutil/network/hwaddr.h"
#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

/* *******************************************************
 * interfaces
 * *******************************************************
 */
xu_void_t xu_hwaddr_clear(xu_hwaddr_ref_t hwaddr)
{
    // check
    xu_assert_and_check_return(hwaddr);

    // clear it
    xu_memset(hwaddr->u8, 0, sizeof(hwaddr->u8));
}
xu_void_t xu_hwaddr_copy(xu_hwaddr_ref_t hwaddr, xu_hwaddr_ref_t copied)
{
    // check
    xu_assert_and_check_return(hwaddr && copied);

    // copy it
    hwaddr->u8[0] = copied->u8[0];
    hwaddr->u8[1] = copied->u8[1];
    hwaddr->u8[2] = copied->u8[2];
    hwaddr->u8[3] = copied->u8[3];
    hwaddr->u8[4] = copied->u8[4];
    hwaddr->u8[5] = copied->u8[5];
}
xu_bool_t xu_hwaddr_is_equal(xu_hwaddr_ref_t hwaddr, xu_hwaddr_ref_t other)
{
    // check
    xu_assert_and_check_return_val(hwaddr && other, xu_false);

    // is equal?
    return !xu_memcmp(hwaddr->u8, other->u8, sizeof(hwaddr->u8));
}
xu_char_t const* xu_hwaddr_cstr(xu_hwaddr_ref_t hwaddr, xu_char_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(hwaddr && data && maxn >= XU_HWADDR_CSTR_MAXN, xu_null);

    // make it
    xu_long_t size = xu_snprintf(data, maxn - 1, "%02x:%02x:%02x:%02x:%02x:%02x", hwaddr->u8[0], hwaddr->u8[1],
                                 hwaddr->u8[2], hwaddr->u8[3], hwaddr->u8[4], hwaddr->u8[5]);
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
xu_bool_t xu_hwaddr_cstr_set(xu_hwaddr_ref_t hwaddr, xu_char_t const* cstr)
{
    // check
    xu_assert_and_check_return_val(cstr, xu_false);

    // done
    xu_uint32_t      v  = 0;
    xu_char_t        c  = '\0';
    xu_size_t        i  = 0;
    xu_char_t const* p  = cstr;
    xu_bool_t        ok = xu_true;
    xu_hwaddr_t      temp;
    do
    {
        // the character
        c = *p++;

        // digit?
        if (xu_isdigit16(c) && v <= 0xff)
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
        // ':' or "-" or '\0'?
        else if (i < 6 && (c == ':' || c == '-' || !c) && v <= 0xff)
        {
            // save value
            temp.u8[i++] = v;

            // clear value
            v = 0;
        }
        // failed?
        else
        {
            ok = xu_false;
            break;
        }

    } while (c);

    // failed
    if (i != 6) ok = xu_false;

    // save it if ok
    if (ok && hwaddr) *hwaddr = temp;

    // trace
    //    xu_assertf(ok, "invalid hwaddr: %s", cstr);

    // ok?
    return ok;
}
