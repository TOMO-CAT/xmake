#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */

#define XU_VECTOR_GROW_SIZE (256)

/* *******************************************************
 * implementation
 */
static xu_size_t xu_vector_insert_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_hong_t                 t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_vector_insert_prev(vector, xu_vector_size(vector) >> 1, (xu_pointer_t)0xf);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_insert_prev(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xf);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xf);

    // clear it
    xu_vector_clear(vector);
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_insert_head_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_hong_t                 t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_vector_insert_head(vector, (xu_pointer_t)0xf);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_insert_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xf);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xf);

    // clear it
    xu_vector_clear(vector);
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_insert_tail_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_hong_t                 t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_vector_insert_tail(vector, (xu_pointer_t)0xf);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_insert_tail(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xf);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xf);

    // clear it
    xu_vector_clear(vector);
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_ninsert_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    // insert one first
    xu_vector_insert_head(vector, (xu_pointer_t)0xf);

    xu_size_t n = 10000;
    xu_hong_t t = xu_mclock();
    xu_vector_ninsert_prev(vector, 1, (xu_pointer_t)0xf, n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_ninsert_prev(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n + 1);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xf);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xf);

    // clear it
    xu_vector_clear(vector);
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_ninsert_head_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_hong_t t = xu_mclock();
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_ninsert_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xf);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xf);

    // clear it
    xu_vector_clear(vector);
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_ninsert_tail_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_hong_t t = xu_mclock();
    xu_vector_ninsert_tail(vector, (xu_pointer_t)0xf, n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_ninsert_tail(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xf);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xf);

    // clear it
    xu_vector_clear(vector);
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}

