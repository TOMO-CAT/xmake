#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */
#define XU_DEMO_PIPE_NAME "hello"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_named_pipe_main(xu_int_t argc, xu_char_t** argv)
{
    // init buffer
    xu_size_t  length = 100 * 1024 * 1024;
    xu_byte_t* buffer = (xu_byte_t*)xu_malloc(length);
    xu_assert_and_check_return_val(buffer, -1);

    // is server?
    xu_bool_t is_server = argc > 1 && !xu_strcmp(argv[1], "server");
    if (is_server)
    {
        xu_pipe_file_ref_t pipe = xu_pipe_file_init(XU_DEMO_PIPE_NAME, XU_PIPE_MODE_WO, 0);
        if (pipe)
        {
            // trace
            xu_trace_i("connect pipe ..");

            // connect pipe first
            xu_long_t connected = -1;
            while (!(connected = xu_pipe_file_connect(pipe)))
            {
                xu_long_t wait = xu_pipe_file_wait(pipe, XU_PIPE_EVENT_CONN, -1);
                xu_assert_and_check_break(wait > 0);
            }
            if (connected > 0)
            {
                // trace
                xu_trace_i("write data to pipe ..");

                // write data to pipe
                xu_bool_t ok = xu_pipe_file_bwrit(pipe, buffer, length);
                xu_trace_i("write ok: %d", ok);
            }
            xu_pipe_file_exit(pipe);
        }
    }
    else
    {
        xu_pipe_file_ref_t pipe = xu_pipe_file_init(XU_DEMO_PIPE_NAME, XU_PIPE_MODE_RO, 0);
        if (pipe)
        {
            // trace
            xu_trace_i("read data to pipe ..");

            // read data from pipe
            xu_bool_t ok = xu_pipe_file_bread(pipe, buffer, length);
            xu_trace_i("read ok: %d", ok);
            xu_pipe_file_exit(pipe);
        }
    }
    // exit buffer
    xu_free(buffer);
    return 0;
}
