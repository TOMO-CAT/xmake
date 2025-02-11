#include "xutil/unit_test/ut.h"

/* *******************************************************
 * globals
 */
static xu_size_t    g_func_indx = 0;
static xu_element_t g_func_prev;

/* *******************************************************
 * test
 */
static xu_size_t xu_demo_test_hash_func(xu_element_ref_t element, xu_cpointer_t priv, xu_size_t mask, xu_size_t index)
{
    return g_func_prev.hash(element, priv, mask, g_func_indx);
}
static xu_void_t xu_demo_test_cstr_h(xu_size_t index)
{
    // the count
    xu_size_t count = 1000000;

    // save element
    g_func_indx = index;
    g_func_prev = xu_element_str(xu_true);

    // the element
    xu_element_t element = g_func_prev;
    element.hash         = xu_demo_test_hash_func;

    // init filter
    xu_bloom_filter_ref_t filter = xu_bloom_filter_init(XU_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        xu_size_t i      = 0;
        xu_size_t r      = 0;
        xu_char_t s[256] = {0};
        xu_hong_t t      = xu_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            xu_long_t value = xu_random();

            // format it
            xu_snprintf(s, sizeof(s) - 1, "%ld", value);

            // set value to filter
            if (!xu_bloom_filter_set(filter, s))
            {
                // repeat++
                r++;
            }
        }
        t = xu_mclock() - t;

        // trace
        xu_trace_i("cstr: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        xu_bloom_filter_exit(filter);
    }
}
static xu_void_t xu_demo_test_cstr_p()
{
    // the count
    xu_size_t count = 10000000;

    // init filter
    xu_bloom_filter_ref_t filter =
        xu_bloom_filter_init(XU_BLOOM_FILTER_PROBABILITY_0_01, 3, count, xu_element_str(xu_true));
    if (filter)
    {
        // done
        xu_size_t i      = 0;
        xu_size_t r      = 0;
        xu_char_t s[256] = {0};
        for (i = 0; i < count; i++)
        {
            // the value
            xu_long_t value = xu_random();

            // format it
            xu_snprintf(s, sizeof(s) - 1, "%ld", value);

            // set value to filter
            if (!xu_bloom_filter_set(filter, s))
            {
                // repeat++
                r++;
            }
        }

        // trace
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
        xu_trace_i("cstr: count: %lu, repeat: %lu, repeat_p ~= p: %lf", count, r, (xu_double_t)r / count);
#else
        xu_trace_i("cstr: count: %lu, repeat: %lu", count, r);
#endif

        // exit filter
        xu_bloom_filter_exit(filter);
    }
}
static xu_void_t xu_demo_test_uint8_h(xu_size_t index)
{
    // the count
    xu_size_t count = XU_MAXU8;

    // save element
    g_func_indx = index;
    g_func_prev = xu_element_uint8();

    // the element
    xu_element_t element = g_func_prev;
    element.hash         = xu_demo_test_hash_func;

    // init filter
    xu_bloom_filter_ref_t filter = xu_bloom_filter_init(XU_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        xu_size_t i = 0;
        xu_size_t r = 0;
        xu_hong_t t = xu_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            xu_long_t value = xu_random_range(0, XU_MAXU8);

            // set value to filter
            if (!xu_bloom_filter_set(filter, (xu_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }
        t = xu_mclock() - t;

        // trace
        xu_trace_i("uint8: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        xu_bloom_filter_exit(filter);
    }
}
static xu_void_t xu_demo_test_uint16_h(xu_size_t index)
{
    // the count
    xu_size_t count = XU_MAXU16;

    // save element
    g_func_indx = index;
    g_func_prev = xu_element_uint16();

    // the element
    xu_element_t element = g_func_prev;
    element.hash         = xu_demo_test_hash_func;

    // init filter
    xu_bloom_filter_ref_t filter = xu_bloom_filter_init(XU_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        xu_size_t i = 0;
        xu_size_t r = 0;
        xu_hong_t t = xu_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            xu_long_t value = xu_random_range(0, XU_MAXU16);

            // set value to filter
            if (!xu_bloom_filter_set(filter, (xu_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }
        t = xu_mclock() - t;

        // trace
        xu_trace_i("uint16: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        xu_bloom_filter_exit(filter);
    }
}
static xu_void_t xu_demo_test_uint32_h(xu_size_t index)
{
    // the count
    xu_size_t count = 1000000;

    // save element
    g_func_indx = index;
    g_func_prev = xu_element_uint32();

    // the element
    xu_element_t element = g_func_prev;
    element.hash         = xu_demo_test_hash_func;

    // init filter
    xu_bloom_filter_ref_t filter = xu_bloom_filter_init(XU_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        xu_size_t i = 0;
        xu_size_t r = 0;
        xu_hong_t t = xu_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            xu_long_t value = xu_random_value();

            // set value to filter
            if (!xu_bloom_filter_set(filter, (xu_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }
        t = xu_mclock() - t;

        // trace
        xu_trace_i("uint32: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        xu_bloom_filter_exit(filter);
    }
}
static xu_void_t xu_demo_test_long_h(xu_size_t index)
{
    // the count
    xu_size_t count = 1000000;

    // save element
    g_func_indx = index;
    g_func_prev = xu_element_long();

    // the element
    xu_element_t element = g_func_prev;
    element.hash         = xu_demo_test_hash_func;

    // init filter
    xu_bloom_filter_ref_t filter = xu_bloom_filter_init(XU_BLOOM_FILTER_PROBABILITY_0_001, 1, count, element);
    if (filter)
    {
        // done
        xu_size_t i = 0;
        xu_size_t r = 0;
        xu_hong_t t = xu_mclock();
        for (i = 0; i < count; i++)
        {
            // the value
            xu_long_t value = xu_random();

            // set value to filter
            if (!xu_bloom_filter_set(filter, (xu_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }
        t = xu_mclock() - t;

        // trace
        xu_trace_i("long: index: %lu, repeat: %lu, time: %lld ms", index, r, t);

        // exit filter
        xu_bloom_filter_exit(filter);
    }
}
static xu_void_t xu_demo_test_long_p()
{
    // the count
    xu_size_t count = 10000000;

    // init filter
    xu_bloom_filter_ref_t filter = xu_bloom_filter_init(XU_BLOOM_FILTER_PROBABILITY_0_01, 3, count, xu_element_long());
    if (filter)
    {
        // done
        xu_size_t i = 0;
        xu_size_t r = 0;
        for (i = 0; i < count; i++)
        {
            // the value
            xu_long_t value = xu_random();

            // set value to filter
            if (!xu_bloom_filter_set(filter, (xu_cpointer_t)value))
            {
                // repeat++
                r++;
            }
        }

        // trace
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
        xu_trace_i("long: count: %lu, repeat: %lu, repeat_p ~= p: %lf", count, r, (xu_double_t)r / count);
#else
        xu_trace_i("long: count: %lu, repeat: %lu", count, r);
#endif

        // exit filter
        xu_bloom_filter_exit(filter);
    }
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_bloom_filter_main(xu_int_t argc, xu_char_t** argv)
{
    xu_trace_i("===========================================================");
    xu_demo_test_uint8_h(0);
    xu_demo_test_uint8_h(1);
    xu_demo_test_uint8_h(2);
    xu_demo_test_uint8_h(3);
    xu_demo_test_uint8_h(4);
    xu_demo_test_uint8_h(5);
    xu_demo_test_uint8_h(6);
    xu_demo_test_uint8_h(7);
    xu_demo_test_uint8_h(8);
    xu_demo_test_uint8_h(9);
    xu_demo_test_uint8_h(10);
    xu_demo_test_uint8_h(11);
    xu_demo_test_uint8_h(12);
    xu_demo_test_uint8_h(13);
    xu_demo_test_uint8_h(14);
    xu_demo_test_uint8_h(15);

    xu_trace_i("===========================================================");
    xu_demo_test_uint16_h(0);
    xu_demo_test_uint16_h(1);
    xu_demo_test_uint16_h(2);
    xu_demo_test_uint16_h(3);
    xu_demo_test_uint16_h(4);
    xu_demo_test_uint16_h(5);
    xu_demo_test_uint16_h(6);
    xu_demo_test_uint16_h(7);
    xu_demo_test_uint16_h(8);
    xu_demo_test_uint16_h(9);
    xu_demo_test_uint16_h(10);
    xu_demo_test_uint16_h(11);
    xu_demo_test_uint16_h(12);
    xu_demo_test_uint16_h(13);
    xu_demo_test_uint16_h(14);
    xu_demo_test_uint16_h(15);

    xu_trace_i("===========================================================");
    xu_demo_test_uint32_h(0);
    xu_demo_test_uint32_h(1);
    xu_demo_test_uint32_h(2);
    xu_demo_test_uint32_h(3);
    xu_demo_test_uint32_h(4);
    xu_demo_test_uint32_h(5);
    xu_demo_test_uint32_h(6);
    xu_demo_test_uint32_h(7);
    xu_demo_test_uint32_h(8);
    xu_demo_test_uint32_h(9);
    xu_demo_test_uint32_h(10);
    xu_demo_test_uint32_h(11);
    xu_demo_test_uint32_h(12);
    xu_demo_test_uint32_h(13);
    xu_demo_test_uint32_h(14);
    xu_demo_test_uint32_h(15);

    xu_trace_i("===========================================================");
    xu_demo_test_long_h(0);
    xu_demo_test_long_h(1);
    xu_demo_test_long_h(2);
    xu_demo_test_long_h(3);
    xu_demo_test_long_h(4);
    xu_demo_test_long_h(5);
    xu_demo_test_long_h(6);
    xu_demo_test_long_h(7);
    xu_demo_test_long_h(8);
    xu_demo_test_long_h(9);
    xu_demo_test_long_h(10);
    xu_demo_test_long_h(11);
    xu_demo_test_long_h(12);
    xu_demo_test_long_h(13);
    xu_demo_test_long_h(14);
    xu_demo_test_long_h(15);

    xu_trace_i("===========================================================");
    xu_demo_test_cstr_h(0);
    xu_demo_test_cstr_h(1);
    xu_demo_test_cstr_h(2);
    xu_demo_test_cstr_h(3);
    xu_demo_test_cstr_h(4);
    xu_demo_test_cstr_h(5);
    xu_demo_test_cstr_h(6);
    xu_demo_test_cstr_h(7);
    xu_demo_test_cstr_h(8);
    xu_demo_test_cstr_h(9);
    xu_demo_test_cstr_h(10);
    xu_demo_test_cstr_h(11);
    xu_demo_test_cstr_h(12);
    //  xu_demo_test_cstr_h(13);
    //  xu_demo_test_cstr_h(14);
    //  xu_demo_test_cstr_h(15);

    xu_trace_i("===========================================================");
    xu_demo_test_long_p();
    xu_demo_test_cstr_p();

    return 0;
}
