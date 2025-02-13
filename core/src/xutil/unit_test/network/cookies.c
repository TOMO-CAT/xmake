#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_cookies_main(xu_int_t argc, xu_char_t** argv)
{
    // set cookies
    xu_cookies_set(xu_cookies(), "www.baidu.com", "/", xu_false, "HttpOnly;Secure");
    xu_cookies_set(xu_cookies(), xu_null, xu_null, xu_false,
                   "BAIDUID=0; max-age=946080000; expires=Sat, 06-Apr-41 03:21:53 GMT; domain=.baidu.com; path=/");
    xu_cookies_set(
        xu_cookies(), xu_null, xu_null, xu_true,
        "FG=1; max-age=946080000; expires=Sat, 06-Apr-2017 03:21:53 GMT; domain=.baidu.com; path=/; version=1");
    xu_cookies_set(xu_cookies(), xu_null, xu_null, xu_false,
                   "BAIDUID=2; max-age=946080000; expires=Sat, 06-Apr-2016 03:21:53 GMT; domain=.baidu.com; "
                   "path=/style; version=1");
    xu_cookies_set(xu_cookies(), xu_null, xu_null, xu_false,
                   "BG=3; expires=Wednesday, 09-Nov-99 23:12:40 GMT; domain=.space.baidu.com; path=/");
    xu_cookies_set(xu_cookies(), xu_null, xu_null, xu_false,
                   "BAIDSID=4; expires=Wednesday, 09-Nov-99 23:12:40 GMT; domain=.baidu.com; path=/style/foo");
    xu_cookies_set(xu_cookies(), xu_null, xu_null, xu_false,
                   "pvid=3317093836; path=/; domain=qq.com; expires=Sun, 18 Jan 2038 00:00:00 GMT;");
    xu_cookies_set(xu_cookies(), xu_null, xu_null, xu_false,
                   "clientuin=; EXPIRES=Fri, 02-Jan-1970 00:00:00 GMT; PATH=/; DOMAIN=qq.com;");
    xu_cookies_set_from_url(xu_cookies(), "http://mail.163.com:2000/WorldClient.dll?View=Main",
                            "User=wangrq; Expires=Tue, 19-Apr-2015 07:10:56 GMT; path=/");
    xu_cookies_set_from_url(xu_cookies(), "http://mail.163.com:2000/WorldClient.dll?View=Main",
                            "Session=Yz9eJRh6QijR; path=/");
    xu_cookies_set_from_url(xu_cookies(), "http://mail.163.com:2000/WorldClient.dll?View=Main",
                            "Lang=zh; Expires=Tue, 19-Apr-2015 07:10:56 GMT; path=/");
    xu_cookies_set_from_url(xu_cookies(), "http://mail.163.com:2000/WorldClient.dll?View=Main",
                            "Theme=Standard; Expires=Tue, 19-Apr-2015 07:10:56 GMT; path=/");

    // get cookies
    xu_string_t value;
    xu_string_init(&value);
    xu_trace_i("%s", xu_cookies_get_from_url(xu_cookies(), "http://www.space.baidu.com/style/foo/login.css", &value));
    xu_trace_i("%s", xu_cookies_get_from_url(xu_cookies(), "http://www.baidu.com/style/foo/login.css", &value));
    xu_trace_i("%s", xu_cookies_get_from_url(xu_cookies(), "http://www.baidu.com/style", &value));
    xu_trace_i("%s", xu_cookies_get_from_url(xu_cookies(), "http://www.baidu.com", &value));
    xu_trace_i("%s", xu_cookies_get_from_url(xu_cookies(), "https://www.baidu.com/", &value));
    xu_trace_i("%s", xu_cookies_get_from_url(xu_cookies(),
                                             "http://pingfore.qq.com/pingd?dm=qzone.qq.com&url=login/"
                                             "qzone_n&tt=-&rdm=-&rurl=-&pvid=7466815060&scr=-&scl=-&lang=-&java=1&cc=-&"
                                             "pf=-&tz=-8&ct=-&vs=3.3&emu=0.20486706611700356",
                                             &value));
    xu_trace_i("%s",
               xu_cookies_get_from_url(xu_cookies(), "http://mail.163.com:2000/?Session=LZBMQVW&View=Menu", &value));
    xu_string_exit(&value);

    return 0;
}
