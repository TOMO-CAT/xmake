#pragma once

#include "xutil/platform/pipe.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 * *******************************************************
 */

// the process flag enum
typedef enum __xu_process_flag_e
{
    XU_PROCESS_FLAG_NONE    = 0,
    XU_PROCESS_FLAG_SUSPEND = 1 //!< suspend process
    ,
    XU_PROCESS_FLAG_DETACH = 2 //!< all subprocesses will be exited when the parent process is exited (ctrl+c or onexit)
                               //!< if this flag is not setted
} xu_process_flag_e;

// the process redirect type enum
typedef enum __xu_process_redirect_type_e
{
    XU_PROCESS_REDIRECT_TYPE_NONE     = 0,
    XU_PROCESS_REDIRECT_TYPE_FILEPATH = 1 //!< redirect to file path
    ,
    XU_PROCESS_REDIRECT_TYPE_FILE = 2 //!< redirect to file
    ,
    XU_PROCESS_REDIRECT_TYPE_PIPE = 3 //!< redirect to pipe

} xu_process_redirect_type_e;

// the process redirect io type
typedef union __xu_process_redirect_io_t
{
    xu_pipe_file_ref_t pipe;
    xu_file_ref_t      file;
    xu_char_t const*   path;

} xu_process_redirect_io_t;

// the process attribute type
typedef struct __xu_process_attr_t
{
    // the flags
    xu_uint16_t flags;

    // the stdin redirect type
    xu_uint16_t intype;

    // the stdin file mode
    xu_uint16_t inmode;

    // the stdout redirect type
    xu_uint16_t outtype;

    /*! the stdout file mode
     *
     * default: XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC
     *
     * support:
     *
     * - XU_FILE_MODE_WO
     * - XU_FILE_MODE_RW
     * - XU_FILE_MODE_CREAT
     * - XU_FILE_MODE_APPEND
     * - XU_FILE_MODE_TRUNC
     */
    xu_uint16_t outmode;

    // the stderr redirect type
    xu_uint16_t errtype;

    // the stderr file mode
    xu_uint16_t errmode;

    // the stdin
    xu_process_redirect_io_t in;

    // the stdout
    xu_process_redirect_io_t out;

    // the stderr
    xu_process_redirect_io_t err;

    /*! the environment
     *
     * @code

        xu_char_t const* envp[] =
        {
            "path=/usr/bin"
        ,   xu_null
        };

        attr.envp = envp;

     * @endcode
     *
     * the envp argument is an array of pointers to null-terminated strings
     * and must be terminated by a null pointer
     *
     * if the value of envp is null, then the child process inherits
     * the environment of the parent process.
     */
    xu_char_t const** envp;

    // the given current working directory for child process
    xu_char_t const* curdir;

    // the user private data
    xu_cpointer_t priv;

} xu_process_attr_t, *xu_process_attr_ref_t;

// the process ref type
typedef __xu_typeref__(process);

// the process wait info type
typedef struct __xu_process_waitinfo_t
{
    // the index of the processes
    xu_int_t index;

    // the status
    xu_int_t status;

    // the process
    xu_process_ref_t process;

} xu_process_waitinfo_t, *xu_process_waitinfo_ref_t;

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! run a given process
 *
 * @code

    // init argv
    xu_char_t const* argv[] =
    {
        "echo"
    ,   "hello"
    ,   xu_null
    };

    // init envp
    xu_char_t const* envp[] =
    {
        "path=/usr/bin"
    ,   xu_null
    };

    // init attr
    xu_process_attr_t attr = {0};
    attr.envp = envp;

    // run bash
    if (xu_process_run("echo", argv, &attr) == 0)
    {
        // trace
        xu_trace_i("ok");
    }

    // run bash
    if (xu_process_run("/bin/echo", xu_null, xu_null) == 0)
    {
        // trace
        xu_trace_i("ok");
    }

 * @endcode
 *
 * @param pathname      the process path or name
 * @param argv          the list of arguments must be terminated by a null pointer
 *                      and must be terminated by a null pointer
 *                      and argv[0] is the self path name
 * @param attr          the process attributes
 *
 * @return              the status value, failed: -1, ok: 0, other: error code
 */
xu_long_t xu_process_run(xu_char_t const* pathname, xu_char_t const* argv[], xu_process_attr_ref_t attr);

