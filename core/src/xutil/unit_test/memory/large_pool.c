#include "xutil/unit_test/ut.h"
#include <stdlib.h>

/* *******************************************************
 * macros
 */

// init pool
#if 1
#    define xu_demo_init_pool() xu_large_allocator_init((xu_byte_t*)malloc(500 * 1024 * 1024), 500 * 1024 * 1024)
#else
#    define xu_demo_init_pool() xu_large_allocator_init(xu_null, 0)
#endif

/* *******************************************************
 * demo
 */
xu_void_t xu_demo_large_allocator_leak(xu_noarg_t);
xu_void_t xu_demo_large_allocator_leak()
{
    // done
    xu_allocator_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_demo_init_pool();
        xu_assert_and_check_break(pool);

        // make data0
        xu_pointer_t data0 = xu_allocator_large_malloc(pool, 10, xu_null);
        xu_assert_and_check_break(data0);

        // make data1
        xu_pointer_t data1 = xu_allocator_large_malloc(pool, 10, xu_null);
        xu_assert_and_check_break(data1);

#ifdef __xu_debug__
        // dump pool
        xu_allocator_dump(pool);
#endif

    } while (0);

    // exit pool
    if (pool) xu_allocator_exit(pool);
}
xu_void_t xu_demo_large_allocator_free2(xu_noarg_t);
xu_void_t xu_demo_large_allocator_free2()
{
    // done
    xu_allocator_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_demo_init_pool();
        xu_assert_and_check_break(pool);

        // make data
        xu_pointer_t data = xu_allocator_large_malloc(pool, 10, xu_null);
        xu_assert_and_check_break(data);

        // exit data
        xu_allocator_large_free(pool, data);
        xu_allocator_large_free(pool, data);

#ifdef __xu_debug__
        // dump pool
        xu_allocator_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) xu_allocator_exit(pool);
}
xu_void_t xu_demo_large_allocator_underflow(xu_noarg_t);
xu_void_t xu_demo_large_allocator_underflow()
{
    // done
    xu_allocator_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_demo_init_pool();
        xu_assert_and_check_break(pool);

        // make data
        xu_pointer_t data = xu_allocator_large_malloc(pool, 10, xu_null);
        xu_assert_and_check_break(data);

        // done underflow
        xu_memset(data, 0, 10 + 1);

        // exit data
        xu_allocator_large_free(pool, data);

#ifdef __xu_debug__
        // dump pool
        xu_allocator_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) xu_allocator_exit(pool);
}
xu_void_t xu_demo_large_allocator_underflow2(xu_noarg_t);
xu_void_t xu_demo_large_allocator_underflow2()
{
    // done
    xu_allocator_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_demo_init_pool();
        xu_assert_and_check_break(pool);

        // make data
        xu_pointer_t data = xu_allocator_large_malloc(pool, 10, xu_null);
        xu_assert_and_check_break(data);

        // done underflow
        xu_memset(data, 0, 10 + 1);

        // make data2
        data = xu_allocator_large_malloc(pool, 10, xu_null);
        xu_assert_and_check_break(data);

#ifdef __xu_debug__
        // dump pool
        xu_allocator_dump(pool);
#endif
    } while (0);

    // exit pool
    if (pool) xu_allocator_exit(pool);
}
xu_void_t xu_demo_large_allocator_real(xu_size_t size);
xu_void_t xu_demo_large_allocator_real(xu_size_t size)
{
    // done
    xu_allocator_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_demo_init_pool();
        xu_assert_and_check_break(pool);

        // make data
        xu_size_t    real = 0;
        xu_pointer_t data = xu_allocator_large_malloc(pool, size, &real);
        xu_assert_and_check_break(data);

        // trace
        xu_trace_i("size: %lu => real: %lu", size, real);

    } while (0);

    // exit pool
    if (pool) xu_allocator_exit(pool);
}
xu_void_t xu_demo_large_allocator_perf(xu_noarg_t);
xu_void_t xu_demo_large_allocator_perf()
{
    // done
    xu_allocator_ref_t pool = xu_null;
    do
    {
        // init pool
        pool = xu_demo_init_pool();
        xu_assert_and_check_break(pool);

        // make data list
        xu_size_t     maxn = 10000;
        xu_pointer_t* list = (xu_pointer_t*)xu_allocator_large_nalloc0(pool, maxn, sizeof(xu_pointer_t), xu_null);
        xu_assert_and_check_break(list);

        // done
        __xu_volatile__ xu_size_t indx     = 0;
        __xu_volatile__ xu_size_t pagesize = xu_page_size();
        __xu_volatile__ xu_hong_t time     = xu_mclock();
        for (indx = 0; indx < maxn; indx++)
        {
            // make data
            list[indx] = xu_allocator_large_malloc0(pool, xu_random_range(1, pagesize << 4), xu_null);
            xu_assert_and_check_break(list[indx]);

            // re-make data
            if (!(indx & 31))
            {
                list[indx] = xu_allocator_large_ralloc(pool, list[indx], xu_random_range(1, pagesize << 4), xu_null);
                xu_assert_and_check_break(list[indx]);
            }

            // free data
            __xu_volatile__ xu_size_t size = xu_random_range(0, 10);
            if (size > 5 && indx)
            {
                size -= 5;
                while (size--)
                {
                    // the free index
                    xu_size_t free_indx = xu_random_range(0, indx);

                    // free it
                    if (list[free_indx]) xu_allocator_large_free(pool, list[free_indx]);
                    list[free_indx] = xu_null;
                }
            }
        }
        time = xu_mclock() - time;

#ifdef __xu_debug__
        // dump pool
        xu_allocator_dump(pool);
#endif

        // trace
        xu_trace_i("time: %lld ms", time);

        // clear pool
        xu_allocator_clear(pool);

    } while (0);

    // exit pool
    if (pool) xu_allocator_exit(pool);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_memory_large_allocator_main(xu_int_t argc, xu_char_t** argv)
{
#if 1
    xu_demo_large_allocator_perf();
#endif

#if 0
    xu_demo_large_allocator_leak();
#endif

#if 0
    xu_demo_large_allocator_free2();
#endif

#if 0
    xu_demo_large_allocator_underflow();
#endif

#if 0
    xu_demo_large_allocator_underflow2();
#endif

#if 1
    xu_demo_large_allocator_real(16 * 256);
    xu_demo_large_allocator_real(32 * 256);
    xu_demo_large_allocator_real(64 * 256);
    xu_demo_large_allocator_real(96 * 256);
    xu_demo_large_allocator_real(128 * 256);
    xu_demo_large_allocator_real(192 * 256);
    xu_demo_large_allocator_real(256 * 256);
    xu_demo_large_allocator_real(384 * 256);
    xu_demo_large_allocator_real(512 * 256);
    xu_demo_large_allocator_real(1024 * 256);
    xu_demo_large_allocator_real(2048 * 256);
    xu_demo_large_allocator_real(3072 * 256);
#endif

    return 0;
}
