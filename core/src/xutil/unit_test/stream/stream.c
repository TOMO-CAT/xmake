#include "xutil/unit_test/ut.h"

/* *******************************************************
 * types
 */
typedef struct __xu_demo_context_t
{
    // verbose
    xu_bool_t verbose;

} xu_demo_context_t;

/* *******************************************************
 * func
 */
#ifdef XU_CONFIG_MODULE_HAVE_OBJECT
static xu_bool_t xu_demo_http_post_func(xu_size_t state, xu_hize_t offset, xu_hong_t size, xu_hize_t save,
                                        xu_size_t rate, xu_cpointer_t priv)
{
    // percent
    xu_size_t percent = 0;
    if (size > 0)
        percent = (xu_size_t)((offset * 100) / size);
    else if (state == XU_STATE_CLOSED)
        percent = 100;

    // trace
    xu_trace_i("post: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", save, rate, percent, xu_state_cstr(state));

    // ok
    return xu_true;
}
static xu_bool_t xu_demo_stream_head_func(xu_char_t const* line, xu_cpointer_t priv)
{
    xu_printf("response: %s\n", line);
    return xu_true;
}
static xu_bool_t xu_demo_stream_save_func(xu_size_t state, xu_hize_t offset, xu_hong_t size, xu_hize_t save,
                                          xu_size_t rate, xu_cpointer_t priv)
{
    // check
    xu_demo_context_t* context = (xu_demo_context_t*)priv;
    xu_assert_and_check_return_val(context, xu_false);

    // print verbose info
    if (context->verbose)
    {
        // percent
        xu_size_t percent = 0;
        if (size > 0)
            percent = (xu_size_t)((offset * 100) / size);
        else if (state == XU_STATE_CLOSED)
            percent = 100;

        // trace
        xu_printf("save: %llu bytes, rate: %lu bytes/s, percent: %lu%%, state: %s\n", save, rate, percent,
                  xu_state_cstr(state));
    }

    // ok
    return xu_true;
}

/* *******************************************************
 * globals
 */
static xu_option_item_t g_options[] = {
    {'-', "gzip", XU_OPTION_MODE_KEY, XU_OPTION_TYPE_BOOL, "enable gzip"},
    {'-', "no-verbose", XU_OPTION_MODE_KEY, XU_OPTION_TYPE_BOOL, "disable verbose info"},
    {'d', "debug", XU_OPTION_MODE_KEY, XU_OPTION_TYPE_BOOL, "enable debug info"},
    {'k', "keep-alive", XU_OPTION_MODE_KEY, XU_OPTION_TYPE_BOOL, "keep alive"},
    {'h', "header", XU_OPTION_MODE_KEY_VAL, XU_OPTION_TYPE_CSTR, "the custem http header"},
    {'-', "post-data", XU_OPTION_MODE_KEY_VAL, XU_OPTION_TYPE_CSTR, "set the post data"},
    {'-', "post-file", XU_OPTION_MODE_KEY_VAL, XU_OPTION_TYPE_CSTR, "set the post file"},
    {'-', "range", XU_OPTION_MODE_KEY_VAL, XU_OPTION_TYPE_CSTR, "set the range"},
    {'-', "timeout", XU_OPTION_MODE_KEY_VAL, XU_OPTION_TYPE_INTEGER, "set the timeout"},
    {'-', "limitrate", XU_OPTION_MODE_KEY_VAL, XU_OPTION_TYPE_INTEGER, "set the limitrate"},
    {'h', "help", XU_OPTION_MODE_KEY, XU_OPTION_TYPE_BOOL, "display this help and exit"},
    {'-', "url", XU_OPTION_MODE_VAL, XU_OPTION_TYPE_CSTR, "the url"},
    {'-', xu_null, XU_OPTION_MODE_MORE, XU_OPTION_TYPE_NONE, xu_null}

};

/* *******************************************************
 * main
 */
