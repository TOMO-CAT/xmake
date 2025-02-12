#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_bool_t xu_http_demo_head_func(xu_char_t const* line, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(line, xu_false);

    // trace
    xu_trace_i("head: %s", line);

    // ok
    return xu_true;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_http_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_http_ref_t http = xu_null;
    do
    {
        // init http
        http = xu_http_init();
        xu_assert_and_check_break(http);

        // init cookies
        if (!xu_http_ctrl(http, XU_HTTP_OPTION_SET_COOKIES, xu_cookies())) break;

        // init head func
        if (!xu_http_ctrl(http, XU_HTTP_OPTION_SET_HEAD_FUNC, xu_http_demo_head_func)) break;

        // init url
        if (!xu_http_ctrl(http, XU_HTTP_OPTION_SET_URL, argv[1])) break;

        // init head
        //        if (!xu_http_ctrl(http, XU_HTTP_OPTION_SET_HEAD, "Connection", "keep-alive")) break;

        // init post
        if (argv[2])
        {
            // init post url
            if (!xu_http_ctrl(http, XU_HTTP_OPTION_SET_POST_URL, argv[2])) break;

            // init method
            if (!xu_http_ctrl(http, XU_HTTP_OPTION_SET_METHOD, XU_HTTP_METHOD_POST)) break;
        }

        // init timeout
        xu_size_t timeout = 0;
        //  if (!xu_http_ctrl(http, XU_HTTP_OPTION_SET_TIMEOUT, 10000)) break;
        if (!xu_http_ctrl(http, XU_HTTP_OPTION_GET_TIMEOUT, &timeout)) break;

        // init redirect maxn
        //  if (!xu_http_ctrl(http, XU_HTTP_OPTION_SET_REDIRECT, 0)) break;

        // open http
        xu_hong_t t = xu_mclock();
        if (!xu_http_open(http)) break;
        t = xu_mclock() - t;
        xu_trace_i("open: %llu ms", t);

        // read data
        xu_byte_t data[8192];
        xu_size_t read = 0;
        xu_hize_t size = xu_http_status(http)->content_size;
        do
        {
            // read data
            xu_long_t real = xu_http_read(http, data, 8192);
            xu_trace_i("read: %d", real);
            if (real > 0)
            {
                // dump data
                xu_char_t const* p = (xu_char_t const*)data;
                xu_char_t const* e = (xu_char_t const*)data + real;
                xu_char_t        b[8192 + 1];
                while (p < e && *p)
                {
                    xu_char_t*       q = b;
                    xu_char_t const* d = b + 4096;
                    for (; q < d && p < e && *p; p++, q++)
                        *q = *p;
                    *q = '\0';
                    xu_printf("%s", b);
                }
                xu_printf("\n");

                // save read
                read += real;
            }
            else if (!real)
            {
                // wait
                xu_trace_i("wait");
                xu_long_t e = xu_http_wait(http, XU_SOCKET_EVENT_RECV, timeout);
                xu_assert_and_check_break(e >= 0);

                // timeout?
                xu_check_break(e);

                // has read?
                xu_assert_and_check_break(e & XU_SOCKET_EVENT_RECV);
            }
            else
                break;

            // is end?
            if (size && read >= size) break;

        } while (1);

    } while (0);

    // exit http
    if (http) xu_http_exit(http);

    // end
    return 0;
}
