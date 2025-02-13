#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_size_t xu_stack_put_and_pop_test()
{
    // init
    xu_stack_ref_t stack = xu_stack_init(10, xu_element_long());
    xu_assert_and_check_return_val(stack, 0);

    xu_stack_put(stack, (xu_pointer_t)0);
    xu_stack_put(stack, (xu_pointer_t)1);
    xu_stack_put(stack, (xu_pointer_t)2);
    xu_stack_put(stack, (xu_pointer_t)3);
    xu_stack_put(stack, (xu_pointer_t)4);
    xu_stack_put(stack, (xu_pointer_t)5);
    xu_stack_put(stack, (xu_pointer_t)6);
    xu_stack_put(stack, (xu_pointer_t)7);
    xu_stack_put(stack, (xu_pointer_t)8);
    xu_stack_put(stack, (xu_pointer_t)9);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_hong_t                 t = xu_mclock();
    for (i = 0; i < n; i++)
    {
        xu_stack_pop(stack);
        xu_stack_put(stack, (xu_pointer_t)0xf);
    }
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_stack_put_and_pop(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_stack_size(stack),
               xu_stack_maxn(stack));

    // check
    xu_assert(xu_stack_size(stack) == 10);
    xu_assert(xu_stack_head(stack) == (xu_pointer_t)0x0);
    xu_assert(xu_stack_last(stack) == (xu_pointer_t)0xf);

    // clear it
    xu_stack_clear(stack);
    xu_assert(!xu_stack_size(stack));

    // exit
    xu_stack_exit(stack);

    return n / ((xu_uint32_t)(t) + 1);
}

static xu_size_t xu_stack_iterator_next_test()
{
    // init
    xu_size_t      n     = 1000000;
    xu_stack_ref_t stack = xu_stack_init(n, xu_element_long());
    xu_assert_and_check_return_val(stack, 0);

    xu_size_t i;
    for (i = 0; i < n; i++)
        xu_stack_put(stack, (xu_pointer_t)0xf);
    xu_hong_t t = xu_mclock();
    xu_for_all(xu_char_t*, item, stack) xu_used(item);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_stack_iterator_next(%lu): %lld ms, size: %lu, maxn: %lu", 1000000, t, xu_stack_size(stack),
               xu_stack_maxn(stack));

    // exit
    xu_stack_exit(stack);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_stack_iterator_prev_test()
{
    // init
    xu_size_t      n     = 1000000;
    xu_stack_ref_t stack = xu_stack_init(n, xu_element_long());
    xu_assert_and_check_return_val(stack, 0);

    xu_size_t i;
    for (i = 0; i < n; i++)
        xu_stack_put(stack, (xu_pointer_t)0xf);
    xu_hong_t t = xu_mclock();
    xu_rfor_all(xu_char_t*, item, stack) xu_used(item);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_stack_iterator_prev(%lu): %lld ms, size: %lu, maxn: %lu", 1000000, t, xu_stack_size(stack),
               xu_stack_maxn(stack));

    // exit
    xu_stack_exit(stack);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_void_t xu_stack_perf_test()
{
    xu_size_t score = 0;
    xu_trace_i("=============================================================");
    xu_trace_i("put & pop performance:");
    score += xu_stack_put_and_pop_test();

    xu_trace_i("=============================================================");
    xu_trace_i("iterator performance:");
    score += xu_stack_iterator_next_test();
    score += xu_stack_iterator_prev_test();

    xu_trace_i("=============================================================");
    xu_trace_i("score: %lu", score / 100);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_stack_main(xu_int_t argc, xu_char_t** argv)
{
    xu_stack_perf_test();

    return 0;
}
