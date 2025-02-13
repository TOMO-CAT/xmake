#include "xutil/unit_test/ut.h"

static xu_void_t xu_find_int_test()
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++)
        data[i] = i;

    // find
    xu_size_t itor = xu_iterator_tail(iterator);
    xu_hong_t time = xu_mclock();
    for (i = 0; i < n; i++)
        itor = xu_find_all(iterator, (xu_pointer_t)data[800]);
    time = xu_mclock() - time;

    // item
    xu_long_t item = itor != xu_iterator_tail(iterator) ? (xu_long_t)xu_iterator_item(iterator, itor) : 0;

    // time
    xu_trace_i("xu_find_int_all[%ld ?= %ld]: %lld ms", item, data[800], time);

    // free
    xu_free(data);
}
static xu_void_t xu_find_int_test_binary()
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++)
        data[i] = i;

    // find
    xu_size_t itor = xu_iterator_tail(iterator);
    xu_hong_t time = xu_mclock();
    for (i = 0; i < n; i++)
        itor = xu_binary_find_all(iterator, (xu_pointer_t)data[800]);
    time = xu_mclock() - time;

    // item
    xu_long_t item = itor != xu_iterator_tail(iterator) ? (xu_long_t)xu_iterator_item(iterator, itor) : 0;

    // time
    xu_trace_i("xu_binary_find_int_all[%ld ?= %ld]: %lld ms", item, data[800], time);

    // free
    xu_free(data);
}
static xu_void_t xu_find_str_test()
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000;

    // init data
    xu_char_t** data = (xu_char_t**)xu_nalloc0(n, sizeof(xu_char_t*));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_str(&array_iterator, data, n);

    // make
    xu_char_t s[256] = {0};
    for (i = 0; i < n; i++)
    {
        xu_long_t r = xu_snprintf(s, 256, "%04lu", i);
        s[r]        = '\0';
        data[i]     = xu_strdup(s);
    }

    // find
    xu_size_t itor = xu_iterator_tail(iterator);
    xu_hong_t time = xu_mclock();
    for (i = 0; i < n; i++)
        itor = xu_find_all(iterator, (xu_pointer_t)data[800]);
    time = xu_mclock() - time;

    // item
    xu_char_t* item = itor != xu_iterator_tail(iterator) ? (xu_char_t*)xu_iterator_item(iterator, itor) : 0;

    // time
    xu_trace_i("xu_find_str_all[%s ?= %s]: %lld ms", item, data[800], time);

    // free data
    for (i = 0; i < n; i++)
        xu_free(data[i]);
    xu_free(data);
}
static xu_void_t xu_find_str_test_binary()
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000;

    // init data
    xu_char_t** data = (xu_char_t**)xu_nalloc0(n, sizeof(xu_char_t*));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_str(&array_iterator, data, n);

    // make
    xu_char_t s[256] = {0};
    for (i = 0; i < n; i++)
    {
        xu_long_t r = xu_snprintf(s, 256, "%04lu", i);
        s[r]        = '\0';
        data[i]     = xu_strdup(s);
    }

    // find
    xu_size_t itor = xu_iterator_tail(iterator);
    xu_hong_t time = xu_mclock();
    for (i = 0; i < n; i++)
        itor = xu_binary_find_all(iterator, (xu_pointer_t)data[800]);
    time = xu_mclock() - time;

    // item
    xu_char_t* item = itor != xu_iterator_tail(iterator) ? (xu_char_t*)xu_iterator_item(iterator, itor) : 0;

    // time
    xu_trace_i("xu_binary_find_str_all[%s ?= %s]: %lld ms", item, data[800], time);

    // free data
    for (i = 0; i < n; i++)
        xu_free(data[i]);
    xu_free(data);
}

xu_int_t xu_ut_algorithm_find_main(xu_int_t argc, xu_char_t** argv)
{
    // test
    xu_find_int_test();
    xu_find_int_test_binary();
    xu_find_str_test();
    xu_find_str_test_binary();

    return 0;
}