static xu_size_t xu_vector_remove_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_vector_remove(vector, xu_vector_size(vector) >> 1);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_remove(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_remove_head_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_vector_remove_head(vector);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_remove_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_remove_last_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_vector_remove_last(vector);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_remove_last(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_nremove_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    xu_vector_nremove(vector, xu_iterator_head(vector), n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_nremove(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_nremove_head_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    xu_vector_nremove_head(vector, n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_nremove_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_nremove_last_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    xu_vector_nremove_last(vector, n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_nremove_last(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(!xu_vector_size(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_replace_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    __xu_volatile__ xu_size_t itor = xu_iterator_head(vector);
    __xu_volatile__ xu_size_t tail = xu_iterator_tail(vector);
    xu_hong_t                 t    = xu_mclock();
    for (; itor != tail; itor = xu_iterator_next(vector, itor))
        xu_vector_replace(vector, itor, (xu_pointer_t)0xd);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_replace(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xd);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xd);

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_replace_head_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_vector_replace_head(vector, (xu_pointer_t)0xd);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_replace_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xd);

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_replace_last_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_vector_replace_last(vector, (xu_pointer_t)0xd);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_replace_last(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xd);

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_nreplace_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    xu_vector_nreplace(vector, xu_iterator_head(vector), (xu_pointer_t)0xd, n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_nreplace(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xd);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xd);

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_nreplace_head_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    xu_vector_nreplace_head(vector, (xu_pointer_t)0xd, n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_nreplace_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xd);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xd);

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_nreplace_last_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    xu_vector_nreplace_last(vector, (xu_pointer_t)0xd, n);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_nreplace_last(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // check
    xu_assert(xu_vector_size(vector) == n);
    xu_assert(xu_vector_head(vector) == (xu_pointer_t)0xd);
    xu_assert(xu_vector_last(vector) == (xu_pointer_t)0xd);

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}

static xu_size_t xu_vector_iterator_next_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    xu_for_all(xu_char_t*, item, vector) xu_used(item);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_iterator_next(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_size_t xu_vector_iterator_prev_test()
{
    // init
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return_val(vector, 0);

    xu_size_t n = 10000;
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xf, n);
    xu_hong_t t = xu_mclock();
    xu_rfor_all(xu_char_t*, item, vector) xu_used(item);
    t = xu_mclock() - t;

    // time
    xu_trace_i("xu_vector_iterator_prev(%lu): %lld ms, size: %lu, maxn: %lu", n, t, xu_vector_size(vector),
               xu_vector_maxn(vector));

    // exit
    xu_vector_exit(vector);

    return n / ((xu_uint32_t)(t) + 1);
}
static xu_void_t xu_vector_int_dump(xu_vector_ref_t vector)
{
    xu_trace_i("xu_int_t size: %lu, maxn: %lu", xu_vector_size(vector), xu_vector_maxn(vector));
    xu_for_all(xu_char_t*, item, vector) { xu_trace_i("xu_int_t at[%lu]: %x", item_itor, item); }
}
static xu_void_t xu_vector_int_test()
{
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("insert:");
    xu_vector_ninsert_head(vector, (xu_pointer_t)0xa, 10);
    xu_vector_ninsert_tail(vector, (xu_pointer_t)0xf, 10);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)0);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)1);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)2);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)3);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)4);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)5);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)6);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)7);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)8);
    xu_vector_insert_prev(vector, 10, (xu_pointer_t)9);
    xu_vector_insert_head(vector, (xu_pointer_t)4);
    xu_vector_insert_head(vector, (xu_pointer_t)3);
    xu_vector_insert_head(vector, (xu_pointer_t)2);
    xu_vector_insert_head(vector, (xu_pointer_t)1);
    xu_vector_insert_head(vector, (xu_pointer_t)0);
    xu_vector_insert_tail(vector, (xu_pointer_t)5);
    xu_vector_insert_tail(vector, (xu_pointer_t)6);
    xu_vector_insert_tail(vector, (xu_pointer_t)7);
    xu_vector_insert_tail(vector, (xu_pointer_t)8);
    xu_vector_insert_tail(vector, (xu_pointer_t)9);
    xu_vector_int_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("remove:");
    xu_vector_nremove_head(vector, 5);
    xu_vector_nremove_last(vector, 5);
    xu_vector_int_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("replace:");
    xu_vector_nreplace_head(vector, (xu_pointer_t)0xf, 10);
    xu_vector_nreplace_last(vector, (xu_pointer_t)0xa, 10);
    xu_vector_replace_head(vector, 0);
    xu_vector_replace_last(vector, 0);
    xu_vector_int_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("clear:");
    xu_vector_clear(vector);
    xu_vector_int_dump(vector);

    xu_vector_exit(vector);
}
static xu_void_t xu_vector_str_dump(xu_vector_ref_t vector)
{
    xu_trace_i("str size: %lu, maxn: %lu", xu_vector_size(vector), xu_vector_maxn(vector));
    xu_for_all(xu_char_t*, item, vector) xu_trace_i("str at[%lu]: %s", item_itor, item);
}
static xu_void_t xu_vector_str_test()
{
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_str(xu_true));
    xu_assert_and_check_return(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("insert:");
    xu_vector_ninsert_head(vector, "HHHHHHHHHH", 10);
    xu_vector_ninsert_tail(vector, "TTTTTTTTTT", 10);
    xu_vector_insert_prev(vector, 10, "0000000000");
    xu_vector_insert_prev(vector, 10, "1111111111");
    xu_vector_insert_prev(vector, 10, "2222222222");
    xu_vector_insert_prev(vector, 10, "3333333333");
    xu_vector_insert_prev(vector, 10, "4444444444");
    xu_vector_insert_prev(vector, 10, "5555555555");
    xu_vector_insert_prev(vector, 10, "6666666666");
    xu_vector_insert_prev(vector, 10, "7777777777");
    xu_vector_insert_prev(vector, 10, "8888888888");
    xu_vector_insert_prev(vector, 10, "9999999999");
    xu_vector_insert_head(vector, "4444444444");
    xu_vector_insert_head(vector, "3333333333");
    xu_vector_insert_head(vector, "2222222222");
    xu_vector_insert_head(vector, "1111111111");
    xu_vector_insert_head(vector, "0000000000");
    xu_vector_insert_tail(vector, "5555555555");
    xu_vector_insert_tail(vector, "6666666666");
    xu_vector_insert_tail(vector, "7777777777");
    xu_vector_insert_tail(vector, "8888888888");
    xu_vector_insert_tail(vector, "9999999999");
    xu_vector_str_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("remove:");
    xu_vector_nremove_head(vector, 5);
    xu_vector_nremove_last(vector, 5);
    xu_vector_str_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("replace:");
    xu_vector_nreplace_head(vector, "TTTTTTTTTT", 10);
    xu_vector_nreplace_last(vector, "HHHHHHHHHH", 10);
    xu_vector_replace_head(vector, "OOOOOOOOOO");
    xu_vector_replace_last(vector, "OOOOOOOOOO");
    xu_vector_str_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("clear:");
    xu_vector_clear(vector);
    xu_vector_str_dump(vector);

    xu_vector_exit(vector);
}
static xu_void_t xu_vector_mem_dump(xu_vector_ref_t vector)
{
    xu_trace_i("ifm size: %lu, maxn: %lu", xu_vector_size(vector), xu_vector_maxn(vector));
    xu_for_all(xu_char_t*, item, vector) { xu_trace_i("ifm at[%lu]: %s", item_itor, item); }
}
static xu_void_t xu_vector_mem_free(xu_element_ref_t element, xu_pointer_t buff)
{
    xu_trace_i("ifm free: %s, priv: %s", buff, element->priv);
}
static xu_void_t xu_vector_mem_test()
{
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_mem(11, xu_vector_mem_free, "ifm"));
    xu_assert_and_check_return(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("insert:");
    xu_vector_ninsert_head(vector, "HHHHHHHHHH", 10);
    xu_vector_ninsert_tail(vector, "TTTTTTTTTT", 10);
    xu_vector_insert_prev(vector, 10, "0000000000");
    xu_vector_insert_prev(vector, 10, "1111111111");
    xu_vector_insert_prev(vector, 10, "2222222222");
    xu_vector_insert_prev(vector, 10, "3333333333");
    xu_vector_insert_prev(vector, 10, "4444444444");
    xu_vector_insert_prev(vector, 10, "5555555555");
    xu_vector_insert_prev(vector, 10, "6666666666");
    xu_vector_insert_prev(vector, 10, "7777777777");
    xu_vector_insert_prev(vector, 10, "8888888888");
    xu_vector_insert_prev(vector, 10, "9999999999");
    xu_vector_insert_head(vector, "4444444444");
    xu_vector_insert_head(vector, "3333333333");
    xu_vector_insert_head(vector, "2222222222");
    xu_vector_insert_head(vector, "1111111111");
    xu_vector_insert_head(vector, "0000000000");
    xu_vector_insert_tail(vector, "5555555555");
    xu_vector_insert_tail(vector, "6666666666");
    xu_vector_insert_tail(vector, "7777777777");
    xu_vector_insert_tail(vector, "8888888888");
    xu_vector_insert_tail(vector, "9999999999");
    xu_vector_mem_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("remove:");
    xu_vector_nremove_head(vector, 5);
    xu_vector_nremove_last(vector, 5);
    xu_vector_mem_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("replace:");
    xu_vector_nreplace_head(vector, "TTTTTTTTTT", 10);
    xu_vector_nreplace_last(vector, "HHHHHHHHHH", 10);
    xu_vector_replace_head(vector, "OOOOOOOOOO");
    xu_vector_replace_last(vector, "OOOOOOOOOO");
    xu_vector_mem_dump(vector);

    xu_trace_i("=============================================================");
    xu_trace_i("clear:");
    xu_vector_clear(vector);
    xu_vector_mem_dump(vector);

    xu_vector_exit(vector);
}
static xu_void_t xu_vector_perf_test()
{
    xu_size_t score = 0;
    xu_trace_i("=============================================================");
    xu_trace_i("insert performance:");
    score += xu_vector_insert_test();
    score += xu_vector_insert_head_test();
    score += xu_vector_insert_tail_test();
    score += xu_vector_ninsert_test();
    score += xu_vector_ninsert_head_test();
    score += xu_vector_ninsert_tail_test();

    xu_trace_i("=============================================================");
    xu_trace_i("remove performance:");
    score += xu_vector_remove_test();
    score += xu_vector_remove_head_test();
    score += xu_vector_remove_last_test();
    score += xu_vector_nremove_test();
    score += xu_vector_nremove_head_test();
    score += xu_vector_nremove_last_test();

    xu_trace_i("=============================================================");
    xu_trace_i("replace performance:");
    score += xu_vector_replace_test();
    score += xu_vector_replace_head_test();
    score += xu_vector_replace_last_test();
    score += xu_vector_nreplace_test();
    score += xu_vector_nreplace_head_test();
    score += xu_vector_nreplace_last_test();

    xu_trace_i("=============================================================");
    xu_trace_i("iterator performance:");
    score += xu_vector_iterator_next_test();
    score += xu_vector_iterator_prev_test();

    xu_trace_i("=============================================================");
    xu_trace_i("score: %lu", score / 100);
}
static xu_void_t xu_vector_test_itor_perf()
{
    // init vector
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_size());
    xu_assert_and_check_return(vector);

    // reset random
    xu_random_reset(xu_true);

    // add items
    __xu_volatile__ xu_size_t n = 10000;
    while (n--)
        xu_vector_insert_tail(vector, (xu_pointer_t)(xu_size_t)xu_random_value());

    // done
    xu_hong_t                 t       = xu_mclock();
    __xu_volatile__ xu_hize_t test[2] = {0};
    __xu_volatile__ xu_size_t itor    = xu_iterator_head(vector);
    for (; itor != xu_iterator_tail(vector);)
    {
        __xu_volatile__ xu_size_t item = (xu_size_t)xu_iterator_item(vector, itor);
        if (!(((xu_size_t)item >> 25) & 0x1))
        {
            // remove, hack: the itor of the same item is mutable
            xu_vector_remove(vector, itor);

            // continue
            continue;
        }
        else
        {
            test[0] += (xu_size_t)item;
            test[1]++;
        }

        itor = xu_iterator_next(vector, itor);
    }
    t = xu_mclock() - t;
    xu_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], xu_vector_size(vector), t);

    // exit
    xu_vector_exit(vector);
}
static xu_bool_t xu_vector_test_walk_item(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // done
    xu_hize_t* test = (xu_hize_t*)value;
    xu_size_t  i    = (xu_size_t)item;
    xu_bool_t  ok   = xu_false;
    if (!((i >> 25) & 0x1))
        ok = xu_true;
    else
    {
        test[0] += i;
        test[1]++;
    }

    // ok?
    return ok;
}
static xu_void_t xu_vector_test_walk_perf()
{
    // init vector
    xu_vector_ref_t vector = xu_vector_init(XU_VECTOR_GROW_SIZE, xu_element_long());
    xu_assert_and_check_return(vector);

    // reset random
    xu_random_reset(xu_true);

    // add items
    __xu_volatile__ xu_size_t n = 10000;
    while (n--)
        xu_vector_insert_tail(vector, (xu_pointer_t)(xu_size_t)xu_random_value());

    // done
    xu_hong_t                 t       = xu_mclock();
    __xu_volatile__ xu_hize_t test[2] = {0};
    xu_remove_if(vector, xu_vector_test_walk_item, (xu_pointer_t)test);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], xu_vector_size(vector), t);

    // exit
    xu_vector_exit(vector);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_vector_main(xu_int_t argc, xu_char_t** argv)
{
#if 1
    xu_vector_int_test();
    xu_vector_str_test();
    xu_vector_mem_test();
#endif

#if 1
    xu_vector_perf_test();
#endif

#if 1
    xu_vector_test_itor_perf();
    xu_vector_test_walk_perf();
#endif

    return 0;
}