/*! run a given process from the command line
 *
 * @param cmd           the command line
 * @param attr          the process attributes
 *
 * @return              the status value, failed: -1, ok: 0, other: error code
 */
xu_long_t xu_process_run_cmd(xu_char_t const* cmd, xu_process_attr_ref_t attr);

/*! init a given process
 *
 * @code

    // init process
    xu_process_ref_t process = xu_process_init("/bin/echo", xu_null, xu_null);
    if (process)
    {
        // wait process
        xu_long_t status = 0;
        if (xu_process_wait(process, &status, 10) > 0)
        {
            // trace
            xu_trace_i("process exited: %ld", status);
        }
        // kill process
        else
        {
            // kill it
            xu_process_kill(process);

            // wait it again
            xu_process_wait(process, &status, -1);
        }

        // exit process
        xu_process_exit(process);
    }

 * @endcode
 *
 * @param pathname      the process path or name
 *
 * @param argv          the list of arguments must be terminated by a null pointer
 *                      and must be terminated by a null pointer
 *                      and argv[0] is the self path name
 *
 * @param attr          the process attributes
 *
 * @return              the process
 */
xu_process_ref_t xu_process_init(xu_char_t const* pathname, xu_char_t const* argv[], xu_process_attr_ref_t attr);

/*! init a given process from the command line
 *
 * @param cmd           the command line
 * @param attr          the process attributes
 *
 * @return              the process
 */
xu_process_ref_t xu_process_init_cmd(xu_char_t const* cmd, xu_process_attr_ref_t attr);

/*! exit the process
 *
 * @param process       the process
 */
xu_void_t xu_process_exit(xu_process_ref_t process);

/*! kill the process
 *
 * @param process       the process
 */
xu_void_t xu_process_kill(xu_process_ref_t process);

/*! get the user private data
 *
 * @param process       the process
 *
 * @return              the user private data
 */
xu_cpointer_t xu_process_priv(xu_process_ref_t process);

/*! set the user private data
 *
 * @param process       the process
 * @param priv          the user private data
 */
xu_void_t xu_process_priv_set(xu_process_ref_t process, xu_cpointer_t priv);

/*! resume the process
 *
 * @param process       the process
 */
xu_void_t xu_process_resume(xu_process_ref_t process);

/*! suspend the process
 *
 * @param process       the process
 */
xu_void_t xu_process_suspend(xu_process_ref_t process);

/*! wait the process
 *
 * @param process       the process
 * @param pstatus       the process exited status pointer, maybe null
 * @param timeout       the timeout (ms), infinity: -1
 *
 * @return              wait failed: -1, timeout: 0, ok: 1
 */
xu_long_t xu_process_wait(xu_process_ref_t process, xu_long_t* pstatus, xu_long_t timeout);

/*! wait the process list
 *
 * @code

    // init processes
    xu_size_t        count1 = 0;
    xu_process_ref_t processes1[5] = {0};
    xu_process_ref_t processes2[5] = {0};
    for (; count1 < 4; count1++)
    {
        processes1[count1] = xu_process_init(argv[1], (xu_char_t const**)(argv + 1), xu_null);
        xu_assert_and_check_break(processes1[count1]);
    }

    // ok?
    while (count1)
    {
        // trace
        xu_trace_i("waiting: %ld", count1);

        // wait processes
        xu_long_t               infosize = -1;
        xu_process_waitinfo_t   infolist[4];
        if ((infosize = xu_process_waitlist(processes1, infolist, xu_arrayn(infolist), -1)) > 0)
        {
            xu_size_t i = 0;
            for (i = 0; i < infosize; i++)
            {
                // trace
                xu_trace_i("process(%ld:%p) exited: %ld", infolist[i].index, infolist[i].process, infolist[i].status);

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
            count1 = count2;
        }
    }

 * @endcode
 *
 * @param processes     the null-terminated process list
 * @param infolist      the info list
 * @param infomaxn      the info maxn
 * @param timeout       the timeout (ms), infinity: -1
 *
 * @return              > 0: the info list size, 0: timeout, -1: failed
 */
xu_long_t xu_process_waitlist(xu_process_ref_t const* processes, xu_process_waitinfo_ref_t infolist, xu_size_t infomaxn,
                              xu_long_t timeout);