xu_int_t xu_ut_stream_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_option_ref_t option  = xu_null;
    xu_stream_ref_t istream = xu_null;
    xu_stream_ref_t ostream = xu_null;
    xu_stream_ref_t pstream = xu_null;
    do
    {
        // init option
        option = xu_option_init("stream", "the stream demo", g_options);
        xu_assert_and_check_break(option);

        // done option
        if (xu_option_done(option, argc - 1, &argv[1]))
        {
            // debug & verbose
            xu_bool_t debug   = xu_option_find(option, "debug");
            xu_bool_t verbose = xu_option_find(option, "no-verbose") ? xu_false : xu_true;

            // done url
            if (xu_option_find(option, "url"))
            {
                // init istream
                istream = xu_stream_init_from_url(xu_option_item_cstr(option, "url"));
                xu_assert_and_check_break(istream);

                // ctrl http
                if (xu_stream_type(istream) == XU_STREAM_TYPE_HTTP)
                {
                    // enable gzip?
                    if (xu_option_find(option, "gzip"))
                    {
                        // auto unzip
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_AUTO_UNZIP, 1)) break;

                        // need gzip
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_HEAD, "Accept-Encoding", "gzip,deflate"))
                            break;
                    }

                    // enable debug?
                    if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_HEAD_FUNC,
                                        debug ? xu_demo_stream_head_func : xu_null))
                        break;

                    // custem header?
                    if (xu_option_find(option, "header"))
                    {
                        // init
                        xu_string_t key;
                        xu_string_t val;
                        xu_string_init(&key);
                        xu_string_init(&val);

                        // done
                        xu_bool_t        k = xu_true;
                        xu_char_t const* p = xu_option_item_cstr(option, "header");
                        while (*p)
                        {
                            // is key?
                            if (k)
                            {
                                if (*p != ':' && !xu_isspace(*p))
                                    xu_string_chrcat(&key, *p++);
                                else if (*p == ':')
                                {
                                    // skip ':'
                                    p++;

                                    // skip space
                                    while (*p && xu_isspace(*p))
                                        p++;

                                    // is val now
                                    k = xu_false;
                                }
                                else
                                    p++;
                            }
                            // is val?
                            else
                            {
                                if (*p != ';')
                                    xu_string_chrcat(&val, *p++);
                                else
                                {
                                    // skip ';'
                                    p++;

                                    // skip space
                                    while (*p && xu_isspace(*p))
                                        p++;

                                    // set header
                                    if (xu_string_size(&key) && xu_string_size(&val))
                                    {
                                        if (debug)
                                            xu_printf("header: %s: %s\n", xu_string_cstr(&key), xu_string_cstr(&val));
                                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_HEAD, xu_string_cstr(&key),
                                                            xu_string_cstr(&val)))
                                            break;
                                    }

                                    // is key now
                                    k = xu_true;

                                    // clear key & val
                                    xu_string_clear(&key);
                                    xu_string_clear(&val);
                                }
                            }
                        }

                        // set header
                        if (xu_string_size(&key) && xu_string_size(&val))
                        {
                            if (debug) xu_printf("header: %s: %s\n", xu_string_cstr(&key), xu_string_cstr(&val));
                            if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_HEAD, xu_string_cstr(&key),
                                                xu_string_cstr(&val)))
                                break;
                        }

                        // exit
                        xu_string_exit(&key);
                        xu_string_exit(&val);
                    }

                    // keep alive?
                    if (xu_option_find(option, "keep-alive"))
                    {
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_HEAD, "Connection", "keep-alive")) break;
                    }

                    // post-data?
                    if (xu_option_find(option, "post-data"))
                    {
                        xu_char_t const* post_data = xu_option_item_cstr(option, "post-data");
                        xu_hize_t        post_size = xu_strlen(post_data);
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_METHOD, XU_HTTP_METHOD_POST)) break;
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_POST_DATA, post_data, post_size)) break;
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_POST_FUNC, xu_demo_http_post_func)) break;
                        if (debug) xu_printf("post: %llu\n", post_size);
                    }
                    // post-file?
                    else if (xu_option_find(option, "post-file"))
                    {
                        xu_char_t const* url = xu_option_item_cstr(option, "post-file");
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_METHOD, XU_HTTP_METHOD_POST)) break;
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_POST_URL, url)) break;
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_POST_FUNC, xu_demo_http_post_func)) break;
                        if (debug) xu_printf("post: %s\n", url);
                    }
                }

                // set range
                if (xu_option_find(option, "range"))
                {
                    xu_char_t const* p = xu_option_item_cstr(option, "range");
                    if (p)
                    {
                        // the bof
                        xu_hize_t eof = 0;
                        xu_hize_t bof = xu_atoll(p);
                        while (*p && xu_isdigit(*p))
                            p++;
                        if (*p == '-')
                        {
                            p++;
                            eof = xu_atoll(p);
                        }
                        if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_HTTP_SET_RANGE, bof, eof)) break;
                    }
                }

                // set timeout
                if (xu_option_find(option, "timeout"))
                {
                    xu_size_t timeout = xu_option_item_uint32(option, "timeout");
                    if (!xu_stream_ctrl(istream, XU_STREAM_CTRL_SET_TIMEOUT, timeout)) break;
                }

                // print verbose info
                if (verbose) xu_printf("open: %s: ..\n", xu_option_item_cstr(option, "url"));

                // open istream
                if (!xu_stream_open(istream))
                {
                    // print verbose info
                    if (verbose) xu_printf("open: %s\n", xu_state_cstr(xu_stream_state(istream)));
                    break;
                }

                // print verbose info
                if (verbose) xu_printf("open: ok\n");

                // init ostream
                if (xu_option_find(option, "more0"))
                {
                    // the path
                    xu_char_t const* path = xu_option_item_cstr(option, "more0");

                    // init
                    ostream = xu_stream_init_from_file(path, XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);

                    // print verbose info
                    if (verbose) xu_printf("save: %s\n", path);
                }
                else
                {
                    // the name
                    xu_char_t const* name = xu_strrchr(xu_option_item_cstr(option, "url"), '/');
                    if (!name) name = xu_strrchr(xu_option_item_cstr(option, "url"), '\\');
                    if (!name) name = "/stream.file";

                    // the path
                    xu_char_t path[XU_PATH_MAXN] = {0};
                    if (xu_directory_current(path, XU_PATH_MAXN))
                        xu_strcat(path, name);
                    else
                        break;

                    // init file
                    ostream = xu_stream_init_from_file(path, XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);

                    // print verbose info
                    if (verbose) xu_printf("save: %s\n", path);
                }
                xu_assert_and_check_break(ostream);

                // the limit rate
                xu_size_t limitrate = 0;
                if (xu_option_find(option, "limitrate")) limitrate = xu_option_item_uint32(option, "limitrate");

                // save it
                xu_hong_t         save    = 0;
                xu_demo_context_t context = {0};
                context.verbose           = verbose;
                if ((save = xu_transfer(istream, ostream, limitrate, xu_demo_stream_save_func, &context)) < 0) break;
            }
            else
                xu_option_help(option);
        }
        else
            xu_option_help(option);

    } while (0);

    // exit pstream
    if (pstream) xu_stream_exit(pstream);
    pstream = xu_null;

    // exit istream
    if (istream) xu_stream_exit(istream);
    istream = xu_null;

    // exit ostream
    if (ostream) xu_stream_exit(ostream);
    ostream = xu_null;

    // exit option
    if (option) xu_option_exit(option);
    option = xu_null;

    return 0;
}
#else
xu_int_t xu_ut_stream_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_stream_ref_t istream = xu_null;
    xu_stream_ref_t ostream = xu_null;
    do
    {
        // init istream
        istream = xu_stream_init_from_url(argv[1]);
        xu_assert_and_check_break(istream);

        // init ostream
        ostream = xu_stream_init_from_file(argv[2], XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);

        // open istream
        if (!xu_stream_open(istream)) break;

        // open ostream
        if (!xu_stream_open(ostream)) break;

        // writ data
        xu_byte_t data[XU_STREAM_BLOCK_MAXN];
        xu_hize_t writ = 0;
        xu_hize_t left = xu_stream_left(istream);
        do
        {
            // read data
            xu_long_t real = xu_stream_read(istream, data, XU_STREAM_BLOCK_MAXN);
            if (real > 0)
            {
                // writ data
                if (!xu_stream_bwrit(ostream, data, real)) break;

                // save writ
                writ += real;
            }
            else if (!real)
            {
                // wait
                xu_long_t wait = xu_stream_wait(istream, XU_STREAM_WAIT_READ, xu_stream_timeout(istream));
                xu_assert_and_check_break(wait >= 0);

                // timeout?
                xu_check_break(wait);

                // has writ?
                xu_assert_and_check_break(wait & XU_STREAM_WAIT_READ);
            }
            else
                break;

            // is end?
            if (writ >= left) break;

        } while (1);

    } while (0);

    // exit istream
    if (istream) xu_stream_exit(istream);
    istream = xu_null;

    // exit ostream
    if (ostream) xu_stream_exit(ostream);
    ostream = xu_null;
    return 0;
}
#endif
