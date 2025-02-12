#include "xutil/unit_test/ut.h"

/* *******************************************************
 * test
 */
static xu_void_t xu_integer_test_isqrti(xu_uint32_t x)
{
    __xu_volatile__ xu_long_t   n = 10000000;
    __xu_volatile__ xu_uint32_t r = 0;
    xu_hong_t                   t = xu_mclock();
    while (n--)
    {
        r = xu_isqrti(x);
    }
    t = xu_mclock() - t;
    xu_printf("[integer]: isqrti(%u): %u, %lld ms\n", x, r, t);
}
static xu_void_t xu_integer_test_isqrti64(xu_uint64_t x)
{
    __xu_volatile__ xu_long_t   n = 10000000;
    __xu_volatile__ xu_uint32_t r = 0;
    xu_hong_t                   t = xu_mclock();
    while (n--)
    {
        r = xu_isqrti64(x);
    }
    t = xu_mclock() - t;
    xu_printf("[integer]: isqrti64(%llu): %u, %lld ms\n", x, r, t);
}
static xu_void_t xu_integer_test_ilog2i(xu_uint32_t x)
{
    __xu_volatile__ xu_long_t n = 10000000;
    __xu_volatile__ xu_long_t r = 0;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        r = xu_ilog2i(x);
    }
    t = xu_mclock() - t;
    xu_printf("[integer]: ilog2i(%u): %lu, %lld ms\n", x, r, t);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_libm_integer_main(xu_int_t argc, xu_char_t** argv)
{
    // ilog2i
    xu_integer_test_ilog2i(0);
    xu_integer_test_ilog2i(256);
    xu_integer_test_ilog2i(1096);
    xu_integer_test_ilog2i(65537);
    xu_integer_test_ilog2i(1 << 30);

    // isqrti
    xu_integer_test_isqrti(0);
    xu_integer_test_isqrti(256);
    xu_integer_test_isqrti(1096);
    xu_integer_test_isqrti(65537);
    xu_integer_test_isqrti(1 << 30);

    // isqrti64
    xu_integer_test_isqrti64(0);
    xu_integer_test_isqrti64(256);
    xu_integer_test_isqrti64(1096);
    xu_integer_test_isqrti64(65537);
    xu_integer_test_isqrti64(1 << 30);
    xu_integer_test_isqrti64(1ULL << 35);
    xu_integer_test_isqrti64(1ULL << 45);
    xu_integer_test_isqrti64(1ULL << 60);
    xu_integer_test_isqrti64(1ULL << 62);
    xu_integer_test_isqrti64((1ULL << 56) + 123456789);

    return 0;
}
