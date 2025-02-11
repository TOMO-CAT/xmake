#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */

#ifdef __xu_debug__
#    define xu_hash_map_test_dump(h) xu_hash_map_dump(h)
#else
#    define xu_hash_map_test_dump(h)
#endif

#define xu_hash_map_test_get_s2i(h, s)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_assert(xu_strlen((xu_char_t*)s) == (xu_size_t)xu_hash_map_get(h, (xu_char_t*)(s)));                         \
    } while (0);
#define xu_hash_map_test_insert_s2i(h, s)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_size_t n = xu_strlen((xu_char_t*)(s));                                                                      \
        xu_hash_map_insert(h, (xu_char_t*)(s), (xu_pointer_t)n);                                                       \
    } while (0);
#define xu_hash_map_test_remove_s2i(h, s)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_hash_map_remove(h, s);                                                                                      \
        xu_assert(!xu_hash_map_get(h, s));                                                                             \
    } while (0);

#define xu_hash_map_test_get_i2s(h, i)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_char_t s[256] = {0};                                                                                        \
        xu_snprintf(s, 256, "%u", i);                                                                                  \
        xu_assert(!xu_strcmp(s, (xu_char_t const*)xu_hash_map_get(h, (xu_pointer_t)i)));                               \
    } while (0);
#define xu_hash_map_test_insert_i2s(h, i)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_char_t s[256] = {0};                                                                                        \
        xu_snprintf(s, 256, "%u", i);                                                                                  \
        xu_hash_map_insert(h, (xu_pointer_t)i, s);                                                                     \
    } while (0);
#define xu_hash_map_test_remove_i2s(h, i)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_hash_map_remove(h, (xu_pointer_t)i);                                                                        \
        xu_assert(!xu_hash_map_get(h, (xu_pointer_t)i));                                                               \
    } while (0);

#define xu_hash_map_test_get_m2m(h, i)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_memset_u32(item, i, step >> 2);                                                                             \
        xu_assert(!xu_memcmp(item, xu_hash_map_get(h, item), step));                                                   \
    } while (0);
#define xu_hash_map_test_insert_m2m(h, i)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_memset_u32(item, i, step >> 2);                                                                             \
        xu_hash_map_insert(h, item, item);                                                                             \
    } while (0);
#define xu_hash_map_test_remove_m2m(h, i)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_memset_u32(item, i, step >> 2);                                                                             \
        xu_hash_map_remove(h, item);                                                                                   \
        xu_assert(!xu_hash_map_get(h, item));                                                                          \
    } while (0);

#define xu_hash_map_test_get_i2i(h, i)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_assert(i == (xu_size_t)xu_hash_map_get(h, (xu_pointer_t)i));                                                \
    } while (0);
#define xu_hash_map_test_insert_i2i(h, i)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_hash_map_insert(h, (xu_pointer_t)i, (xu_pointer_t)i);                                                       \
    } while (0);
#define xu_hash_map_test_remove_i2i(h, i)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_hash_map_remove(h, (xu_pointer_t)i);                                                                        \
        xu_assert(!xu_hash_map_get(h, (xu_pointer_t)i));                                                               \
    } while (0);

#define xu_hash_map_test_get_i2t(h, i)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_assert(xu_hash_map_get(h, (xu_pointer_t)i));                                                                \
    } while (0);
#define xu_hash_map_test_insert_i2t(h, i)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_hash_map_insert(h, (xu_pointer_t)i, (xu_pointer_t)(xu_size_t)xu_true);                                      \
    } while (0);
