#include "xutil/unit_test/ut.h"
#include <stdlib.h>

/* *******************************************************
 * demo
 */
xu_void_t xu_demo_fixed_pool_leak(xu_noarg_t);
xu_void_t xu_demo_fixed_pool_leak()
{
    // done
    xu_fixed_pool_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_fixed_pool_init(xu_null, 0, sizeof(xu_size_t), xu_null, xu_null, xu_null);
        xu_assert_and_check_break(pool);

        // make data0
        xu_pointer_t data0 = xu_fixed_pool_malloc(pool);
        xu_assert_and_check_break(data0);

        // make data1
        xu_pointer_t data1 = xu_fixed_pool_malloc(pool);
        xu_assert_and_check_break(data1);

#ifdef __xu_debug__
        // dump pool
        xu_fixed_pool_dump(pool);
#endif

    } while (0);

    // exit pool
    if (pool) xu_fixed_pool_exit(pool);
}
xu_void_t xu_demo_fixed_pool_free2(xu_noarg_t);
xu_void_t xu_demo_fixed_pool_free2()
{
    // done
    xu_fixed_pool_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_fixed_pool_init(xu_null, 0, sizeof(xu_size_t), xu_null, xu_null, xu_null);
        xu_assert_and_check_break(pool);

        // make data
        xu_pointer_t data = xu_fixed_pool_malloc(pool);
        xu_assert_and_check_break(data);

        // exit data
        xu_fixed_pool_free(pool, data);
        xu_fixed_pool_free(pool, data);

#ifdef __xu_debug__
        // dump pool
        xu_fixed_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) xu_fixed_pool_exit(pool);
}
xu_void_t xu_demo_fixed_pool_underflow(xu_noarg_t);
xu_void_t xu_demo_fixed_pool_underflow()
{
    // done
    xu_fixed_pool_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_fixed_pool_init(xu_null, 0, sizeof(xu_size_t), xu_null, xu_null, xu_null);
        xu_assert_and_check_break(pool);

        // make data
        xu_pointer_t data = xu_fixed_pool_malloc(pool);
        xu_assert_and_check_break(data);

        // done underflow
        xu_memset(data, 0, sizeof(xu_size_t) + 1);

        // exit data
        xu_fixed_pool_free(pool, data);

#ifdef __xu_debug__
        // dump pool
        xu_fixed_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) xu_fixed_pool_exit(pool);
}
xu_void_t xu_demo_fixed_pool_underflow2(xu_noarg_t);
xu_void_t xu_demo_fixed_pool_underflow2()
{
    // done
    xu_fixed_pool_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_fixed_pool_init(xu_null, 0, sizeof(xu_size_t), xu_null, xu_null, xu_null);
        xu_assert_and_check_break(pool);

        // make data
        xu_pointer_t data = xu_fixed_pool_malloc(pool);
        xu_assert_and_check_break(data);

        // done underflow
        xu_memset(data, 0, sizeof(xu_size_t) + 1);

        // make data2
        data = xu_fixed_pool_malloc(pool);
        xu_assert_and_check_break(data);

#ifdef __xu_debug__
        // dump pool
        xu_fixed_pool_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) xu_fixed_pool_exit(pool);
}
xu_void_t xu_demo_fixed_pool_perf(xu_size_t item_size);
xu_void_t xu_demo_fixed_pool_perf(xu_size_t item_size)
{
    // done
    xu_fixed_pool_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_fixed_pool_init(xu_null, 0, item_size, xu_null, xu_null, xu_null);
        xu_assert_and_check_break(pool);

        // make data list
        xu_size_t     maxn = 10000;
        xu_pointer_t* list = (xu_pointer_t*)calloc(maxn, sizeof(xu_pointer_t));
        xu_assert_and_check_break(list);

        // done
        __xu_volatile__ xu_size_t indx = 0;
        __xu_volatile__ xu_hong_t time = xu_mclock();
        __xu_volatile__ xu_size_t rand = 0xbeaf;
        for (indx = 0; indx < maxn; indx++)
        {
            // make data
            list[indx] = xu_fixed_pool_malloc(pool);
            xu_assert_and_check_break(list[indx]);

            // make rand
            rand = (rand * 10807 + 1) & 0xffffffff;

            // free data
            __xu_volatile__ xu_size_t size = rand & 15;
            if (size > 5 && indx)
            {
                size -= 5;
                while (size--)
                {
                    // the free index
                    xu_size_t free_indx = rand % indx;

                    // free it
                    if (list[free_indx]) xu_fixed_pool_free(pool, list[free_indx]);
                    list[free_indx] = xu_null;
                }
            }
        }
        time = xu_mclock() - time;

#ifdef __xu_debug__
        // dump pool
        xu_fixed_pool_dump(pool);
#endif

        // trace
        xu_trace_i("time: %lld ms", time);

        // clear pool
        xu_fixed_pool_clear(pool);

    } while (0);

    // exit pool
    if (pool) xu_fixed_pool_exit(pool);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_memory_fixed_pool_main(xu_int_t argc, xu_char_t** argv)
{
#if 1
    xu_demo_fixed_pool_perf(16);
    xu_demo_fixed_pool_perf(32);
    xu_demo_fixed_pool_perf(64);
    xu_demo_fixed_pool_perf(96);
    xu_demo_fixed_pool_perf(128);
    xu_demo_fixed_pool_perf(192);
    xu_demo_fixed_pool_perf(256);
    xu_demo_fixed_pool_perf(384);
    xu_demo_fixed_pool_perf(512);
    xu_demo_fixed_pool_perf(1024);
    xu_demo_fixed_pool_perf(2048);
    xu_demo_fixed_pool_perf(3072);
#endif

#if 0
    xu_demo_fixed_pool_leak();
#endif

#if 0
    xu_demo_fixed_pool_free2();
#endif

#if 0
    xu_demo_fixed_pool_underflow();
#endif

#if 0
    xu_demo_fixed_pool_underflow2();
#endif

    return 0;
}
