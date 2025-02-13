#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */
#define XU_WHOIS_TEST_HOST_WALK (1)

/* *******************************************************
 * test
 */
#if !XU_WHOIS_TEST_HOST_WALK
static xu_void_t xu_whois_test_done(xu_char_t const* name)
{
    // init
    //  xu_stream_ref_t stream = xu_stream_init_from_url("sock://whois.internic.net:43");
    xu_stream_ref_t stream = xu_stream_init_from_url("sock://199.7.51.74:43");
    //  xu_stream_ref_t stream = xu_stream_init_from_url("sock://whois.cnnic.net.cn:43");
    xu_assert_and_check_return(stream);

    // timeout
    xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_TIMEOUT, 1000);

    // data
    xu_char_t data[251] = {0};

    // open
    if (xu_stream_open(stream))
    {
        xu_stream_printf(stream, "%s \r\n", name);
        xu_stream_sync(stream, xu_true);
        xu_stream_bread(stream, data, 250);
        xu_trace_i("%s", data);
    }

    // exit
    xu_stream_exit(stream);
}
#else
static xu_bool_t xu_whois_test_no_match_com(xu_char_t const* name)
{
    // init
    //  xu_stream_ref_t stream = xu_stream_init_from_url("sock://whois.internic.net:43");
    xu_stream_ref_t stream = xu_stream_init_from_url("sock://199.7.51.74:43");
    xu_assert_and_check_return_val(stream, xu_false);

    // timeout
    xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_TIMEOUT, 1000);

    // data
    xu_char_t data[251] = {0};

    // open
    if (xu_stream_open(stream))
    {
        xu_stream_printf(stream, "%s \r\n", name);
        xu_stream_sync(stream, xu_true);
        xu_stream_bread(stream, (xu_byte_t*)data, 250);
        if (xu_strstr(data + 150, "No match")) return xu_true;
    }

    // exit
    xu_stream_exit(stream);
    return xu_false;
}
static xu_bool_t xu_whois_test_no_match_cn(xu_char_t const* name)
{
    // init
    //  xu_stream_ref_t stream = xu_stream_init_from_url("sock://whois.cnnic.net.cn:43");
    xu_stream_ref_t stream = xu_stream_init_from_url("sock://218.241.97.14:43");
    xu_assert_and_check_return_val(stream, xu_false);

    // timeout
    xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_TIMEOUT, 1000);

    // data
    xu_char_t data[21] = {0};

    // open
    if (xu_stream_open(stream))
    {
        xu_stream_printf(stream, "%s \r\n", name);
        xu_stream_sync(stream, xu_true);
        xu_stream_bread(stream, (xu_byte_t*)data, 20);
        if (xu_strstr(data, "no matching")) return xu_true;
    }

    // exit
    xu_stream_exit(stream);
    return xu_false;
}
static xu_void_t xu_whois_test_walk_2()
{
    // table
    xu_char_t const* t   = "abcdefghijklmnopqrstuvwxyz";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 26; i < 27 * 26; i++)
    {
        j = i;
        b = j % 26;
        j /= 26;
        p[5] = t[b];
        b    = j % 26;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
static xu_void_t xu_whois_test_walk_3()
{
    // table
    xu_char_t const* t   = "abcdefghijklmnopqrstuvwxyz";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 26 * 26; i < 27 * 26 * 26; i++)
    {
        j = i;
        b = j % 26;
        j /= 26;
        p[6] = t[b];
        b    = j % 26;
        j /= 26;
        p[5] = t[b];
        b    = j % 26;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
static xu_void_t xu_whois_test_walk_4()
{
    // table
    xu_char_t const* t   = "abcdefghijklmnopqrstuvwxyz";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 26 * 26 * 26; i < 27 * 26 * 26 * 26; i++)
    {
        j = i;
        b = j % 26;
        j /= 26;
        p[7] = t[b];
        b    = j % 26;
        j /= 26;
        p[6] = t[b];
        b    = j % 26;
        j /= 26;
        p[5] = t[b];
        b    = j % 26;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
static xu_void_t xu_whois_test_walk_5()
{
    // table
    xu_char_t const* t   = "abcdefghijklmnopqrstuvwxyz";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 26 * 26 * 26 * 26; i < 27 * 26 * 26 * 26 * 26; i++)
    {
        j = i;
        b = j % 26;
        j /= 26;
        p[8] = t[b];
        b    = j % 26;
        j /= 26;
        p[7] = t[b];
        b    = j % 26;
        j /= 26;
        p[6] = t[b];
        b    = j % 26;
        j /= 26;
        p[5] = t[b];
        b    = j % 26;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}

static xu_void_t xu_whois_test_walk_6()
{
    // table
    xu_char_t const* t   = "abcdefghijklmnopqrstuvwxyz";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 26 * 26 * 26 * 26 * 26; i < 27 * 26 * 26 * 26 * 26 * 26; i++)
    {
        j = i;
        b = j % 26;
        j /= 26;
        p[9] = t[b];
        b    = j % 26;
        j /= 26;
        p[8] = t[b];
        b    = j % 26;
        j /= 26;
        p[7] = t[b];
        b    = j % 26;
        j /= 26;
        p[6] = t[b];
        b    = j % 26;
        j /= 26;
        p[5] = t[b];
        b    = j % 26;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}

static xu_bool_t xu_whois_test_walk_ping_2(xu_char_t const* file)
{
    // init stream
    xu_stream_ref_t stream = xu_stream_init_from_url(file);
    xu_assert_and_check_return_val(stream, xu_false);

    // init ping
    xu_char_t* ping = xu_malloc0_cstr(1000 * 16);
    xu_assert_and_check_return_val(stream, xu_false);

    // open
    xu_size_t n = 0;
    if (xu_stream_open(stream))
    {
        while (xu_stream_bread_line(stream, &ping[n * 16], 15) > 0)
            n++;
    }

    // exit stream
    xu_stream_exit(stream);

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            xu_char_t p[41] = {0};
            xu_snprintf(p, 40, "%s%s.cn", &ping[i * 16], &ping[j * 16]);
            xu_trace_i("%s: %s", p, xu_whois_test_no_match_cn(p) ? "ok" : "no");
        }
    }

    // exit ping
    xu_free(ping);
    return xu_true;
}
static xu_bool_t xu_whois_test_walk_ping_3(xu_char_t const* file)
{
    // init stream
    xu_stream_ref_t stream = xu_stream_init_from_url(file);
    xu_assert_and_check_return_val(stream, xu_false);

    // init ping
    xu_char_t* ping = xu_malloc0_cstr(1000 * 16);
    xu_assert_and_check_return_val(stream, xu_false);

    // open
    xu_size_t n = 0;
    if (xu_stream_open(stream))
    {
        while (xu_stream_bread_line(stream, &ping[n * 16], 15) > 0)
            n++;
    }

    // exit stream
    xu_stream_exit(stream);

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t k = 0;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            for (k = 0; k < n; k++)
            {
                xu_char_t p[61] = {0};
                xu_snprintf(p, 60, "%s%s%s.cn", &ping[i * 16], &ping[j * 16], &ping[k * 16]);
                xu_trace_i("%s: %s", p, xu_whois_test_no_match_cn(p) ? "ok" : "no");
            }
        }
    }

    // exit ping
    xu_free(ping);
    return xu_true;
}
static xu_void_t xu_whois_test_walk_num_1()
{
    // table
    xu_char_t const* t   = "0123456789";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 0; i < 10; i++)
    {
        j    = i;
        b    = j % 10;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
static xu_void_t xu_whois_test_walk_num_2()
{
    // table
    xu_char_t const* t   = "0123456789";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 10; i < 100; i++)
    {
        j = i;
        b = j % 10;
        j /= 10;
        p[5] = t[b];
        b    = j % 10;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
static xu_void_t xu_whois_test_walk_num_3()
{
    // table
    xu_char_t const* t   = "0123456789";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 100; i < 1000; i++)
    {
        j = i;
        b = j % 10;
        j /= 10;
        p[6] = t[b];
        b    = j % 10;
        j /= 10;
        p[5] = t[b];
        b    = j % 10;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
static xu_void_t xu_whois_test_walk_num_4()
{
    // table
    xu_char_t const* t   = "0123456789";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 1000; i < 10000; i++)
    {
        j = i;
        b = j % 10;
        j /= 10;
        p[7] = t[b];
        b    = j % 10;
        j /= 10;
        p[6] = t[b];
        b    = j % 10;
        j /= 10;
        p[5] = t[b];
        b    = j % 10;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
static xu_void_t xu_whois_test_walk_num_5()
{
    // table
    xu_char_t const* t   = "0123456789";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 10000; i < 100000; i++)
    {
        j = i;
        b = j % 10;
        j /= 10;
        p[8] = t[b];
        b    = j % 10;
        j /= 10;
        p[7] = t[b];
        b    = j % 10;
        j /= 10;
        p[6] = t[b];
        b    = j % 10;
        j /= 10;
        p[5] = t[b];
        b    = j % 10;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
static xu_void_t xu_whois_test_walk_num_6()
{
    // table
    xu_char_t const* t   = "0123456789";
    xu_char_t        p[] = {'w', 'w', 'w', '.', 'x', 'x', 'x', 'x', 'x', 'x', '.', 'c', 'o', 'm', '\0'};

    // walk
    xu_size_t i = 0;
    xu_size_t j = 0;
    xu_size_t b = 0;
    for (i = 100000; i < 1000000; i++)
    {
        j = i;
        b = j % 10;
        j /= 10;
        p[9] = t[b];
        b    = j % 10;
        j /= 10;
        p[8] = t[b];
        b    = j % 10;
        j /= 10;
        p[7] = t[b];
        b    = j % 10;
        j /= 10;
        p[6] = t[b];
        b    = j % 10;
        j /= 10;
        p[5] = t[b];
        b    = j % 10;
        p[4] = t[b];

        xu_trace_i("%s: %s", p, xu_whois_test_no_match_com(&p[4]) ? "ok" : "no");
    }
}
#endif

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_whois_main(xu_int_t argc, xu_char_t** argv)
{
#if XU_WHOIS_TEST_HOST_WALK
    xu_whois_test_walk_2();
    xu_whois_test_walk_3();
    xu_whois_test_walk_4();
    xu_whois_test_walk_5();
    xu_whois_test_walk_6();
    xu_whois_test_walk_num_1();
    xu_whois_test_walk_num_2();
    xu_whois_test_walk_num_3();
    xu_whois_test_walk_num_4();
    xu_whois_test_walk_num_5();
    xu_whois_test_walk_num_6();
    xu_whois_test_walk_ping_2(argv[1]);
    xu_whois_test_walk_ping_3(argv[1]);
#else
    xu_whois_test_done(argv[1]);
#endif

    return 0;
}
