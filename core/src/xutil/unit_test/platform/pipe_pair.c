#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */
#define BUFSIZE (8192 << 1)
#define COUNT (10000)

/* *******************************************************
 * implementation
 */
static xu_int_t xu_demo_pipe_pair_read(xu_cpointer_t priv)
{
    xu_pipe_file_ref_t file = (xu_pipe_file_ref_t)priv;
    xu_byte_t          data[BUFSIZE];
    xu_size_t          count = 0;
    while (1)
    {
        count++;
        if (!xu_pipe_file_bread(file, data, sizeof(data))) break;
        xu_trace_i("read %lu", count);
    }
    xu_pipe_file_exit(file);
    return 0;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_pipe_pair_main(xu_int_t argc, xu_char_t** argv)
{
    // test the anonymous pipe
    xu_pipe_file_ref_t file[2] = {};
    if (xu_pipe_file_init_pair(file, xu_null, 8192))
    {
        // start the read thread
        xu_thread_ref_t thread = xu_thread_init(xu_null, xu_demo_pipe_pair_read, file[0], 0);

        // write data to pipe
        xu_byte_t data[BUFSIZE];
        xu_size_t count = COUNT;
        while (count--)
        {
            xu_trace_i("write ..");
            if (!xu_pipe_file_bwrit(file[1], data, sizeof(data))) break;
        }

        // exit pipe files
        xu_pipe_file_exit(file[1]);

        // exit thread
        xu_thread_wait(thread, -1, xu_null);
        xu_thread_exit(thread);
    }
    return 0;
}
