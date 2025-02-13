#include "xutil/unit_test/ut.h"

/* *******************************************************
 * types
 */
typedef struct __xu_demo_loop_t
{
    // the semaphore
    xu_semaphore_ref_t semaphore;

    // the loop
    xu_thread_ref_t loop;

    // the index
    xu_size_t index;

    // is stoped?
    xu_atomic_t bstoped;

} xu_demo_loop_t;

/* *******************************************************
 * thread
 */
static xu_char_t const* xu_demo_gets(xu_char_t* line, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(line && maxn, xu_null);

    // done
    xu_char_t* p = line;
    xu_char_t* e = line + maxn;
    while (p < e)
    {
        // get character
        xu_char_t ch = xu_getchar();
        if (ch == '\r') xu_getchar();
        xu_check_break(ch != '\r' && ch != '\n');

        // append digit
        if (xu_isdigit(ch))
            *p++ = ch;
        else
        {
            // trace
            xu_trace_e("invalid character: %x, please input digit!", ch);
        }
    }

    // end
    if (p < e) *p = '\0';

    // ok?
    return line;
}
static xu_int_t xu_demo_loop(xu_cpointer_t priv)
{
    // check
    xu_demo_loop_t* loop = (xu_demo_loop_t*)priv;

    // done
    do
    {
        // check
        xu_assert_and_check_break(loop);

        // trace
        xu_trace_i("[thread: %lu]: init", loop->index);

        // loop
        while (!xu_atomic_get(&loop->bstoped))
        {
            // wait
            xu_long_t wait = xu_semaphore_wait(loop->semaphore, -1);
            xu_assert_and_check_break(wait >= 0);

            // timeout?
            xu_check_continue(wait);

            // trace
            xu_trace_i("[semaphore: %lu]: wait: ok", loop->index);
        }

    } while (0);

    // trace
    xu_trace_i("[thread: %lu]: exit", loop ? loop->index : 0);

    // end
    return 0;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_semaphore_main(xu_int_t argc, xu_char_t** argv)
{
    // init loop
    xu_demo_loop_t loop[10];
    xu_size_t      i = 0;
    xu_size_t      n = xu_arrayn(loop);
    for (i = 0; i < n; i++)
    {
        // init semaphore
        loop[i].semaphore = xu_semaphore_init(0);
        xu_assert_and_check_break(loop[i].semaphore);

        // post semaphore
        xu_semaphore_post(loop[i].semaphore, 1);

        // init index
        loop[i].index = i;

        // init stoped
        loop[i].bstoped = 0;

        // init loop
        loop[i].loop = xu_thread_init(xu_null, xu_demo_loop, loop + i, 0);
        xu_assert_and_check_break(loop[i].loop);
    }

    // check
    xu_assert_and_check_return_val(i == n, 0);

    // wait some time
    xu_msleep(100);

    // post
    xu_char_t line[256];
    xu_bool_t stop = xu_false;
    while (!stop)
    {
        // get line
        xu_char_t const* p = xu_demo_gets(line, sizeof(line));
        xu_assert_and_check_break(p);

        // trace
        xu_trace_i("post: %s", p);

        // done
        while (*p && !stop)
        {
            xu_char_t ch = *p++;
            switch (ch)
            {
            case 'q': stop = xu_true; break;
            default:
            {
                if (ch >= '0' && ch <= '9')
                {
                    // the index
                    xu_size_t index = ch - '0';
                    xu_assert_and_check_break(index < n && index == loop[index].index);

                    // post semaphore
                    if (loop[index].semaphore) xu_semaphore_post(loop[index].semaphore, 1);
                }
            }
            break;
            }
        }
    }

    // post loop
    for (i = 0; i < n; i++)
    {
        // quit thread
        xu_atomic_set(&loop[i].bstoped, 1);

        // post semaphore
        if (loop[i].semaphore) xu_semaphore_post(loop[i].semaphore, 1);
    }

    // exit loop
    for (i = 0; i < n; i++)
    {
        // exit loop
        if (loop[i].loop)
        {
            // wait it
            if (!xu_thread_wait(loop[i].loop, 5000, xu_null))
            {
                // trace
                xu_trace_e("wait loop[%lu]: timeout", i);
            }

            // exit it
            xu_thread_exit(loop[i].loop);
        }

        // exit semaphore
        if (loop[i].semaphore) xu_semaphore_exit(loop[i].semaphore);
    }

    // exit
    xu_trace_i("exit");
    return 0;
}
