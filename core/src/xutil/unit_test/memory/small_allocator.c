#include "xutil/unit_test/ut.h"

/* *******************************************************
 * demo
 */
xu_void_t xu_demo_small_allocator_leak(xu_noarg_t);
xu_void_t xu_demo_small_allocator_leak()
{
    // done
    xu_allocator_ref_t small_allocator = xu_null;
    do
    {
        // init small allocator
        small_allocator = xu_small_allocator_init(xu_null);
        xu_assert_and_check_break(small_allocator);

        // make data0
        xu_pointer_t data0 = xu_allocator_malloc(small_allocator, 10);
        xu_assert_and_check_break(data0);

        // make data1
        xu_pointer_t data1 = xu_allocator_malloc(small_allocator, 10);
        xu_assert_and_check_break(data1);

#ifdef __xu_debug__
        // dump small_allocator
        xu_allocator_dump(small_allocator);
#endif

    } while (0);

    // exit small allocator
    if (small_allocator) xu_allocator_exit(small_allocator);
    small_allocator = xu_null;
}
xu_void_t xu_demo_small_allocator_free2(xu_noarg_t);
xu_void_t xu_demo_small_allocator_free2()
{
    // done
    xu_allocator_ref_t small_allocator = xu_null;
    do
    {
        // init small allocator
        small_allocator = xu_small_allocator_init(xu_null);
        xu_assert_and_check_break(small_allocator);

        // make data
        xu_pointer_t data = xu_allocator_malloc(small_allocator, 10);
        xu_assert_and_check_break(data);

        // exit data
        xu_allocator_free(small_allocator, data);
        xu_allocator_free(small_allocator, data);

#ifdef __xu_debug__
        // dump small_allocator
        xu_allocator_dump(small_allocator);
#endif
    } while (0);

    // exit small allocator
    if (small_allocator) xu_allocator_exit(small_allocator);
    small_allocator = xu_null;
}
xu_void_t xu_demo_small_allocator_underflow(xu_noarg_t);
xu_void_t xu_demo_small_allocator_underflow()
{
    // done
    xu_allocator_ref_t small_allocator = xu_null;
    do
    {
        // init small allocator
        small_allocator = xu_small_allocator_init(xu_null);
        xu_assert_and_check_break(small_allocator);

        // make data
        xu_pointer_t data = xu_allocator_malloc(small_allocator, 10);
        xu_assert_and_check_break(data);

        // done underflow
        xu_memset(data, 0, 10 + 1);

        // exit data
        xu_allocator_free(small_allocator, data);

#ifdef __xu_debug__
        // dump small_allocator
        xu_allocator_dump(small_allocator);
#endif
    } while (0);

    // exit small allocator
    if (small_allocator) xu_allocator_exit(small_allocator);
    small_allocator = xu_null;
}
xu_void_t xu_demo_small_allocator_underflow2(xu_noarg_t);
xu_void_t xu_demo_small_allocator_underflow2()
{
    // done
    xu_allocator_ref_t small_allocator = xu_null;
    do
    {
        // init small allocator
        small_allocator = xu_small_allocator_init(xu_null);
        xu_assert_and_check_break(small_allocator);

        // make data
        xu_pointer_t data = xu_allocator_malloc(small_allocator, 10);
        xu_assert_and_check_break(data);

        // done underflow
        xu_memset(data, 0, 10 + 1);

        // make data2
        data = xu_allocator_malloc(small_allocator, 10);
        xu_assert_and_check_break(data);

#ifdef __xu_debug__
        // dump small_allocator
        xu_allocator_dump(small_allocator);
#endif
    } while (0);

    // exit small allocator
    if (small_allocator) xu_allocator_exit(small_allocator);
    small_allocator = xu_null;
}
xu_void_t xu_demo_small_allocator_perf(xu_noarg_t);
xu_void_t xu_demo_small_allocator_perf()
{
    // done
    xu_allocator_ref_t small_allocator = xu_null;
    xu_allocator_ref_t large_allocator = xu_null;
    do
    {
        // init small allocator
        small_allocator = xu_small_allocator_init(xu_null);
        xu_assert_and_check_break(small_allocator);

        // init large allocator
        large_allocator = xu_large_allocator_init(xu_null, 0);
        xu_assert_and_check_break(large_allocator);

        // make data list
        xu_size_t     maxn = 100000;
        xu_pointer_t* list =
            (xu_pointer_t*)xu_allocator_large_nalloc0(large_allocator, maxn, sizeof(xu_pointer_t), xu_null);
        xu_assert_and_check_break(list);

        // done
        __xu_volatile__ xu_size_t indx = 0;
        __xu_volatile__ xu_hong_t time = xu_mclock();
        __xu_volatile__ xu_size_t rand = 0xbeaf;
        for (indx = 0; indx < maxn; indx++)
        {
            // make data
            list[indx] = xu_allocator_malloc0(small_allocator, (rand & 3071) + 1);
            xu_assert_and_check_break(list[indx]);

            // make rand
            rand = (rand * 10807 + 1) & 0xffffffff;

            // re-make data
            if (!(indx & 31))
            {
                list[indx] = xu_allocator_ralloc(small_allocator, list[indx], (rand & 3071) + 1);
                xu_assert_and_check_break(list[indx]);
            }

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
                    if (list[free_indx]) xu_allocator_free(small_allocator, list[free_indx]);
                    list[free_indx] = xu_null;
                }
            }
        }
        time = xu_mclock() - time;

#ifdef __xu_debug__
        // dump small_allocator
        xu_allocator_dump(small_allocator);
#endif

        // trace
        xu_trace_i("time: %lld ms", time);

        // clear small_allocator
        xu_allocator_clear(small_allocator);

        // exit list
        xu_allocator_large_free(large_allocator, list);

    } while (0);

    // exit small allocator
    if (small_allocator) xu_allocator_exit(small_allocator);
    small_allocator = xu_null;

    // exit large allocator
    if (large_allocator) xu_allocator_exit(large_allocator);
    large_allocator = xu_null;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_memory_small_allocator_main(xu_int_t argc, xu_char_t** argv)
{
#if 1
    xu_demo_small_allocator_perf();
#endif

#if 0
    xu_demo_small_allocator_leak();
#endif

#if 0
    xu_demo_small_allocator_free2();
#endif

#if 0
    xu_demo_small_allocator_underflow();
#endif

#if 0
    xu_demo_small_allocator_underflow2();
#endif

    return 0;
}
