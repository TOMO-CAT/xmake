#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_memory_string_pool_main(xu_int_t argc, xu_char_t** argv)
{
#if 0
    // hello
    xu_char_t const* hello = xu_string_pool_insert(xu_string_pool(), "hello world");
    xu_trace_i("hello: %s", hello);

    // performance
    xu_char_t s[256] = {0};
    xu_hong_t t = xu_mclock();
    __xu_volatile__ xu_size_t n = 1000000;
    while (n--)
    {
        xu_int_t r = xu_snprintf(s, 256, "%u", xu_random_range(0, 10000));
        s[r] = '\0';
#    if 1
        xu_string_pool_insert(xu_string_pool(), s);
        if (!(n & 15)) xu_string_pool_remove(xu_string_pool(), s);
#    else
        xu_free(xu_strdup(s));
#    endif
    }
    t = xu_mclock() - t;
    xu_trace_i("time: %lld", t);

    // del hello
    xu_string_pool_remove(xu_string_pool(), hello);
#endif
    return 0;
}
