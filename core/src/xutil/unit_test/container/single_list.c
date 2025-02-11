#include "xutil/unit_test/ut.h"

/* *******************************************************
 * test
 */
static xu_void_t xu_single_list_insert_next_test()
{
    // init list
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // make list
    xu_size_t itor = xu_single_list_insert_head(list, (xu_pointer_t)0xd);

    // done
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    xu_hong_t                 t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_single_list_insert_next(list, itor, (xu_pointer_t)0xd);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_insert_next(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // check
    xu_assert(xu_single_list_size(list) == n + 1);
    xu_assert(xu_single_list_head(list) == (xu_pointer_t)0xd);
    xu_assert(xu_single_list_last(list) == (xu_pointer_t)0xd);

    // clear it
    xu_single_list_clear(list);

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_insert_head_test()
{
    // init
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // done
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    xu_hong_t                 t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_single_list_insert_head(list, (xu_pointer_t)0xd);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // check
    xu_assert(xu_single_list_size(list) == n);
    xu_assert(xu_single_list_head(list) == (xu_pointer_t)0xd);
    xu_assert(xu_single_list_last(list) == (xu_pointer_t)0xd);

    // clear it
    xu_single_list_clear(list);

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_insert_tail_test()
{
    // init
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // done
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    xu_hong_t                 t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_single_list_insert_tail(list, (xu_pointer_t)0xd);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // check
    xu_assert(xu_single_list_size(list) == n);
    xu_assert(xu_single_list_head(list) == (xu_pointer_t)0xd);
    xu_assert(xu_single_list_last(list) == (xu_pointer_t)0xd);

    // clear it
    xu_single_list_clear(list);

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_remove_next_test()
{
    // init
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // make list
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    for (i = 0; i < n; i++)
        xu_single_list_insert_head(list, (xu_pointer_t)0xd);

    // done
    xu_size_t itor = xu_iterator_head(list);
    xu_hong_t t    = xu_mclock();
    for (i = 0, n--; i < n; i++)
        xu_single_list_remove_next(list, itor);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_remove(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // check
    xu_assert(1 == xu_single_list_size(list));

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_remove_head_test()
{
    // init
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // make list
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    for (i = 0; i < n; i++)
        xu_single_list_insert_head(list, (xu_pointer_t)0xd);

    // done
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_single_list_remove_head(list);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // check
    xu_assert(!xu_single_list_size(list));

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_replace_test()
{
    // init
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // make list
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    for (i = 0; i < n; i++)
        xu_single_list_insert_head(list, (xu_pointer_t)0xd);

    // done
    xu_size_t itor = xu_iterator_head(list);
    xu_size_t tail = xu_iterator_tail(list);
    xu_hong_t t    = xu_mclock();
    for (; itor != tail; itor = xu_iterator_next(list, itor))
        xu_single_list_replace(list, itor, (xu_pointer_t)0xe);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_replace(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // check
    xu_assert(xu_single_list_size(list) == n);
    xu_assert(xu_single_list_head(list) == (xu_pointer_t)0xe);
    xu_assert(xu_single_list_last(list) == (xu_pointer_t)0xe);

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_replace_head_test()
{
    // init
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // make list
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    for (i = 0; i < n; i++)
        xu_single_list_insert_head(list, (xu_pointer_t)0xd);

    // done
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_single_list_replace_head(list, (xu_pointer_t)0xe);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // check
    xu_assert(xu_single_list_size(list) == n);
    xu_assert(xu_single_list_head(list) == (xu_pointer_t)0xe);

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_replace_last_test()
{
    // init
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // make list
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    for (i = 0; i < n; i++)
        xu_single_list_insert_head(list, (xu_pointer_t)0xd);

    // done
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        xu_single_list_replace_last(list, (xu_pointer_t)0xe);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // check
    xu_assert(xu_single_list_size(list) == n);
    xu_assert(xu_single_list_last(list) == (xu_pointer_t)0xe);

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_iterator_next_test()
{
    // init
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_size());
    xu_assert_and_check_return(list);

    // make list
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 1000000;
    for (i = 0; i < n; i++)
        xu_single_list_insert_head(list, (xu_pointer_t)0xd);

    // done
    xu_hong_t t = xu_mclock();
    xu_for_all(xu_size_t, item, list) xu_used(item);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("xu_single_list_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, xu_single_list_size(list),
               xu_single_list_maxn(list));

    // exit
    xu_single_list_exit(list);
}
static xu_void_t xu_single_list_int_dump(xu_single_list_ref_t list)
{
    // trace
    xu_trace_i("xu_int_t size: %d, maxn: %d", xu_single_list_size(list), xu_single_list_maxn(list));

    // done
    xu_for_all(xu_char_t*, item, list)
    {
        // trace
        xu_trace_i("xu_int_t at[%lx]: %x", item_itor, item);
    }
}
static xu_void_t xu_single_list_int_test()
{
    // done
    xu_single_list_ref_t list = xu_null;
    xu_size_t            i;
    do
    {
        // init list
        list = xu_single_list_init(0, xu_element_long());
        xu_assert_and_check_break(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("insert:");

        // insert head
        i = xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);
        xu_single_list_insert_head(list, (xu_pointer_t)0xa);

        // insert tail
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);
        xu_single_list_insert_tail(list, (xu_pointer_t)0xf);

        // insert next
        xu_single_list_insert_next(list, i, (xu_pointer_t)0);
        xu_single_list_insert_next(list, i, (xu_pointer_t)1);
        xu_single_list_insert_next(list, i, (xu_pointer_t)2);
        xu_single_list_insert_next(list, i, (xu_pointer_t)3);
        xu_single_list_insert_next(list, i, (xu_pointer_t)4);
        xu_single_list_insert_next(list, i, (xu_pointer_t)5);
        xu_single_list_insert_next(list, i, (xu_pointer_t)6);
        xu_single_list_insert_next(list, i, (xu_pointer_t)7);
        xu_single_list_insert_next(list, i, (xu_pointer_t)8);
        xu_single_list_insert_next(list, i, (xu_pointer_t)9);

        // insert head
        xu_single_list_insert_head(list, (xu_pointer_t)4);
        xu_single_list_insert_head(list, (xu_pointer_t)3);
        xu_single_list_insert_head(list, (xu_pointer_t)2);
        xu_single_list_insert_head(list, (xu_pointer_t)1);
        xu_single_list_insert_head(list, (xu_pointer_t)0);

        // dump
        xu_single_list_int_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("remove:");

        // remove head
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);

        // remove
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);

        // dump
        xu_single_list_int_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("replace:");

        // replace head
        xu_single_list_replace_head(list, (xu_pointer_t)0);

        // replace last
        xu_single_list_replace_last(list, (xu_pointer_t)1);

        // dump
        xu_single_list_int_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("clear:");

        // clear
        xu_single_list_clear(list);

        // dump
        xu_single_list_int_dump(list);

    } while (0);

    // exit list
    if (list) xu_single_list_exit(list);
}
static xu_void_t xu_single_list_str_dump(xu_single_list_ref_t list)
{
    // trace
    xu_trace_i("str size: %d, maxn: %d", xu_single_list_size(list), xu_single_list_maxn(list));

    // done
    xu_for_all(xu_char_t*, item, list)
    {
        // trace
        xu_trace_i("str at[%lx]: %s", item_itor, item);
    }
}
static xu_void_t xu_single_list_str_test()
{
    // done
    xu_single_list_ref_t list = xu_null;
    xu_size_t            i;
    do
    {
        // init list
        list = xu_single_list_init(0, xu_element_str(xu_true));
        xu_assert_and_check_break(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("insert:");

        // insert head
        i = xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");

        // insert tail
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");

        // insert next
        xu_single_list_insert_next(list, i, "0000000000");
        xu_single_list_insert_next(list, i, "1111111111");
        xu_single_list_insert_next(list, i, "2222222222");
        xu_single_list_insert_next(list, i, "3333333333");
        xu_single_list_insert_next(list, i, "4444444444");
        xu_single_list_insert_next(list, i, "5555555555");
        xu_single_list_insert_next(list, i, "6666666666");
        xu_single_list_insert_next(list, i, "7777777777");
        xu_single_list_insert_next(list, i, "8888888888");
        xu_single_list_insert_next(list, i, "9999999999");

        // insert head
        xu_single_list_insert_head(list, "4444444444");
        xu_single_list_insert_head(list, "3333333333");
        xu_single_list_insert_head(list, "2222222222");
        xu_single_list_insert_head(list, "1111111111");
        xu_single_list_insert_head(list, "0000000000");

        // dump
        xu_single_list_str_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("remove:");

        // remove head
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);

        // remove
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);

        // dump
        xu_single_list_str_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("replace:");

        // replace head
        xu_single_list_replace_head(list, "0000000000");

        // replace last
        xu_single_list_replace_last(list, "1111111111");

        // dump
        xu_single_list_str_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("clear:");

        // clear
        xu_single_list_clear(list);

        // dump
        xu_single_list_str_dump(list);

    } while (0);

    // exit list
    if (list) xu_single_list_exit(list);
}
static xu_void_t xu_single_list_mem_free(xu_element_ref_t element, xu_pointer_t buff)
{
    xu_trace_i("ifm free: %s, priv: %s", buff, element->priv);
}
static xu_void_t xu_single_list_mem_dump(xu_single_list_ref_t list)
{
    // trace
    xu_trace_i("ifm size: %d, maxn: %d", xu_single_list_size(list), xu_single_list_maxn(list));

    // done
    xu_for_all(xu_char_t*, item, list)
    {
        // trace
        xu_trace_i("ifm at[%lx]: %s", item_itor, item);
    }
}
static xu_void_t xu_single_list_mem_test()
{
    // done
    xu_single_list_ref_t list = xu_null;
    xu_size_t            i;
    do
    {
        // init list
        list = xu_single_list_init(0, xu_element_mem(11, xu_single_list_mem_free, "mem"));
        xu_assert_and_check_break(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("insert:");

        // insert head
        i = xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");
        xu_single_list_insert_head(list, "AAAAAAAAAA");

        // insert tail
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");
        xu_single_list_insert_tail(list, "FFFFFFFFFF");

        // insert next
        xu_single_list_insert_next(list, i, "0000000000");
        xu_single_list_insert_next(list, i, "1111111111");
        xu_single_list_insert_next(list, i, "2222222222");
        xu_single_list_insert_next(list, i, "3333333333");
        xu_single_list_insert_next(list, i, "4444444444");
        xu_single_list_insert_next(list, i, "5555555555");
        xu_single_list_insert_next(list, i, "6666666666");
        xu_single_list_insert_next(list, i, "7777777777");
        xu_single_list_insert_next(list, i, "8888888888");
        xu_single_list_insert_next(list, i, "9999999999");

        // insert head
        xu_single_list_insert_head(list, "4444444444");
        xu_single_list_insert_head(list, "3333333333");
        xu_single_list_insert_head(list, "2222222222");
        xu_single_list_insert_head(list, "1111111111");
        xu_single_list_insert_head(list, "0000000000");

        // dump
        xu_single_list_mem_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("remove:");

        // remove head
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);
        xu_single_list_remove_head(list);

        // remove
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);
        xu_single_list_remove_next(list, i);

        // dump
        xu_single_list_mem_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("replace:");

        // replace head
        xu_single_list_replace_head(list, "0000000000");

        // replace last
        xu_single_list_replace_last(list, "1111111111");

        // dump
        xu_single_list_mem_dump(list);

        // trace
        xu_trace_i("=============================================================");
        xu_trace_i("clear:");

        // clear
        xu_single_list_clear(list);

        // dump
        xu_single_list_mem_dump(list);

    } while (0);

    // exit list
    if (list) xu_single_list_exit(list);
}
static xu_void_t xu_single_list_perf_test()
{
    // insert
    xu_single_list_insert_next_test();
    xu_single_list_insert_head_test();
    xu_single_list_insert_tail_test();

    // remove
    xu_single_list_remove_next_test();
    xu_single_list_remove_head_test();

    // replace
    xu_single_list_replace_test();
    xu_single_list_replace_head_test();
    xu_single_list_replace_last_test();

    // iterator
    xu_single_list_iterator_next_test();
}
static xu_bool_t xu_single_list_test_walk_item(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
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
static xu_void_t xu_single_list_walk_perf()
{
    // init list
    xu_single_list_ref_t list = xu_single_list_init(0, xu_element_long());
    xu_assert_and_check_return(list);

    // make list
    __xu_volatile__ xu_size_t n = 100000;
    while (n--)
        xu_single_list_insert_tail(list, (xu_pointer_t)(xu_size_t)xu_random_value());

    // done
    xu_hong_t                 t       = xu_mclock();
    __xu_volatile__ xu_hize_t test[2] = {0};
    xu_remove_if(list, xu_single_list_test_walk_item, (xu_pointer_t)test);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], xu_single_list_size(list), t);

    // exit list
    xu_single_list_exit(list);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_single_list_main(xu_int_t argc, xu_char_t** argv)
{
    xu_single_list_int_test();
    xu_single_list_str_test();
    xu_single_list_mem_test();

#if 1
    xu_single_list_perf_test();
#endif

#if 1
    xu_single_list_walk_perf();
#endif

    return 0;
}
