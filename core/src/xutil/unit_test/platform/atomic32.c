#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_void_t xu_test_atomic32_get()
{
    xu_trace_i("atomic32_get(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 0);
    xu_assert_and_check_return(xu_atomic32_get(&a) == 0);
    xu_atomic32_init(&a, 1);
    xu_assert_and_check_return(xu_atomic32_get(&a) == 1);
    xu_atomic32_init(&a, 1000);
    xu_assert_and_check_return(xu_atomic32_get(&a) == 1000);
    xu_trace_i("atomic32_get(): test ok!");
}
static xu_void_t xu_test_atomic32_fetch_and_set()
{
    xu_trace_i("atomic32_fetch_and_set(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_set(&a, 1) == 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_set(&a, 2) == 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_set(&a, 1000) == 2);
    xu_trace_i("atomic32_fetch_and_set(): test ok!");
}
static xu_void_t xu_test_atomic32_fetch_and_cmpset()
{
    xu_trace_i("atomic32_fetch_and_cmpset(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_cmpset(&a, 0, 1) == 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_cmpset(&a, 1, 2) == 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_cmpset(&a, 10, 1000) == 2);
    xu_assert_and_check_return(xu_atomic32_fetch_and_cmpset(&a, 10, 1000) == 2);
    xu_trace_i("atomic32_fetch_and_cmpset(): test ok!");
}
static xu_void_t xu_test_atomic32_compare_and_swap()
{
    xu_trace_i("atomic32_compare_and_swap(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 0);
    xu_int32_t p = 0;
    xu_assert_and_check_return(xu_atomic32_compare_and_swap(&a, &p, 1) && p == 0);
    p = 1;
    xu_assert_and_check_return(xu_atomic32_compare_and_swap(&a, &p, 2) && p == 1);
    p = 10;
    xu_assert_and_check_return(!xu_atomic32_compare_and_swap(&a, &p, 1000) && p == 2);
    xu_trace_i("atomic32_compare_and_swap(): test ok!");
}
static xu_void_t xu_test_atomic32_fetch_and_add()
{
    xu_trace_i("atomic32_fetch_and_add(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_add(&a, 1) == 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_add(&a, 2) == 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_add(&a, 1000) == 3);
    xu_assert_and_check_return(xu_atomic32_fetch_and_add(&a, 1000) == 1003);
    xu_trace_i("atomic32_fetch_and_add(): test ok!");
}
static xu_void_t xu_test_atomic32_fetch_and_sub()
{
    xu_trace_i("atomic32_fetch_and_sub(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_sub(&a, 1) == 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_sub(&a, 2) == -1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_sub(&a, 1000) == -3);
    xu_assert_and_check_return(xu_atomic32_fetch_and_sub(&a, 1000) == -1003);
    xu_trace_i("atomic32_fetch_and_sub(): test ok!");
}
static xu_void_t xu_test_atomic32_fetch_and_and()
{
    xu_trace_i("atomic32_fetch_and_and(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_and(&a, 1) == 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_and(&a, 2) == 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_and(&a, 0xff) == 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_and(&a, 1000) == 0);
    xu_trace_i("atomic32_fetch_and_and(): test ok!");
}
static xu_void_t xu_test_atomic32_fetch_and_xor()
{
    xu_trace_i("atomic32_fetch_and_xor(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_xor(&a, 1) == 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_xor(&a, 2) == 0);
    xu_assert_and_check_return(xu_atomic32_fetch_and_xor(&a, 3) == 2);
    xu_assert_and_check_return(xu_atomic32_fetch_and_xor(&a, 1000) == 1);
    xu_trace_i("atomic32_fetch_and_xor(): test ok!");
}
static xu_void_t xu_test_atomic32_fetch_and_or()
{
    xu_trace_i("atomic32_fetch_and_or(): test ..");
    xu_atomic32_t a;
    xu_atomic32_init(&a, 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_or(&a, 1) == 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_or(&a, 2) == 1);
    xu_assert_and_check_return(xu_atomic32_fetch_and_or(&a, 3) == 3);
    xu_assert_and_check_return(xu_atomic32_fetch_and_or(&a, 1000) == 3);
    xu_trace_i("atomic32_fetch_and_or(): test ok!");
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_atomic32_main(xu_int_t argc, xu_char_t** argv)
{
    xu_test_atomic32_get();
    xu_test_atomic32_compare_and_swap();
    xu_test_atomic32_fetch_and_set();
    xu_test_atomic32_fetch_and_cmpset();
    xu_test_atomic32_fetch_and_add();
    xu_test_atomic32_fetch_and_sub();
    xu_test_atomic32_fetch_and_and();
    xu_test_atomic32_fetch_and_xor();
    xu_test_atomic32_fetch_and_or();
    return 0;
}