#define xu_hash_map_test_remove_i2t(h, i)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_hash_map_remove(h, (xu_pointer_t)i);                                                                        \
        xu_assert(!xu_hash_map_get(h, (xu_pointer_t)i));                                                               \
    } while (0);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static xu_void_t xu_hash_map_test_s2i_func()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(8, xu_element_str(xu_true), xu_element_long());
    xu_assert_and_check_return(hash);

    // set
    xu_hash_map_test_insert_s2i(hash, "");
    xu_hash_map_test_insert_s2i(hash, "0");
    xu_hash_map_test_insert_s2i(hash, "01");
    xu_hash_map_test_insert_s2i(hash, "012");
    xu_hash_map_test_insert_s2i(hash, "0123");
    xu_hash_map_test_insert_s2i(hash, "01234");
    xu_hash_map_test_insert_s2i(hash, "012345");
    xu_hash_map_test_insert_s2i(hash, "0123456");
    xu_hash_map_test_insert_s2i(hash, "01234567");
    xu_hash_map_test_insert_s2i(hash, "012345678");
    xu_hash_map_test_insert_s2i(hash, "0123456789");
    xu_hash_map_test_insert_s2i(hash, "9876543210");
    xu_hash_map_test_insert_s2i(hash, "876543210");
    xu_hash_map_test_insert_s2i(hash, "76543210");
    xu_hash_map_test_insert_s2i(hash, "6543210");
    xu_hash_map_test_insert_s2i(hash, "543210");
    xu_hash_map_test_insert_s2i(hash, "43210");
    xu_hash_map_test_insert_s2i(hash, "3210");
    xu_hash_map_test_insert_s2i(hash, "210");
    xu_hash_map_test_insert_s2i(hash, "10");
    xu_hash_map_test_insert_s2i(hash, "0");
    xu_hash_map_test_insert_s2i(hash, "");
    xu_hash_map_test_dump(hash);

    // get
    xu_hash_map_test_get_s2i(hash, "");
    xu_hash_map_test_get_s2i(hash, "01");
    xu_hash_map_test_get_s2i(hash, "012");
    xu_hash_map_test_get_s2i(hash, "0123");
    xu_hash_map_test_get_s2i(hash, "01234");
    xu_hash_map_test_get_s2i(hash, "012345");
    xu_hash_map_test_get_s2i(hash, "0123456");
    xu_hash_map_test_get_s2i(hash, "01234567");
    xu_hash_map_test_get_s2i(hash, "012345678");
    xu_hash_map_test_get_s2i(hash, "0123456789");
    xu_hash_map_test_get_s2i(hash, "9876543210");
    xu_hash_map_test_get_s2i(hash, "876543210");
    xu_hash_map_test_get_s2i(hash, "76543210");
    xu_hash_map_test_get_s2i(hash, "6543210");
    xu_hash_map_test_get_s2i(hash, "543210");
    xu_hash_map_test_get_s2i(hash, "43210");
    xu_hash_map_test_get_s2i(hash, "3210");
    xu_hash_map_test_get_s2i(hash, "210");
    xu_hash_map_test_get_s2i(hash, "10");
    xu_hash_map_test_get_s2i(hash, "0");
    xu_hash_map_test_get_s2i(hash, "");

    // del
    xu_hash_map_test_remove_s2i(hash, "");
    xu_hash_map_test_remove_s2i(hash, "01");
    xu_hash_map_test_remove_s2i(hash, "012");
    xu_hash_map_test_remove_s2i(hash, "0123");
    xu_hash_map_test_remove_s2i(hash, "01234");
    xu_hash_map_test_remove_s2i(hash, "012345");
    xu_hash_map_test_remove_s2i(hash, "0123456");
    xu_hash_map_test_remove_s2i(hash, "01234567");
    xu_hash_map_test_remove_s2i(hash, "012345678");
    xu_hash_map_test_remove_s2i(hash, "0123456789");
    xu_hash_map_test_remove_s2i(hash, "0123456789");
    xu_hash_map_test_dump(hash);

    // clear
    xu_hash_map_clear(hash);
    xu_hash_map_test_dump(hash);

    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_s2i_perf()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(0, xu_element_str(xu_true), xu_element_long());
    xu_assert_and_check_return(hash);

    // performance
    xu_char_t                 s[256] = {0};
    __xu_volatile__ xu_size_t n      = 100000;
    xu_hong_t                 t      = xu_mclock();
    while (n--)
    {
        xu_long_t r = xu_snprintf(s, sizeof(s) - 1, "%ld", xu_random_value());
        s[r]        = '\0';
        xu_hash_map_test_insert_s2i(hash, s);
        xu_hash_map_test_get_s2i(hash, s);
    }
    t = xu_mclock() - t;
    xu_trace_i("s2i: time: %lld", t);

    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_i2s_func()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(8, xu_element_long(), xu_element_str(xu_true));
    xu_assert_and_check_return(hash);

    // set
    xu_hash_map_test_insert_i2s(hash, 0);
    xu_hash_map_test_insert_i2s(hash, 1);
    xu_hash_map_test_insert_i2s(hash, 12);
    xu_hash_map_test_insert_i2s(hash, 123);
    xu_hash_map_test_insert_i2s(hash, 1234);
    xu_hash_map_test_insert_i2s(hash, 12345);
    xu_hash_map_test_insert_i2s(hash, 123456);
    xu_hash_map_test_insert_i2s(hash, 1234567);
    xu_hash_map_test_insert_i2s(hash, 12345678);
    xu_hash_map_test_insert_i2s(hash, 123456789);
    xu_hash_map_test_insert_i2s(hash, 876543210);
    xu_hash_map_test_insert_i2s(hash, 76543210);
    xu_hash_map_test_insert_i2s(hash, 6543210);
    xu_hash_map_test_insert_i2s(hash, 543210);
    xu_hash_map_test_insert_i2s(hash, 43210);
    xu_hash_map_test_insert_i2s(hash, 3210);
    xu_hash_map_test_insert_i2s(hash, 210);
    xu_hash_map_test_insert_i2s(hash, 10);
    xu_hash_map_test_insert_i2s(hash, 0);
    xu_hash_map_test_dump(hash);

    // get
    xu_hash_map_test_get_i2s(hash, 0);
    xu_hash_map_test_get_i2s(hash, 1);
    xu_hash_map_test_get_i2s(hash, 12);
    xu_hash_map_test_get_i2s(hash, 123);
    xu_hash_map_test_get_i2s(hash, 1234);
    xu_hash_map_test_get_i2s(hash, 12345);
    xu_hash_map_test_get_i2s(hash, 123456);
    xu_hash_map_test_get_i2s(hash, 1234567);
    xu_hash_map_test_get_i2s(hash, 12345678);
    xu_hash_map_test_get_i2s(hash, 123456789);
    xu_hash_map_test_get_i2s(hash, 876543210);
    xu_hash_map_test_get_i2s(hash, 76543210);
    xu_hash_map_test_get_i2s(hash, 6543210);
    xu_hash_map_test_get_i2s(hash, 543210);
    xu_hash_map_test_get_i2s(hash, 43210);
    xu_hash_map_test_get_i2s(hash, 3210);
    xu_hash_map_test_get_i2s(hash, 210);
    xu_hash_map_test_get_i2s(hash, 10);
    xu_hash_map_test_get_i2s(hash, 0);

    // del
    xu_hash_map_test_remove_i2s(hash, 0);
    xu_hash_map_test_remove_i2s(hash, 1);
    xu_hash_map_test_remove_i2s(hash, 12);
    xu_hash_map_test_remove_i2s(hash, 123);
    xu_hash_map_test_remove_i2s(hash, 1234);
    xu_hash_map_test_remove_i2s(hash, 12345);
    xu_hash_map_test_remove_i2s(hash, 123456);
    xu_hash_map_test_remove_i2s(hash, 1234567);
    xu_hash_map_test_remove_i2s(hash, 12345678);
    xu_hash_map_test_remove_i2s(hash, 123456789);
    xu_hash_map_test_remove_i2s(hash, 123456789);
    xu_hash_map_test_dump(hash);

    // clear
    xu_hash_map_clear(hash);
    xu_hash_map_test_dump(hash);

    // exit
    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_i2s_perf()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(0, xu_element_long(), xu_element_str(xu_true));
    xu_assert_and_check_return(hash);

    // performance
    __xu_volatile__ xu_size_t n = 100000;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        xu_size_t i = xu_random_value();
        xu_hash_map_test_insert_i2s(hash, i);
        xu_hash_map_test_get_i2s(hash, i);
    }
    t = xu_mclock() - t;
    xu_trace_i("i2s: time: %lld", t);

    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_m2m_func()
{
    // init hash
    xu_size_t const   step = 256;
    xu_byte_t         item[step];
    xu_hash_map_ref_t hash =
        xu_hash_map_init(8, xu_element_mem(step, xu_null, xu_null), xu_element_mem(step, xu_null, xu_null));
    xu_assert_and_check_return(hash);

    // set
    xu_hash_map_test_insert_m2m(hash, 0);
    xu_hash_map_test_insert_m2m(hash, 1);
    xu_hash_map_test_insert_m2m(hash, 2);
    xu_hash_map_test_insert_m2m(hash, 3);
    xu_hash_map_test_insert_m2m(hash, 4);
    xu_hash_map_test_insert_m2m(hash, 5);
    xu_hash_map_test_insert_m2m(hash, 6);
    xu_hash_map_test_insert_m2m(hash, 7);
    xu_hash_map_test_insert_m2m(hash, 8);
    xu_hash_map_test_insert_m2m(hash, 9);
    xu_hash_map_test_insert_m2m(hash, 10);
    xu_hash_map_test_insert_m2m(hash, 11);
    xu_hash_map_test_insert_m2m(hash, 12);
    xu_hash_map_test_insert_m2m(hash, 13);
    xu_hash_map_test_insert_m2m(hash, 14);
    xu_hash_map_test_insert_m2m(hash, 15);
    xu_hash_map_test_insert_m2m(hash, 16);
    xu_hash_map_test_insert_m2m(hash, 17);
    xu_hash_map_test_insert_m2m(hash, 18);
    xu_hash_map_test_insert_m2m(hash, 19);
    xu_hash_map_test_insert_m2m(hash, 20);
    xu_hash_map_test_insert_m2m(hash, 21);
    xu_hash_map_test_insert_m2m(hash, 22);
    xu_hash_map_test_insert_m2m(hash, 23);
    xu_hash_map_test_insert_m2m(hash, 24);
    xu_hash_map_test_insert_m2m(hash, 25);
    xu_hash_map_test_insert_m2m(hash, 26);
    xu_hash_map_test_insert_m2m(hash, 27);
    xu_hash_map_test_insert_m2m(hash, 28);
    xu_hash_map_test_insert_m2m(hash, 29);
    xu_hash_map_test_insert_m2m(hash, 30);
    xu_hash_map_test_insert_m2m(hash, 31);
    xu_hash_map_test_insert_m2m(hash, 32);
    xu_hash_map_test_dump(hash);

    // get
    xu_hash_map_test_get_m2m(hash, 0);
    xu_hash_map_test_get_m2m(hash, 1);
    xu_hash_map_test_get_m2m(hash, 2);
    xu_hash_map_test_get_m2m(hash, 3);
    xu_hash_map_test_get_m2m(hash, 4);
    xu_hash_map_test_get_m2m(hash, 5);
    xu_hash_map_test_get_m2m(hash, 6);
    xu_hash_map_test_get_m2m(hash, 7);
    xu_hash_map_test_get_m2m(hash, 8);
    xu_hash_map_test_get_m2m(hash, 9);
    xu_hash_map_test_get_m2m(hash, 10);
    xu_hash_map_test_get_m2m(hash, 11);
    xu_hash_map_test_get_m2m(hash, 12);
    xu_hash_map_test_get_m2m(hash, 13);
    xu_hash_map_test_get_m2m(hash, 14);
    xu_hash_map_test_get_m2m(hash, 15);
    xu_hash_map_test_get_m2m(hash, 16);
    xu_hash_map_test_get_m2m(hash, 17);
    xu_hash_map_test_get_m2m(hash, 18);
    xu_hash_map_test_get_m2m(hash, 19);
    xu_hash_map_test_get_m2m(hash, 20);
    xu_hash_map_test_get_m2m(hash, 21);
    xu_hash_map_test_get_m2m(hash, 22);
    xu_hash_map_test_get_m2m(hash, 23);
    xu_hash_map_test_get_m2m(hash, 24);
    xu_hash_map_test_get_m2m(hash, 25);
    xu_hash_map_test_get_m2m(hash, 26);
    xu_hash_map_test_get_m2m(hash, 27);
    xu_hash_map_test_get_m2m(hash, 28);
    xu_hash_map_test_get_m2m(hash, 29);
    xu_hash_map_test_get_m2m(hash, 30);
    xu_hash_map_test_get_m2m(hash, 31);
    xu_hash_map_test_get_m2m(hash, 32);

    // del
    xu_hash_map_test_remove_m2m(hash, 10);
    xu_hash_map_test_remove_m2m(hash, 11);
    xu_hash_map_test_remove_m2m(hash, 12);
    xu_hash_map_test_remove_m2m(hash, 13);
    xu_hash_map_test_remove_m2m(hash, 14);
    xu_hash_map_test_remove_m2m(hash, 15);
    xu_hash_map_test_remove_m2m(hash, 16);
    xu_hash_map_test_remove_m2m(hash, 17);
    xu_hash_map_test_remove_m2m(hash, 18);
    xu_hash_map_test_remove_m2m(hash, 19);
    xu_hash_map_test_remove_m2m(hash, 20);
    xu_hash_map_test_remove_m2m(hash, 21);
    xu_hash_map_test_remove_m2m(hash, 22);
    xu_hash_map_test_remove_m2m(hash, 23);
    xu_hash_map_test_remove_m2m(hash, 24);
    xu_hash_map_test_remove_m2m(hash, 25);
    xu_hash_map_test_remove_m2m(hash, 26);
    xu_hash_map_test_remove_m2m(hash, 27);
    xu_hash_map_test_remove_m2m(hash, 28);
    xu_hash_map_test_remove_m2m(hash, 29);
    xu_hash_map_test_remove_m2m(hash, 30);
    xu_hash_map_test_remove_m2m(hash, 31);
    xu_hash_map_test_remove_m2m(hash, 32);
    xu_hash_map_test_dump(hash);

    // clear
    xu_hash_map_clear(hash);
    xu_hash_map_test_dump(hash);

    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_m2m_perf()
{
    // init hash: mem => mem
    xu_size_t const   step = 12;
    xu_byte_t         item[step];
    xu_hash_map_ref_t hash =
        xu_hash_map_init(0, xu_element_mem(step, xu_null, xu_null), xu_element_mem(step, xu_null, xu_null));
    xu_assert_and_check_return(hash);

    // performance
    __xu_volatile__ xu_size_t n = 100000;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        xu_uint32_t i = (xu_uint32_t)xu_random_value();
        xu_hash_map_test_insert_m2m(hash, i);
        xu_hash_map_test_get_m2m(hash, i);
    }
    t = xu_mclock() - t;
    xu_trace_i("m2m: time: %lld", t);

    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_i2i_func()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(8, xu_element_long(), xu_element_long());
    xu_assert_and_check_return(hash);

    // set
    xu_hash_map_test_insert_i2i(hash, 0);
    xu_hash_map_test_insert_i2i(hash, 1);
    xu_hash_map_test_insert_i2i(hash, 12);
    xu_hash_map_test_insert_i2i(hash, 123);
    xu_hash_map_test_insert_i2i(hash, 1234);
    xu_hash_map_test_insert_i2i(hash, 12345);
    xu_hash_map_test_insert_i2i(hash, 123456);
    xu_hash_map_test_insert_i2i(hash, 1234567);
    xu_hash_map_test_insert_i2i(hash, 12345678);
    xu_hash_map_test_insert_i2i(hash, 123456789);
    xu_hash_map_test_insert_i2i(hash, 876543210);
    xu_hash_map_test_insert_i2i(hash, 76543210);
    xu_hash_map_test_insert_i2i(hash, 6543210);
    xu_hash_map_test_insert_i2i(hash, 543210);
    xu_hash_map_test_insert_i2i(hash, 43210);
    xu_hash_map_test_insert_i2i(hash, 3210);
    xu_hash_map_test_insert_i2i(hash, 210);
    xu_hash_map_test_insert_i2i(hash, 10);
    xu_hash_map_test_insert_i2i(hash, 0);
    xu_hash_map_test_dump(hash);

    // get
    xu_hash_map_test_get_i2i(hash, 0);
    xu_hash_map_test_get_i2i(hash, 1);
    xu_hash_map_test_get_i2i(hash, 12);
    xu_hash_map_test_get_i2i(hash, 123);
    xu_hash_map_test_get_i2i(hash, 1234);
    xu_hash_map_test_get_i2i(hash, 12345);
    xu_hash_map_test_get_i2i(hash, 123456);
    xu_hash_map_test_get_i2i(hash, 1234567);
    xu_hash_map_test_get_i2i(hash, 12345678);
    xu_hash_map_test_get_i2i(hash, 123456789);
    xu_hash_map_test_get_i2i(hash, 876543210);
    xu_hash_map_test_get_i2i(hash, 76543210);
    xu_hash_map_test_get_i2i(hash, 6543210);
    xu_hash_map_test_get_i2i(hash, 543210);
    xu_hash_map_test_get_i2i(hash, 43210);
    xu_hash_map_test_get_i2i(hash, 3210);
    xu_hash_map_test_get_i2i(hash, 210);
    xu_hash_map_test_get_i2i(hash, 10);
    xu_hash_map_test_get_i2i(hash, 0);

    // del
    xu_hash_map_test_remove_i2i(hash, 0);
    xu_hash_map_test_remove_i2i(hash, 1);
    xu_hash_map_test_remove_i2i(hash, 12);
    xu_hash_map_test_remove_i2i(hash, 123);
    xu_hash_map_test_remove_i2i(hash, 1234);
    xu_hash_map_test_remove_i2i(hash, 12345);
    xu_hash_map_test_remove_i2i(hash, 123456);
    xu_hash_map_test_remove_i2i(hash, 1234567);
    xu_hash_map_test_remove_i2i(hash, 12345678);
    xu_hash_map_test_remove_i2i(hash, 123456789);
    xu_hash_map_test_remove_i2i(hash, 123456789);
    xu_hash_map_test_dump(hash);

    // clear
    xu_hash_map_clear(hash);
    xu_hash_map_test_dump(hash);

    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_i2i_perf()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(0, xu_element_long(), xu_element_long());
    xu_assert_and_check_return(hash);

    // performance
    __xu_volatile__ xu_size_t n = 100000;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        xu_size_t i = xu_random_value();
        xu_hash_map_test_insert_i2i(hash, i);
        xu_hash_map_test_get_i2i(hash, i);
    }
    t = xu_mclock() - t;
    xu_trace_i("i2i: time: %lld", t);

    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_i2t_func()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(8, xu_element_long(), xu_element_true());
    xu_assert_and_check_return(hash);

    // set
    xu_hash_map_test_insert_i2t(hash, 0);
    xu_hash_map_test_insert_i2t(hash, 1);
    xu_hash_map_test_insert_i2t(hash, 12);
    xu_hash_map_test_insert_i2t(hash, 123);
    xu_hash_map_test_insert_i2t(hash, 1234);
    xu_hash_map_test_insert_i2t(hash, 12345);
    xu_hash_map_test_insert_i2t(hash, 123456);
    xu_hash_map_test_insert_i2t(hash, 1234567);
    xu_hash_map_test_insert_i2t(hash, 12345678);
    xu_hash_map_test_insert_i2t(hash, 123456789);
    xu_hash_map_test_insert_i2t(hash, 876543210);
    xu_hash_map_test_insert_i2t(hash, 76543210);
    xu_hash_map_test_insert_i2t(hash, 6543210);
    xu_hash_map_test_insert_i2t(hash, 543210);
    xu_hash_map_test_insert_i2t(hash, 43210);
    xu_hash_map_test_insert_i2t(hash, 3210);
    xu_hash_map_test_insert_i2t(hash, 210);
    xu_hash_map_test_insert_i2t(hash, 10);
    xu_hash_map_test_insert_i2t(hash, 0);
    xu_hash_map_test_dump(hash);

    // get
    xu_hash_map_test_get_i2t(hash, 0);
    xu_hash_map_test_get_i2t(hash, 1);
    xu_hash_map_test_get_i2t(hash, 12);
    xu_hash_map_test_get_i2t(hash, 123);
    xu_hash_map_test_get_i2t(hash, 1234);
    xu_hash_map_test_get_i2t(hash, 12345);
    xu_hash_map_test_get_i2t(hash, 123456);
    xu_hash_map_test_get_i2t(hash, 1234567);
    xu_hash_map_test_get_i2t(hash, 12345678);
    xu_hash_map_test_get_i2t(hash, 123456789);
    xu_hash_map_test_get_i2t(hash, 876543210);
    xu_hash_map_test_get_i2t(hash, 76543210);
    xu_hash_map_test_get_i2t(hash, 6543210);
    xu_hash_map_test_get_i2t(hash, 543210);
    xu_hash_map_test_get_i2t(hash, 43210);
    xu_hash_map_test_get_i2t(hash, 3210);
    xu_hash_map_test_get_i2t(hash, 210);
    xu_hash_map_test_get_i2t(hash, 10);
    xu_hash_map_test_get_i2t(hash, 0);

    // del
    xu_hash_map_test_remove_i2t(hash, 0);
    xu_hash_map_test_remove_i2t(hash, 1);
    xu_hash_map_test_remove_i2t(hash, 12);
    xu_hash_map_test_remove_i2t(hash, 123);
    xu_hash_map_test_remove_i2t(hash, 1234);
    xu_hash_map_test_remove_i2t(hash, 12345);
    xu_hash_map_test_remove_i2t(hash, 123456);
    xu_hash_map_test_remove_i2t(hash, 1234567);
    xu_hash_map_test_remove_i2t(hash, 12345678);
    xu_hash_map_test_remove_i2t(hash, 123456789);
    xu_hash_map_test_remove_i2t(hash, 123456789);
    xu_hash_map_test_dump(hash);

    // clear
    xu_hash_map_clear(hash);
    xu_hash_map_test_dump(hash);

    // exit
    xu_hash_map_exit(hash);
}
static xu_void_t xu_hash_map_test_i2t_perf()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(0, xu_element_long(), xu_element_true());
    xu_assert_and_check_return(hash);

    // done
    __xu_volatile__ xu_size_t n = 100000;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        xu_size_t i = xu_random_value();
        xu_hash_map_test_insert_i2t(hash, i);
        xu_hash_map_test_get_i2t(hash, i);
    }
    t = xu_mclock() - t;
    xu_trace_i("i2t: time: %lld", t);

    // exit hash
    xu_hash_map_exit(hash);
}
static xu_bool_t xu_hash_map_test_walk_item(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // done
    xu_bool_t              ok        = xu_false;
    xu_hize_t*             test      = (xu_hize_t*)value;
    xu_hash_map_item_ref_t hash_item = (xu_hash_map_item_ref_t)item;
    if (hash_item)
    {
        if (!(((xu_size_t)hash_item->data >> 25) & 0x1))
            ok = xu_true;
        else
        {
            test[0] += (xu_size_t)hash_item->name;
            test[1] += (xu_size_t)hash_item->data;
            test[2]++;
        }
    }

    // ok?
    return ok;
}
static xu_void_t xu_hash_map_test_walk_perf()
{
    // init hash
    xu_hash_map_ref_t hash = xu_hash_map_init(0, xu_element_long(), xu_element_long());
    xu_assert_and_check_return(hash);

    // reset random
    xu_random_reset(xu_true);

    // add items
    __xu_volatile__ xu_size_t n = 100000;
    while (n--)
    {
        xu_size_t i = xu_random_value();
        xu_hash_map_test_insert_i2i(hash, i);
        xu_hash_map_test_get_i2i(hash, i);
    }

    // done
    xu_hong_t                 t       = xu_mclock();
    __xu_volatile__ xu_hize_t test[3] = {0};
    xu_remove_if(hash, xu_hash_map_test_walk_item, (xu_cpointer_t)test);
    t = xu_mclock() - t;
    xu_trace_i("name: %llx, data: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], test[2],
               xu_hash_map_size(hash), t);

    // exit
    xu_hash_map_exit(hash);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_hash_map_main(xu_int_t argc, xu_char_t** argv)
{
#if 1
    xu_hash_map_test_s2i_func();
    xu_hash_map_test_i2s_func();
    xu_hash_map_test_m2m_func();
    xu_hash_map_test_i2i_func();
    xu_hash_map_test_i2t_func();
#endif

#if 1
    xu_hash_map_test_s2i_perf();
    xu_hash_map_test_i2s_perf();
    xu_hash_map_test_m2m_perf();
    xu_hash_map_test_i2i_perf();
    xu_hash_map_test_i2t_perf();
#endif

#if 1
    xu_hash_map_test_walk_perf();
#endif

    return 0;
}
