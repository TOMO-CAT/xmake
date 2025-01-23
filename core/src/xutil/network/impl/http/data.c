/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "http_date"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/network/impl/http/data.h"
#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/misc/time/time.h"
#include "xutil/libc/stdlib/stdlib.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

/* *******************************************************
 * implementation
 */
xu_time_t xu_http_date_from_cstr(xu_char_t const* cstr, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(cstr && size, 0);

    // done
    xu_tm_t          tm   = {0};
    xu_time_t        date = 0;
    xu_char_t const* p    = cstr;
    xu_char_t const* e    = cstr + size;
    do
    {
        // skip space
        while (p < e && xu_isspace(*p))
            p++;

            // ignore
#if 0
        // parse week
        if ((p + 6 < e && !xu_strnicmp(p, "Monday", 6)) || (p + 3 < e && !xu_strnicmp(p, "Mon", 3)))
            tm.week = 1;
        else if ((p + 7 < e && !xu_strnicmp(p, "Tuesday", 7)) || (p + 3 < e && !xu_strnicmp(p, "Tue", 3)))
            tm.week = 2;
        else if ((p + 9 < e && !xu_strnicmp(p, "Wednesday", 9)) || (p + 3 < e && !xu_strnicmp(p, "Wed", 3)))
            tm.week = 3;
        else if ((p + 8 < e && !xu_strnicmp(p, "Thursday", 8)) || (p + 3 < e && !xu_strnicmp(p, "Thu", 3)))
            tm.week = 4;
        else if ((p + 6 < e && !xu_strnicmp(p, "Friday", 6)) || (p + 3 < e && !xu_strnicmp(p, "Fri", 3)))
            tm.week = 5;
        else if ((p + 8 < e && !xu_strnicmp(p, "Saturday", 8)) || (p + 3 < e && !xu_strnicmp(p, "Sat", 3)))
            tm.week = 6;
        else if ((p + 6 < e && !xu_strnicmp(p, "Sunday", 6)) || (p + 3 < e && !xu_strnicmp(p, "Sun", 3)))
            tm.week = 7;
#endif

        // skip week
        while (p < e && *p != ',' && !xu_isspace(*p))
            p++;

        if (p < e && (*p == ',' || xu_isspace(*p))) p++;

        // skip space
        while (p < e && xu_isspace(*p))
            p++;

        // is day?
        xu_bool_t year_suffix = xu_true;
        if (p < e && xu_isdigit(*p))
        {
            /* prefix year
             *
             * .e.g
             * year_suffix == false: Sun, 06-Nov-1994 08:49:37
             * year_suffix == true: Sun Nov 6 08:49:37 1994
             */
            year_suffix = xu_false;

            // parse day
            tm.mday = xu_s10tou32(p);

            // skip day
            while (p < e && *p != '-' && !xu_isspace(*p))
                p++;

            if (p < e && (*p == '-' || xu_isspace(*p))) p++;
        }

        // parse month
        if (p + 3 < e && !xu_strnicmp(p, "Jan", 3))
            tm.month = 1;
        else if (p + 3 < e && !xu_strnicmp(p, "Feb", 3))
            tm.month = 2;
        else if (p + 3 < e && !xu_strnicmp(p, "Mar", 3))
            tm.month = 3;
        else if (p + 3 < e && !xu_strnicmp(p, "Apr", 3))
            tm.month = 4;
        else if (p + 3 < e && !xu_strnicmp(p, "May", 3))
            tm.month = 5;
        else if (p + 3 < e && !xu_strnicmp(p, "Jun", 3))
            tm.month = 6;
        else if (p + 3 < e && !xu_strnicmp(p, "Jul", 3))
            tm.month = 7;
        else if (p + 3 < e && !xu_strnicmp(p, "Aug", 3))
            tm.month = 8;
        else if (p + 3 < e && !xu_strnicmp(p, "Sep", 3))
            tm.month = 9;
        else if (p + 3 < e && !xu_strnicmp(p, "Oct", 3))
            tm.month = 10;
        else if (p + 3 < e && !xu_strnicmp(p, "Nov", 3))
            tm.month = 11;
        else if (p + 3 < e && !xu_strnicmp(p, "Dec", 3))
            tm.month = 12;

        // skip month
        while (p < e && *p != '-' && !xu_isspace(*p))
            p++;

        if (p < e && (*p == '-' || xu_isspace(*p))) p++;

        // year suffix?
        if (year_suffix)
        {
            // parse day
            tm.mday = xu_s10tou32(p);
        }
        else
        {
            // parse year
            tm.year = xu_s10tou32(p);
            if (tm.year < 100) tm.year += 2000;
        }

        // skip year or day
        while (p < e && !xu_isspace(*p))
            p++;
        while (p < e && xu_isspace(*p))
            p++;

        // parse hour
        tm.hour = xu_s10tou32(p);

        // skip hour
        while (p < e && *p != ':')
            p++;

        if (p < e && *p == ':') p++;

        // parse minute
        tm.minute = xu_s10tou32(p);

        // skip minute
        while (p < e && *p != ':')
            p++;

        if (p < e && *p == ':') p++;

        // parse second
        tm.second = xu_s10tou32(p);

        // year suffix?
        if (year_suffix)
        {
            // skip time
            while (p < e && !xu_isspace(*p))
                p++;
            while (p < e && xu_isspace(*p))
                p++;

            // parse year
            tm.year = xu_s10tou32(p);
            if (tm.year < 100) tm.year += 1900;
        }

        // make date
        date = xu_gmmktime(&tm);

    } while (0);

    // ok?
    return date;
}
