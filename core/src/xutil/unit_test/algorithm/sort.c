#include "xutil/algorithm/sort.h"
#include "xutil/algorithm/bubble_sort.h"
#include "xutil/algorithm/heap_sort.h"
#include "xutil/algorithm/insert_sort.h"
#include "xutil/algorithm/quick_sort.h"
#include "xutil/math/random/random.h"
#include "xutil/platform/time.h"
#include "xutil/unit_test/ut.h"

static xu_void_t xu_sort_int_test_perf(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++)
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);

    // sort
    xu_hong_t time = xu_mclock();
    xu_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    xu_free(data);
}

static xu_void_t xu_sort_int_test_perf_bubble(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++)
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);

    // sort
    xu_hong_t time = xu_mclock();
    xu_bubble_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_bubble_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    xu_free(data);
}
static xu_void_t xu_sort_int_test_func_bubble()
{
    // init
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 20;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // trace
    xu_trace_i("");

    // put
    for (i = 0; i < n; i++)
    {
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);
        xu_trace_i("bubble_put: %ld", data[i]);
    }

    // sort
    xu_heap_sort_all(iterator, xu_null);

    // trace
    xu_trace_i("");

    // pop
    for (i = 0; i < n; i++)
        xu_trace_i("bubble_pop: %ld", data[i]);

    // free
    xu_free(data);
}
static xu_void_t xu_sort_int_test_perf_insert(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++)
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);

    // sort
    xu_hong_t time = xu_mclock();
    xu_insert_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_insert_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    xu_free(data);
}
static xu_void_t xu_sort_int_test_func_insert()
{
    // init
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 20;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // trace
    xu_trace_i("");

    // put
    for (i = 0; i < n; i++)
    {
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);
        xu_trace_i("insert_put: %ld", data[i]);
    }

    // sort
    xu_heap_sort_all(iterator, xu_null);

    // trace
    xu_trace_i("");

    // pop
    for (i = 0; i < n; i++)
        xu_trace_i("insert_pop: %ld", data[i]);

    // free
    xu_free(data);
}
static xu_void_t xu_sort_int_test_perf_quick(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++)
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);

    // sort
    xu_hong_t time = xu_mclock();
    xu_quick_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_quick_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    xu_free(data);
}
static xu_void_t xu_sort_int_test_func_quick()
{
    // init
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 20;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // trace
    xu_trace_i("");

    // put
    for (i = 0; i < n; i++)
    {
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);
        xu_trace_i("quick_put: %ld", data[i]);
    }

    // sort
    xu_heap_sort_all(iterator, xu_null);

    // trace
    xu_trace_i("");

    // pop
    for (i = 0; i < n; i++)
        xu_trace_i("quick_pop: %ld", data[i]);

    // free
    xu_free(data);
}
static xu_void_t xu_sort_int_test_perf_heap(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // make
    for (i = 0; i < n; i++)
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);

    // sort
    xu_hong_t time = xu_mclock();
    xu_heap_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_heap_sort_int_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(data[i - 1] <= data[i]);

    // free
    xu_free(data);
}
static xu_void_t xu_sort_int_test_func_heap()
{
    // init
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 20;

    // init data
    xu_long_t* data = (xu_long_t*)xu_nalloc0(n, sizeof(xu_long_t));
    xu_assert_and_check_return(data);

    // init iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator = xu_array_iterator_init_long(&array_iterator, data, n);

    // trace
    xu_trace_i("");

    // put
    for (i = 0; i < n; i++)
    {
        data[i] = xu_random_range(XU_MINS16, XU_MAXS16);
        xu_trace_i("heap_put: %ld", data[i]);
    }

    // sort
    xu_heap_sort_all(iterator, xu_null);

    // trace
    xu_trace_i("");

    // pop
    for (i = 0; i < n; i++)
        xu_trace_i("heap_pop: %ld", data[i]);

    // free
    xu_free(data);
}
static xu_void_t xu_sort_str_test_perf(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

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
        xu_long_t r = xu_snprintf(s, 256, "%ld", xu_random_value());
        s[r]        = '\0';
        data[i]     = xu_strdup(s);
    }

    // sort
    xu_hong_t time = xu_mclock();
    xu_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(xu_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++)
        xu_free(data[i]);
    xu_free(data);
}
static xu_void_t xu_sort_str_test_perf_bubble(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

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
        xu_long_t r = xu_snprintf(s, 256, "%ld", xu_random_value());
        s[r]        = '\0';
        data[i]     = xu_strdup(s);
    }

    // sort
    xu_hong_t time = xu_mclock();
    xu_bubble_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_bubble_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(xu_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++)
        xu_free(data[i]);
    xu_free(data);
}
static xu_void_t xu_sort_str_test_perf_insert(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

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
        xu_long_t r = xu_snprintf(s, 256, "%ld", xu_random_value());
        s[r]        = '\0';
        data[i]     = xu_strdup(s);
    }

    // sort
    xu_hong_t time = xu_mclock();
    xu_insert_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_insert_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(xu_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++)
        xu_free(data[i]);
    xu_free(data);
}
static xu_void_t xu_sort_str_test_perf_quick(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

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
        xu_long_t r = xu_snprintf(s, 256, "%ld", xu_random_value());
        s[r]        = '\0';
        data[i]     = xu_strdup(s);
    }

    // sort
    xu_hong_t time = xu_mclock();
    xu_quick_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_quick_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(xu_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++)
        xu_free(data[i]);
    xu_free(data);
}
static xu_void_t xu_sort_str_test_perf_heap(xu_size_t n)
{
    __xu_volatile__ xu_size_t i = 0;

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
        xu_long_t r = xu_snprintf(s, 256, "%ld", xu_random_value());
        s[r]        = '\0';
        data[i]     = xu_strdup(s);
    }

    // sort
    xu_hong_t time = xu_mclock();
    xu_heap_sort_all(iterator, xu_null);
    time = xu_mclock() - time;

    // time
    xu_trace_i("xu_heap_sort_str_all: %lld ms", time);

    // check
    for (i = 1; i < n; i++)
        xu_assert_and_check_break(xu_strcmp(data[i - 1], data[i]) <= 0);

    // free data
    for (i = 0; i < n; i++)
        xu_free(data[i]);
    xu_free(data);
}

xu_int_t xu_ut_algorithm_sort_main(xu_int_t argc, xu_char_t** argv)
{
    // func
    xu_sort_int_test_func_heap();
    xu_sort_int_test_func_quick();
    xu_sort_int_test_func_bubble();
    xu_sort_int_test_func_insert();

    // perf
    xu_sort_int_test_perf(1000);
    xu_sort_int_test_perf_heap(1000);
    xu_sort_int_test_perf_quick(1000);
    xu_sort_int_test_perf_bubble(1000);
    xu_sort_int_test_perf_insert(1000);
    xu_sort_str_test_perf(1000);
    xu_sort_str_test_perf_heap(1000);
    xu_sort_str_test_perf_quick(1000);
    xu_sort_str_test_perf_bubble(1000);
    xu_sort_str_test_perf_insert(1000);

    return 0;
}
