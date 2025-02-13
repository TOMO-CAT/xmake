#include "xutil/unit_test/ut.h"

/* *******************************************************
 * private implementation
 */
static xu_void_t xu_demo_process_test_run(xu_char_t** argv)
{
    xu_long_t ok = xu_process_run(argv[1], (xu_char_t const**)(argv + 1), xu_null);
    xu_trace_i("run: %s: %ld", argv[1], ok);
}
static xu_void_t xu_demo_process_test_pipe(xu_char_t** argv)
{
    // init pipe files
    xu_pipe_file_ref_t file[2] = {0};
    if (xu_pipe_file_init_pair(file, xu_null, 0))
    {
        // init process
        xu_process_attr_t attr   = {0};
        attr.out.pipe            = file[1];
        attr.outtype             = XU_PROCESS_REDIRECT_TYPE_PIPE;
        attr.err.pipe            = file[1];
        attr.errtype             = XU_PROCESS_REDIRECT_TYPE_PIPE;
        xu_process_ref_t process = xu_process_init(argv[1], (xu_char_t const**)(argv + 1), &attr);
        if (process)
        {
            // read pipe data
            xu_size_t read = 0;
            xu_byte_t data[8192];
            xu_size_t size = sizeof(data);
            xu_bool_t wait = xu_false;
            while (read < size)
            {
                xu_long_t real = xu_pipe_file_read(file[0], data + read, size - read);
                if (real > 0)
                {
                    read += real;
                    wait = xu_false;
                }
                else if (!real && !wait)
                {
                    // wait pipe
                    xu_long_t ok = xu_pipe_file_wait(file[0], XU_PIPE_EVENT_READ, 1000);
                    xu_check_break(ok > 0);
                    wait = xu_true;
                }
                else
                    break;
            }

            // dump data
            if (read) xu_dump_data(data, read);

            // wait process
            xu_long_t status = 0;
            xu_process_wait(process, &status, -1);

            // trace
            xu_trace_i("run: %s, status: %ld", argv[1], status);

            // exit process
            xu_process_exit(process);
        }

        // exit pipe files
        xu_pipe_file_exit(file[0]);
        xu_pipe_file_exit(file[1]);
    }
}
static xu_void_t xu_demo_process_test_waitlist(xu_char_t** argv)
{
    // init processes
    xu_size_t         count1        = 0;
    xu_process_ref_t  processes1[5] = {0};
    xu_process_ref_t  processes2[5] = {0};
    xu_process_attr_t attr          = {0};
    for (; count1 < 4; count1++)
    {
        attr.priv          = xu_u2p(count1);
        processes1[count1] = xu_process_init(argv[1], (xu_char_t const**)(argv + 1), &attr);
        xu_assert_and_check_break(processes1[count1]);
    }

    // ok?
    while (count1)
    {
        // trace
        xu_trace_i("waiting: %ld", count1);

        // wait processes
        xu_long_t             infosize    = -1;
        xu_process_waitinfo_t infolist[4] = {{0}};
        if ((infosize = xu_process_waitlist(processes1, infolist, xu_arrayn(infolist), -1)) > 0)
        {
            xu_size_t i = 0;
            for (i = 0; i < infosize; i++)
            {
                // trace
                xu_trace_i("process(%d:%p) exited: %d, priv: %p", infolist[i].index, infolist[i].process,
                           infolist[i].status, xu_process_priv(infolist[i].process));

                // exit process
                if (infolist[i].process) xu_process_exit(infolist[i].process);

                // remove this process
                processes1[infolist[i].index] = xu_null;
            }

            // update processes
            xu_size_t count2 = 0;
            for (i = 0; i < count1; i++)
            {
                if (processes1[i]) processes2[count2++] = processes1[i];
            }
            xu_memcpy(processes1, processes2, count2 * sizeof(xu_process_ref_t));
            processes1[count2] = xu_null;
            count1             = count2;
        }
    }
}
static xu_void_t xu_demo_process_test_exit(xu_char_t** argv, xu_bool_t detach)
{
    xu_size_t         i    = 0;
    xu_process_attr_t attr = {0};
    if (detach) attr.flags |= XU_PROCESS_FLAG_DETACH;
    for (i = 0; i < 10; i++)
        xu_process_init(argv[1], (xu_char_t const**)(argv + 1), &attr);

    // we attempt to enter or do ctrl+c and see process list in process monitor
    xu_getchar();
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_process_main(xu_int_t argc, xu_char_t** argv)
{
#if 0
    xu_demo_process_test_run(argv);
#else
    xu_used(xu_demo_process_test_run);
#endif

#if 0
    xu_demo_process_test_pipe(argv);
#else
    xu_used(xu_demo_process_test_pipe);
#endif

#if 0
    xu_demo_process_test_waitlist(argv);
#else
    xu_used(xu_demo_process_test_waitlist);
#endif

#if 1
    // we can run `xxx.bat` or `xxx.sh` shell command to test it
    // @see https://github.com/xmake-io/xmake/issues/719
    xu_demo_process_test_exit(argv, xu_false);
//    xu_demo_process_test_exit(argv, xu_true);
#else
    xu_used(xu_demo_process_test_exit);
#endif
    return 0;
}
