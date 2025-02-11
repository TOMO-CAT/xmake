#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_void_t xu_queue_put_and_pop_test()
{
    // init
    xu_queue_ref_t queue = xu_queue_init(10, xu_element_long());
    xu_assert_and_check_return(queue);

    // make queue
    xu_queue_put(queue, (xu_pointer_t)0);
    xu_queue_put(queue, (xu_pointer_t)1);
    xu_queue_put(queue, (xu_pointer_t)2);
    xu_queue_put(queue, (xu_pointer_t)3);
    xu_queue_put(queue, (xu_pointer_t)4);
    xu_queue_put(queue, (xu_pointer_t)5);
    xu_queue_put(queue, (xu_pointer_t)6);
    xu_queue_put(queue, (xu_pointer_t)7);
    xu_queue_put(queue, (xu_pointer_t)8);
    xu_queue_put(queue, (xu_pointer_t)9);

    // done
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_hong_t                 t = xu_mclock();
    for (i = 0; i < n; i++)
    {
        xu_queue_pop(queue);
        xu_queue_put(queue, (xu_pointer_t)0xf);
    }
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_queue_put_and_pop(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_queue_size(queue),
               xu_queue_maxn(queue));

    // check
    xu_assert(xu_queue_size(queue) == 10);
    xu_assert(xu_queue_head(queue) == (xu_pointer_t)0xf);
    xu_assert(xu_queue_last(queue) == (xu_pointer_t)0xf);

    // clear it
    xu_queue_clear(queue);
    xu_assert(!xu_queue_size(queue));

    // exit
    xu_queue_exit(queue);
}
static xu_void_t xu_queue_iterator_next_test()
{
    // init
    xu_queue_ref_t queue = xu_queue_init(0, xu_element_long());
    xu_assert_and_check_return(queue);

    // make queue
    xu_size_t n = 1000000;
    while (n--)
        xu_queue_put(queue, (xu_pointer_t)0xf);

    // done
    xu_hong_t t = xu_mclock();
    xu_for_all(xu_char_t*, item, queue) xu_used(item);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_queue_iterator_next(%lu): %lld ms, size: %lu, maxn: %lu", 1000000, t, xu_queue_size(queue),
               xu_queue_maxn(queue));

    // exit
    xu_queue_exit(queue);
}
static xu_void_t xu_queue_int_dump(xu_queue_ref_t queue)
{
    xu_trace_i("xu_int_t size: %lu, maxn: %lu", xu_queue_size(queue), xu_queue_maxn(queue));
    xu_for_all(xu_char_t*, item, queue) { xu_trace_i("xu_int_t at[%lu]: %u", item_itor, item); }
}
static xu_void_t xu_queue_int_test()
{
    xu_queue_ref_t queue = xu_queue_init(10, xu_element_long());
    xu_assert_and_check_return(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("put:");
    xu_queue_put(queue, (xu_pointer_t)0);
    xu_queue_put(queue, (xu_pointer_t)1);
    xu_queue_put(queue, (xu_pointer_t)2);
    xu_queue_put(queue, (xu_pointer_t)3);
    xu_queue_put(queue, (xu_pointer_t)4);
    xu_queue_put(queue, (xu_pointer_t)5);
    xu_queue_put(queue, (xu_pointer_t)6);
    xu_queue_put(queue, (xu_pointer_t)7);
    xu_queue_put(queue, (xu_pointer_t)8);
    xu_queue_put(queue, (xu_pointer_t)9);
    xu_queue_int_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("pop:");
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_int_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("put:");
    xu_queue_put(queue, (xu_pointer_t)0);
    xu_queue_put(queue, (xu_pointer_t)1);
    xu_queue_put(queue, (xu_pointer_t)2);
    xu_queue_put(queue, (xu_pointer_t)3);
    xu_queue_put(queue, (xu_pointer_t)4);
    xu_queue_int_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("clear:");
    xu_queue_clear(queue);
    xu_queue_int_dump(queue);
    xu_queue_exit(queue);
}
static xu_void_t xu_queue_str_dump(xu_queue_ref_t queue)
{
    xu_trace_i("str size: %lu, maxn: %lu", xu_queue_size(queue), xu_queue_maxn(queue));
    xu_for_all(xu_char_t*, item, queue) { xu_trace_i("str at[%lu]: %s", item_itor, item); }
}
static xu_void_t xu_queue_str_test()
{
    xu_queue_ref_t queue = xu_queue_init(10, xu_element_str(xu_true));
    xu_assert_and_check_return(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("put:");
    xu_queue_put(queue, "0000000000");
    xu_queue_put(queue, "1111111111");
    xu_queue_put(queue, "2222222222");
    xu_queue_put(queue, "3333333333");
    xu_queue_put(queue, "4444444444");
    xu_queue_put(queue, "5555555555");
    xu_queue_put(queue, "6666666666");
    xu_queue_put(queue, "7777777777");
    xu_queue_put(queue, "8888888888");
    xu_queue_put(queue, "9999999999");
    xu_queue_str_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("pop:");
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_str_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("put:");
    xu_queue_put(queue, "0000000000");
    xu_queue_put(queue, "1111111111");
    xu_queue_put(queue, "2222222222");
    xu_queue_put(queue, "3333333333");
    xu_queue_put(queue, "4444444444");
    xu_queue_str_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("clear:");
    xu_queue_clear(queue);
    xu_queue_str_dump(queue);
    xu_queue_exit(queue);
}
static xu_void_t xu_queue_mem_free(xu_element_ref_t element, xu_pointer_t buff)
{
    xu_trace_i("ifm free: %s, priv: %s", buff, element->priv);
}
static xu_void_t xu_queue_mem_dump(xu_queue_ref_t queue)
{
    xu_trace_i("ifm size: %lu, maxn: %lu", xu_queue_size(queue), xu_queue_maxn(queue));
    xu_for_all(xu_char_t*, item, queue) { xu_trace_i("ifm at[%lu]: %s", item_itor, item); }
}
static xu_void_t xu_queue_mem_test()
{
    xu_queue_ref_t queue = xu_queue_init(10, xu_element_mem(11, xu_queue_mem_free, "ifm"));
    xu_assert_and_check_return(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("put:");
    xu_queue_put(queue, "0000000000");
    xu_queue_put(queue, "1111111111");
    xu_queue_put(queue, "2222222222");
    xu_queue_put(queue, "3333333333");
    xu_queue_put(queue, "4444444444");
    xu_queue_put(queue, "5555555555");
    xu_queue_put(queue, "6666666666");
    xu_queue_put(queue, "7777777777");
    xu_queue_put(queue, "8888888888");
    xu_queue_put(queue, "9999999999");
    xu_queue_mem_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("pop:");
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_pop(queue);
    xu_queue_mem_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("put:");
    xu_queue_put(queue, "0000000000");
    xu_queue_put(queue, "1111111111");
    xu_queue_put(queue, "2222222222");
    xu_queue_put(queue, "3333333333");
    xu_queue_put(queue, "4444444444");
    xu_queue_mem_dump(queue);

    xu_trace_i("=============================================================");
    xu_trace_i("clear:");
    xu_queue_clear(queue);
    xu_queue_mem_dump(queue);
    xu_queue_exit(queue);
}
static xu_void_t xu_queue_perf_test()
{
    xu_queue_put_and_pop_test();
    xu_queue_iterator_next_test();
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_queue_main(xu_int_t argc, xu_char_t** argv)
{
    xu_queue_int_test();
    xu_queue_str_test();
    xu_queue_mem_test();
    xu_queue_perf_test();

    return 0;
}
