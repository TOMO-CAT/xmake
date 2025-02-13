#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_filelock_main(xu_int_t argc, xu_char_t** argv)
{
    xu_filelock_ref_t lock = xu_filelock_init_from_path(
        argv[1], xu_file_info(argv[1], xu_null) ? XU_FILE_MODE_RO : XU_FILE_MODE_RW | XU_FILE_MODE_CREAT);
    if (lock)
    {
        // trace
        xu_trace_i("filelock: enter ..");

        // enter lock
        if (xu_filelock_enter(lock, (argv[2] && !xu_strcmp(argv[2], "sh")) ? XU_FILELOCK_MODE_SH : XU_FILELOCK_MODE_EX))
        {
            // trace
            xu_trace_i("filelock: enter ok");

            // wait ..
            xu_getchar();

            // leave lock
            xu_filelock_leave(lock);

            // trace
            xu_trace_i("filelock: leave ok");
        }
        xu_filelock_exit(lock);
    }
    return 0;
}
