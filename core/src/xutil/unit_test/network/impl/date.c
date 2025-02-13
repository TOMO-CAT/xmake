#include "xutil/unit_test/ut.h"

#include "xutil/network/impl/http/date.h"

/* *******************************************************
 * test
 */
static xu_void_t xu_demo_test_date(xu_char_t const* cstr)
{
    xu_tm_t date = {0};
    if (xu_gmtime(xu_http_date_from_cstr(cstr, xu_strlen(cstr)), &date))
    {
        xu_trace_i("%s => %04ld-%02ld-%02ld %02ld:%02ld:%02ld GMT, week: %d", cstr, date.year, date.month, date.mday,
                   date.hour, date.minute, date.second, date.week);
    }
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_impl_date_main(xu_int_t argc, xu_char_t** argv)
{
    xu_demo_test_date("Sun, 06 Nov 1994 08:49:37 GMT");
    xu_demo_test_date("Sun Nov 6 08:49:37 1994");
    xu_demo_test_date("Sun, 06-Nov-1994 08:49:37 GMT");
    xu_demo_test_date("Mon, 19 May 2014 07:21:56 GMT");
    xu_demo_test_date("Thu, 31-Dec-37 23:55:55 GMT");

    return 0;
}
