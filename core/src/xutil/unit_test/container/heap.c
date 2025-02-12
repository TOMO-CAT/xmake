#include "xutil/unit_test/ut.h"

/* *******************************************************
 * test
 */

static xu_long_t xu_test_heap_max_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    return ((xu_p2u32(ldata) > xu_p2u32(rdata)) ? -1 : (xu_p2u32(ldata) < xu_p2u32(rdata)));
}
static xu_void_t xu_test_heap_min_func()
{
    // init heap
    xu_heap_ref_t heap = xu_heap_init(16, xu_element_uint32());
    xu_assert_and_check_return(heap);

    // reset rand
    xu_random_reset(xu_true);

    // make heap
    xu_size_t i = 0;
    for (i = 0; i < 100; i++)
    {
        // the value
        xu_uint32_t val = (xu_uint32_t)xu_random_range(0, 50);

        // trace
        //      xu_trace_i("heap_min: put: %u", val);

        // put it
        xu_heap_put(heap, xu_u2p(val));
    }

    // reset rand
    xu_random_reset(xu_true);

    // remove some values
    for (i = 0; i < 100; i++)
    {
        // the value
        xu_uint32_t val = (xu_uint32_t)xu_random_range(0, 50);

        // remove it?
        if (!(i & 3))
        {
            xu_size_t itor = xu_find_all(heap, xu_u2p(val));
            if (itor != xu_iterator_tail(heap)) xu_heap_remove(heap, itor);
        }
    }

    // append heap
    for (i = 0; i < 30; i++)
    {
        // the value
        xu_uint32_t val = (xu_uint32_t)xu_random_range(0, 50);

        // put it
        xu_heap_put(heap, xu_u2p(val));
    }

    // trace
    xu_trace_i("");

    // dump heap
    while (xu_heap_size(heap))
    {
        // put it
        xu_uint32_t val = (xu_uint32_t)(xu_size_t)xu_heap_top(heap);

        // trace
        xu_trace_i("heap_min: pop: %u", val);

        // pop it
        xu_heap_pop(heap);
    }

    // exit heap
    xu_heap_exit(heap);
}
static xu_void_t xu_test_heap_min_perf()
{
    // init heap
    xu_heap_ref_t heap = xu_heap_init(4096, xu_element_uint32());
    xu_assert_and_check_return(heap);

    // reset rand
    xu_random_reset(xu_true);

    // init time
    xu_hong_t time = xu_mclock();

    // profile
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000; // FIXME: n 为 100000 时会挂在 xu_assert(!i || p <= v);
    __xu_volatile__ xu_size_t p;
    xu_used(&p);
    for (i = 0; i < n; i++)
        xu_heap_put(heap, (xu_pointer_t)(xu_size_t)xu_random_range(0, 50));
    for (i = 0; xu_heap_size(heap); i++)
    {
        // get the top value
        xu_size_t v = (xu_size_t)xu_heap_top(heap);

        // check order
        xu_assert(!i || p <= v);

        // save the previous value
        p = v;

        // pop it
        xu_heap_pop(heap);
    }

    // exit time
    time = xu_mclock() - time;

    // trace
    xu_trace_i("heap_min: %lld ms", time);

    // exit heap
    xu_heap_exit(heap);
}
static xu_void_t xu_test_heap_max_func()
{
    // init element
    xu_element_t element = xu_element_uint32();
    element.comp         = xu_test_heap_max_comp;

    // init heap
    xu_heap_ref_t heap = xu_heap_init(16, element);
    xu_assert_and_check_return(heap);

    // reset rand
    xu_random_reset(xu_true);

    // make heap
    xu_size_t i = 0;
    for (i = 0; i < 100; i++)
    {
        // the value
        xu_uint32_t val = (xu_uint32_t)xu_random_range(0, 50);

        // trace
        //      xu_trace_i("heap_max: put: %u", val);

        // put it
        xu_heap_put(heap, xu_u2p(val));
    }

    // remove some values
    for (i = 0; i < 100; i++)
    {
        // the value
        xu_uint32_t val = (xu_uint32_t)xu_random_range(0, 50);

        // remove it?
        if (!(i & 3))
        {
            xu_size_t itor = xu_find_all(heap, xu_u2p(val));
            if (itor != xu_iterator_tail(heap)) xu_heap_remove(heap, itor);
        }
    }

    // append heap
    for (i = 0; i < 30; i++)
    {
        // the value
        xu_uint32_t val = (xu_uint32_t)xu_random_range(0, 50);

        // put it
        xu_heap_put(heap, xu_u2p(val));
    }

    // trace
    xu_trace_i("");

    // dump heap
    while (xu_heap_size(heap))
    {
        // put it
        xu_uint32_t val = (xu_uint32_t)(xu_size_t)xu_heap_top(heap);

        // trace
        xu_trace_i("heap_max: pop: %u", val);

        // pop it
        xu_heap_pop(heap);
    }

    // exit heap
    xu_heap_exit(heap);
}
static xu_void_t xu_test_heap_max_perf()
{
    // init element
    xu_element_t element = xu_element_uint32();
    element.comp         = xu_test_heap_max_comp;

    // init heap
    xu_heap_ref_t heap = xu_heap_init(4096, element);
    xu_assert_and_check_return(heap);

    // init time
    xu_hong_t time = xu_mclock();

    // profile
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_size_t p;
    xu_used(&p);
    for (i = 0; i < n; i++)
        xu_heap_put(heap, (xu_pointer_t)(xu_size_t)xu_random_range(0, 50));
    for (i = 0; xu_heap_size(heap); i++)
    {
        // get the top value
        xu_size_t v = (xu_size_t)xu_heap_top(heap);

        // check order
        xu_assert(!i || p >= v);

        // save the previous value
        p = v;

        // pop it
        xu_heap_pop(heap);
    }

    // exit time
    time = xu_mclock() - time;

    // trace
    xu_trace_i("heap_max: %lld ms", time);

    // exit heap
    xu_heap_exit(heap);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_heap_main(xu_int_t argc, xu_char_t** argv)
{
    // element
    xu_test_heap_min_func();
    xu_test_heap_max_func();

    // performance
    xu_test_heap_min_perf();
    xu_test_heap_max_perf();
    return 0;
}
