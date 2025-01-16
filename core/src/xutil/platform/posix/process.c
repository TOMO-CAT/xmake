#include "xutil/platform/process.h"
#include "xutil/algorithm/for_if.h"
#include "xutil/container/hash_map.h"
#include "xutil/container/hash_set.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"
#include "xutil/platform/environment.h"
#include "xutil/platform/file.h"
#include "xutil/platform/path.h"
#include "xutil/platform/spinlock.h"
#include "xutil/platform/time.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef XU_CONFIG_POSIX_HAVE_POSIX_SPAWNP
#    include <spawn.h>
#endif
#ifdef XU_CONFIG_LIBC_HAVE_KILL
#    include <signal.h>
#    include <sys/types.h>
#endif

#ifdef XU_CONFIG_OS_MACOSX
#    include <libproc.h>
#    include <sys/proc_info.h>
#endif

/* *******************************************************
 * macros
 * *******************************************************
 */

// pipe file to fd
#define xu_pipefile2fd(file) (xu_int_t)((file) ? (((xu_long_t)(file)) - 1) : -1)

/* *******************************************************
 * types
 * *******************************************************
 */

// the process type
typedef struct __xu_process_t
{
    // the pid
    pid_t pid;

    // is detached?
    xu_bool_t detached;

    // use spawn?
    xu_bool_t spawn;

    // the user private data
    xu_cpointer_t priv;

#ifdef XU_CONFIG_POSIX_HAVE_POSIX_SPAWNP
    // the spawn attributes
    posix_spawnattr_t spawn_attr;

    // the spawn action
    posix_spawn_file_actions_t spawn_action;
#endif

} xu_process_t;

/* *******************************************************
 * globals
 * *******************************************************
 */

// the user environment
extern xu_char_t** environ;

// the processes in the parent process group
static xu_hash_set_ref_t g_processes_group = xu_null;
static xu_spinlock_t     g_processes_lock  = XU_SPINLOCK_INIT;

/* *******************************************************
 * declaration
 * *******************************************************
 */

xu_int_t xu_process_pid(xu_process_ref_t self);

/* *******************************************************
 * private implementation
 * *******************************************************
 */

static xu_int_t xu_process_file_flags(xu_size_t mode)
{
    if (!mode) mode = XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC;

    xu_size_t flags = 0;
    if (mode & XU_FILE_MODE_RO)
        flags |= O_RDONLY;
    else if (mode & XU_FILE_MODE_WO)
        flags |= O_WRONLY;
    else if (mode & XU_FILE_MODE_RW)
        flags |= O_RDWR;
    if (mode & XU_FILE_MODE_CREAT) flags |= O_CREAT;
    if (mode & XU_FILE_MODE_APPEND) flags |= O_APPEND;
    if (mode & XU_FILE_MODE_TRUNC) flags |= O_TRUNC;
    return flags;
}
static xu_int_t xu_process_file_modes(xu_size_t mode)
{
    // no mode? uses the default mode
    if (!mode) mode = XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC;

    // make modes
    xu_size_t modes = 0;
    if (mode & XU_FILE_MODE_CREAT) modes = 0777;

    // ok?
    return modes;
}
#if defined(XU_CONFIG_OS_MACOSX)
static xu_bool_t xu_process_kill_allchilds(pid_t parent_pid)
{
    xu_assert_and_check_return_val(parent_pid > 0, xu_false);

    xu_int_t procs_count = proc_listpids(PROC_PPID_ONLY, parent_pid, xu_null, 0);
    xu_assert_and_check_return_val(procs_count >= 0, xu_false);

    if (procs_count > 0)
    {
        pid_t* pids = xu_nalloc0_type(procs_count, pid_t);
        xu_assert_and_check_return_val(pids, xu_false);

        proc_listpids(PROC_PPID_ONLY, parent_pid, pids, procs_count * sizeof(pid_t));
        for (xu_int_t i = 0; i < procs_count; ++i)
        {
            xu_check_continue(pids[i]);

#    ifdef __xu_debug__
            xu_char_t path[PROC_PIDPATHINFO_MAXSIZE] = {0};
            proc_pidpath(pids[i], path, sizeof(path));
            if (xu_strlen(path) > 0)
            {
                xu_trace_d("kill pid: %d, path: %s", pids[i], path);
            }
#    endif

            // kill subprocess
            kill(pids[i], SIGKILL);
        }

        xu_free(pids);
    }
    return xu_true;
}
#elif defined(XU_CONFIG_OS_LINUX)
static xu_bool_t xu_process_kill_allchilds(pid_t parent_pid)
{
    DIR* procdir = opendir("/proc");
    xu_assert_and_check_return_val(procdir, xu_false);

    xu_int_t       pid;
    xu_char_t      path[256];
    struct dirent* entry;
    while ((entry = readdir(procdir)))
    {
        if (entry->d_type == DT_DIR && (pid = xu_atoi(entry->d_name)) && pid > 0)
        {
            xu_int_t ret = xu_snprintf(path, sizeof(path), "/proc/%d/status", pid);
            if (ret > 0 && ret < sizeof(path)) path[ret] = '\0';

            FILE* fp = fopen(path, "r");
            if (fp)
            {
                xu_char_t line[128];
#    ifdef __xu_debug__
                xu_char_t name[128] = {0};
#    endif
                while (fgets(line, sizeof(line), fp))
                {
                    if (xu_strncmp(line, "PPid:", 5) == 0)
                    {
                        xu_char_t const* p = line + 5;
                        while (*p && xu_isspace(*p))
                            p++;
                        xu_int_t ppid = xu_atoi(p);
                        if (ppid == parent_pid)
                        {
                            xu_trace_d("kill pid: %d, ppid: %d, name: %s", pid, ppid, name);
                            xu_process_kill_allchilds(pid);
                            kill(pid, SIGKILL);
                        }
                    }
#    ifdef __xu_debug__
                    else if (xu_strncmp(line, "Name:", 5) == 0)
                    {
                        xu_char_t const* p = line + 5;
                        while (*p && xu_isspace(*p))
                            p++;
                        xu_strlcpy(name, p, sizeof(name));
                    }
#    endif
                }
                fclose(fp);
            }
        }
    }

    closedir(procdir);
    return xu_true;
}
#else
static xu_bool_t xu_process_kill_allchilds(pid_t pid) { return xu_false; }
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_int_t xu_process_pid(xu_process_ref_t self)
{
    // check
    xu_process_t* process = (xu_process_t*)self;
    xu_assert_and_check_return_val(process, -1);

    return (xu_int_t)process->pid;
}
xu_bool_t xu_process_group_init()
{
    if (!g_processes_group)
        g_processes_group = xu_hash_set_init(XU_HASH_MAP_BUCKET_SIZE_MICRO, xu_element_ptr(xu_null, xu_null));
    return g_processes_group != xu_null;
}
xu_void_t xu_process_group_exit()
{
    // we need not lock it in xu_exit()
    if (g_processes_group)
    {
        // send kill signal to all subprocesses
        // @note we do not destroy these leaked processes now.
        xu_for_all_if(xu_process_t*, process, g_processes_group, process)
        {
            xu_process_kill_allchilds(process->pid);
            xu_process_kill((xu_process_ref_t)process);
        }

        // exit it
        xu_hash_set_exit(g_processes_group);
        g_processes_group = xu_null;
    }
}
#ifdef XU_CONFIG_POSIX_HAVE_POSIX_SPAWNP
static xu_process_ref_t xu_process_init_spawn(xu_char_t const* pathname, xu_char_t const* argv[],
                                              xu_process_attr_ref_t attr)
{
    // check
    xu_assert_and_check_return_val(pathname, xu_null);

    // done
    xu_bool_t     ok      = xu_false;
    xu_process_t* process = xu_null;
    do
    {
        // make process
        process = xu_malloc0_type(xu_process_t);
        xu_assert_and_check_break(process);

        // save the user private data
        if (attr) process->priv = attr->priv;

        // init spawn attributes
        process->spawn = xu_true;
        posix_spawnattr_init(&process->spawn_attr);

        // init spawn action
        posix_spawn_file_actions_init(&process->spawn_action);

        // set attributes
        if (attr)
        {
            xu_int_t infd  = -1;
            xu_int_t outfd = -1;
            xu_int_t errfd = -1;

            // redirect the stdin
            if (attr->intype == XU_PROCESS_REDIRECT_TYPE_FILEPATH && attr->in.path)
            {
                // open stdin
                xu_int_t result = posix_spawn_file_actions_addopen(&process->spawn_action, STDIN_FILENO, attr->in.path,
                                                                   xu_process_file_flags(attr->inmode),
                                                                   xu_process_file_modes(attr->inmode));
                xu_assertf_pass_and_check_break(!result, "cannot redirect stdin to file: %s, error: %d", attr->in.path,
                                                result);
            }
            else if ((attr->intype == XU_PROCESS_REDIRECT_TYPE_PIPE && attr->in.pipe) ||
                     (attr->intype == XU_PROCESS_REDIRECT_TYPE_FILE && attr->in.file))
            {
                // duplicate inpipe/file fd to stdin in the child process
                infd = attr->intype == XU_PROCESS_REDIRECT_TYPE_PIPE ? xu_pipefile2fd(attr->in.pipe)
                                                                     : xu_file2fd(attr->in.file);
                posix_spawn_file_actions_adddup2(&process->spawn_action, infd, STDIN_FILENO);
            }

            // redirect the stdout
            if (attr->outtype == XU_PROCESS_REDIRECT_TYPE_FILEPATH && attr->out.path)
            {
                // open stdout
                xu_int_t result = posix_spawn_file_actions_addopen(&process->spawn_action, STDOUT_FILENO,
                                                                   attr->out.path, xu_process_file_flags(attr->outmode),
                                                                   xu_process_file_modes(attr->outmode));
                xu_assertf_pass_and_check_break(!result, "cannot redirect stdout to file: %s, error: %d",
                                                attr->out.path, result);
            }
            else if ((attr->outtype == XU_PROCESS_REDIRECT_TYPE_PIPE && attr->out.pipe) ||
                     (attr->outtype == XU_PROCESS_REDIRECT_TYPE_FILE && attr->out.file))
            {
                // duplicate outpipe/file fd to stdout in the child process
                outfd = attr->outtype == XU_PROCESS_REDIRECT_TYPE_PIPE ? xu_pipefile2fd(attr->out.pipe)
                                                                       : xu_file2fd(attr->out.file);
                posix_spawn_file_actions_adddup2(&process->spawn_action, outfd, STDOUT_FILENO);
            }

            // redirect the stderr
            if (attr->errtype == XU_PROCESS_REDIRECT_TYPE_FILEPATH && attr->err.path)
            {
                // open stderr
                xu_int_t result = posix_spawn_file_actions_addopen(&process->spawn_action, STDERR_FILENO,
                                                                   attr->err.path, xu_process_file_flags(attr->errmode),
                                                                   xu_process_file_modes(attr->errmode));
                xu_assertf_pass_and_check_break(!result, "cannot redirect stderr to file: %s, error: %d",
                                                attr->err.path, result);
            }
            else if ((attr->errtype == XU_PROCESS_REDIRECT_TYPE_PIPE && attr->err.pipe) ||
                     (attr->errtype == XU_PROCESS_REDIRECT_TYPE_FILE && attr->err.file))
            {
                // duplicate errpipe/file fd to stderr in the child process
                errfd = attr->errtype == XU_PROCESS_REDIRECT_TYPE_PIPE ? xu_pipefile2fd(attr->err.pipe)
                                                                       : xu_file2fd(attr->err.file);
                posix_spawn_file_actions_adddup2(&process->spawn_action, errfd, STDERR_FILENO);
            }

            // close fd
            if (infd != -1) posix_spawn_file_actions_addclose(&process->spawn_action, infd);
            if (outfd != -1) posix_spawn_file_actions_addclose(&process->spawn_action, outfd);
            if (errfd != -1 && errfd != outfd) posix_spawn_file_actions_addclose(&process->spawn_action, errfd);

                // change the current working directory for child process
#    ifdef XU_CONFIG_POSIX_HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR_NP
            if (attr->curdir)
            {
                xu_int_t result = posix_spawn_file_actions_addchdir_np(&process->spawn_action, attr->curdir);
                xu_assertf_pass_and_check_break(!result, "cannot change directory to %s, error: %d", attr->curdir,
                                                result);
            }
#    else
            xu_assertf(!attr->curdir, "posix_spawn do not support chdir!");
#    endif

            // suspend it first
            xu_int_t spawn_flags = 0;
            if (attr->flags & XU_PROCESS_FLAG_SUSPEND)
            {
#    ifdef POSIX_SPAWN_START_SUSPENDED
                spawn_flags |= POSIX_SPAWN_START_SUSPENDED;
#    else
                xu_assertf(0, "suspend process not supported!");
#    endif
            }

            // put the child in its own process group, so ctrl-c won't reach it.
            if (attr->flags & XU_PROCESS_FLAG_DETACH)
            {
                // no need to posix_spawnattr_setpgroup(&attr, 0), it's the default.
                spawn_flags |= POSIX_SPAWN_SETPGROUP;
                process->detached = xu_true;
            }

            // set spawn flags
            if (spawn_flags) posix_spawnattr_setflags(&process->spawn_attr, spawn_flags);
        }

        // no given environment? uses the current user environment
        xu_char_t const** envp = attr ? attr->envp : xu_null;
        if (!envp) envp = (xu_char_t const**)environ;

        // spawn the process
        xu_int_t status = posix_spawnp(&process->pid, pathname, &process->spawn_action, &process->spawn_attr,
                                       (xu_char_t* const*)argv, (xu_char_t* const*)envp);
        if (status != 0)
        {
            // pass the error code to parent/errno
            errno = status;
            break;
        }

        // check pid
        xu_assert_and_check_break(process->pid > 0);

        // save this pid if not detached
        if (!process->detached)
        {
            xu_spinlock_enter(&g_processes_lock);
            if (g_processes_group) xu_hash_set_insert(g_processes_group, process);
            xu_spinlock_leave(&g_processes_lock);
        }

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (process) xu_process_exit((xu_process_ref_t)process);
        process = xu_null;
    }

    // ok?
    return (xu_process_ref_t)process;
}
#endif
#ifndef XU_CONFIG_POSIX_HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR_NP
static xu_process_ref_t xu_process_init_fork(xu_char_t const* pathname, xu_char_t const* argv[],
                                             xu_process_attr_ref_t attr)
{
    // check
    xu_assert_and_check_return_val(pathname, xu_null);

    // done
    xu_bool_t     ok      = xu_false;
    xu_process_t* process = xu_null;
    do
    {
        // make process
        process = xu_malloc0_type(xu_process_t);
        xu_assert_and_check_break(process);

        // save the user private data
        if (attr) process->priv = attr->priv;

            // fork it
#    if defined(XU_CONFIG_POSIX_HAVE_VFORK) && defined(XU_CONFIG_POSIX_HAVE_EXECVPE)
        switch ((process->pid = vfork()))
#    else
        switch ((process->pid = fork()))
#    endif
        {
        case -1:

            // trace
            xu_trace_e("fork failed!");

            // exit it
            _exit(-1);

        case 0:

            // TODO
            // check
            xu_assertf(!attr || !(attr->flags & XU_PROCESS_FLAG_SUSPEND), "suspend process not supported!");

            // put the child in its own process group, so ctrl-c won't reach it.
            if (attr && attr->flags & XU_PROCESS_FLAG_DETACH)
            {
                setpgid(0, 0);
                process->detached = xu_true;
            }

            // set attributes
            if (attr)
            {
                xu_int_t infd  = -1;
                xu_int_t outfd = -1;
                xu_int_t errfd = -1;

                // redirect the stdin
                if (attr->intype == XU_PROCESS_REDIRECT_TYPE_FILEPATH && attr->in.path)
                {
                    // open file
                    infd =
                        open(attr->in.path, xu_process_file_flags(attr->inmode), xu_process_file_modes(attr->inmode));
                    xu_assertf_pass_and_check_break(infd, "cannot redirect stdin to file: %s, error: %d", attr->in.path,
                                                    errno);

                    // redirect it
                    dup2(infd, STDIN_FILENO);
                }
                else if ((attr->intype == XU_PROCESS_REDIRECT_TYPE_PIPE && attr->in.pipe) ||
                         (attr->intype == XU_PROCESS_REDIRECT_TYPE_FILE && attr->in.file))
                {
                    // duplicate inpipe fd to stdin in the child process
                    infd = attr->intype == XU_PROCESS_REDIRECT_TYPE_PIPE ? xu_pipefile2fd(attr->in.pipe)
                                                                         : xu_file2fd(attr->in.file);
                    dup2(infd, STDIN_FILENO);
                }

                // redirect the stdout
                if (attr->outtype == XU_PROCESS_REDIRECT_TYPE_FILEPATH && attr->out.path)
                {
                    // open file
                    outfd = open(attr->out.path, xu_process_file_flags(attr->outmode),
                                 xu_process_file_modes(attr->outmode));
                    xu_assertf_pass_and_check_break(outfd, "cannot redirect stdout to file: %s, error: %d",
                                                    attr->out.path, errno);

                    // redirect it
                    dup2(outfd, STDOUT_FILENO);
                }
                else if ((attr->outtype == XU_PROCESS_REDIRECT_TYPE_PIPE && attr->out.pipe) ||
                         (attr->outtype == XU_PROCESS_REDIRECT_TYPE_FILE && attr->out.file))
                {
                    // duplicate outpipe fd to stdout in the child process
                    outfd = attr->outtype == XU_PROCESS_REDIRECT_TYPE_PIPE ? xu_pipefile2fd(attr->out.pipe)
                                                                           : xu_file2fd(attr->out.file);
                    dup2(outfd, STDOUT_FILENO);
                }

                // redirect the stderr
                if (attr->errtype == XU_PROCESS_REDIRECT_TYPE_FILEPATH && attr->err.path)
                {
                    // open file
                    errfd = open(attr->err.path, xu_process_file_flags(attr->errmode),
                                 xu_process_file_modes(attr->errmode));
                    xu_assertf_pass_and_check_break(errfd, "cannot redirect stderr to file: %s, error: %d",
                                                    attr->err.path, errno);

                    // redirect it
                    dup2(errfd, STDERR_FILENO);
                }
                else if ((attr->errtype == XU_PROCESS_REDIRECT_TYPE_PIPE && attr->err.pipe) ||
                         (attr->errtype == XU_PROCESS_REDIRECT_TYPE_FILE && attr->err.file))
                {
                    // duplicate errpipe fd to stderr in the child process
                    errfd = attr->errtype == XU_PROCESS_REDIRECT_TYPE_PIPE ? xu_pipefile2fd(attr->err.pipe)
                                                                           : xu_file2fd(attr->err.file);
                    dup2(errfd, STDERR_FILENO);
                }

                // close fd
                if (infd != -1) close(infd);
                if (outfd != -1) close(outfd);
                if (errfd != -1 && errfd != outfd) close(errfd);

                // change the current working directory for child process
                if (attr->curdir && 0 != chdir(attr->curdir)) _exit(-1);
            }

            // get environment
            xu_char_t const** envp = attr ? attr->envp : xu_null;

#    if defined(XU_CONFIG_POSIX_HAVE_EXECVPE)
            // no given environment? uses the current user environment
            if (!envp) envp = (xu_char_t const**)environ;

            // exec it in the child process
            execvpe(pathname, (xu_char_t* const*)argv, (xu_char_t* const*)envp);
#    elif defined(XU_CONFIG_POSIX_HAVE_EXECVP)

            /* set environment variables
             *
             * uses fork because it will modify the parent environment
             */
            if (envp)
            {
                // done
                xu_char_t const* env = xu_null;
                while ((env = *envp++))
                {
                    // get name and values
                    xu_char_t const* p = xu_strchr(env, '=');
                    if (p)
                    {
                        // get name
                        xu_char_t name[256];
                        xu_size_t size = xu_min(p - env, sizeof(name) - 1);
                        xu_strncpy(name, env, size);
                        name[size] = '\0';

                        // get values
                        xu_char_t const* values = p + 1;

                        // set values to the environment
                        xu_environment_set(name, values);
                    }
                }
            }

            // exec it in the child process
            execvp(pathname, (xu_char_t* const*)argv);
#    else
#        error
#    endif
            // exit it
            _exit(-1);

        default:
            // parent
            break;
        }

        // check pid
        xu_assert_and_check_break(process->pid > 0);

        // save this pid if not detached
        if (!process->detached)
        {
            xu_spinlock_enter(&g_processes_lock);
            if (g_processes_group) xu_hash_set_insert(g_processes_group, process);
            xu_spinlock_leave(&g_processes_lock);
        }

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (process) xu_process_exit((xu_process_ref_t)process);
        process = xu_null;
    }

    // ok?
    return (xu_process_ref_t)process;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
xu_process_ref_t xu_process_init(xu_char_t const* pathname, xu_char_t const* argv[], xu_process_attr_ref_t attr)
{
#ifdef XU_CONFIG_POSIX_HAVE_POSIX_SPAWNP
#    ifndef XU_CONFIG_POSIX_HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR_NP
    if (attr && attr->curdir) return xu_process_init_fork(pathname, argv, attr);
#    endif
    return xu_process_init_spawn(pathname, argv, attr);
#else
    return xu_process_init_fork(pathname, argv, attr);
#endif
}
xu_process_ref_t xu_process_init_cmd(xu_char_t const* cmd, xu_process_attr_ref_t attr)
{
    // check
    xu_assert_and_check_return_val(cmd, xu_null);

    // done
    xu_process_ref_t  process          = xu_null;
    xu_char_t*        buffer           = xu_null;
    xu_char_t const** argv             = xu_null;
    xu_char_t const*  argv_buffer[256] = {xu_null};
    do
    {
        // make buffer
        xu_size_t maxn = XU_PATH_MAXN;
        buffer         = (xu_char_t*)xu_malloc(maxn);
        xu_assert_and_check_break(buffer);

        // copy and translate command
        xu_char_t ch;
        xu_size_t i         = 0;
        xu_size_t j         = 0;
        xu_size_t argv_maxn = 16;
        for (i = 0; j <= maxn && (ch = cmd[i]); i++)
        {
            // not enough? grow it
            if (j == maxn)
            {
                // grow it
                maxn += XU_PATH_MAXN;
                buffer = (xu_char_t*)xu_ralloc(buffer, maxn);
                xu_assert_and_check_break(buffer);
            }

            // translate "\"", "\'", "\\"
            xu_char_t next = cmd[i + 1];
            if (ch == '\\' && (next == '\"' || next == '\'' || next == '\\')) /* skip it */
                ;
            // copy it
            else
                buffer[j++] = ch;

            // guess the argv max count
            if (xu_isspace(ch)) argv_maxn++;
        }
        xu_assert_and_check_break(j < maxn);
        buffer[j] = '\0';

        // ensure the argv buffer
        if (argv_maxn <= xu_arrayn(argv_buffer))
        {
            argv      = argv_buffer;
            argv_maxn = xu_arrayn(argv_buffer);
        }
        else
        {
            // too large?
            if (argv_maxn > XU_MAXU16) argv_maxn = XU_MAXU16;

            // malloc the argv buffer
            argv = xu_nalloc0_type(argv_maxn, xu_char_t const*);
            xu_assert_and_check_break(argv);
        }

        // reset index
        i = 0;

        // parse command to the arguments
        xu_bool_t  s = 0;
        xu_size_t  m = argv_maxn;
        xu_char_t* p = buffer;
        xu_char_t* b = xu_null;
        while ((ch = *p))
        {
            // enter double quote?
            if (!s && ch == '\"') s = 2;
            // enter single quote?
            else if (!s && ch == '\'')
                s = 1;
            // leave quote?
            else if ((s == 2 && ch == '\"') || (s == 1 && ch == '\''))
                s = 0;
            // is argument end with ' '?
            else if (!s && xu_isspace(ch))
            {
                // fill zero
                *p = '\0';

                // save this argument
                if (b)
                {
                    // trace
                    xu_trace_d("argv: %s", b);

                    // save it
                    if (i < m - 1) argv[i++] = b;

                    // clear it
                    b = xu_null;
                }
            }

            // get the argument pointer
            if ((s || !xu_isspace(ch)) && !b) b = p;

            // next
            p++;
        }

        // save this argument
        if (b)
        {
            // trace
            xu_trace_d("argv: %s", b);

            // save it
            if (i < m - 1) argv[i++] = b;

            // clear it
            b = xu_null;
        }

        // check
        xu_assertf_and_check_break(i < m - 1, "the command(%s) arguments are too much!", cmd);

        // init process
        process = xu_process_init(argv[0], argv, attr);

    } while (0);

    // exit buffer
    if (buffer) xu_free(buffer);
    buffer = xu_null;

    // exit argv buffer
    if (argv != argv_buffer) xu_free(argv);
    argv = xu_null;

    // ok?
    return process;
}
xu_void_t xu_process_exit(xu_process_ref_t self)
{
    // check
    xu_process_t* process = (xu_process_t*)self;
    xu_assert_and_check_return(process);

    // remove this pid from the parent process group if not detached
    if (!process->detached)
    {
        xu_spinlock_enter(&g_processes_lock);
        if (g_processes_group) xu_hash_set_remove(g_processes_group, process);
        xu_spinlock_leave(&g_processes_lock);
    }

#ifdef XU_CONFIG_MODULE_HAVE_COROUTINE
    // attempt to cancel waiting from coroutine first
    xu_pointer_t       scheduler_io = xu_null;
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_PROC;
    object.ref.proc = self;
#    ifndef XU_CONFIG_MICRO_ENABLE
    if ((scheduler_io = xu_co_scheduler_io_self()) &&
        xu_co_scheduler_io_cancel((xu_co_scheduler_io_ref_t)scheduler_io, &object))
    {
    }
    else
#    endif
        if ((scheduler_io = xu_lo_scheduler_io_self()) &&
            xu_lo_scheduler_io_cancel((xu_lo_scheduler_io_ref_t)scheduler_io, &object))
    {
    }
#endif

#ifdef XU_CONFIG_POSIX_HAVE_POSIX_SPAWNP
    if (process->spawn)
    {
        // exit spawn attributes
        posix_spawnattr_destroy(&process->spawn_attr);

        // exit spawn action
        posix_spawn_file_actions_destroy(&process->spawn_action);
    }
#endif

    // exit it
    xu_free(process);
}
xu_void_t xu_process_kill(xu_process_ref_t self)
{
    // check
    xu_process_t* process = (xu_process_t*)self;
    xu_assert_and_check_return(process);

    // kill it
    if (process->pid > 0)
    {
#ifdef XU_CONFIG_LIBC_HAVE_KILL
        if (process->detached)
        {
            // send kill to every process in the subprocess group if the subprocess is detached
            kill(-process->pid, SIGKILL);
        }
        else
            kill(process->pid, SIGKILL);
#else
        // noimpl
        xu_trace_noimpl();
#endif
    }
}
xu_cpointer_t xu_process_priv(xu_process_ref_t self)
{
    // check
    xu_process_t* process = (xu_process_t*)self;
    xu_assert_and_check_return_val(process, xu_null);

    return process->priv;
}
xu_void_t xu_process_priv_set(xu_process_ref_t self, xu_cpointer_t priv)
{
    // check
    xu_process_t* process = (xu_process_t*)self;
    xu_assert_and_check_return(process);

    process->priv = priv;
}
xu_void_t xu_process_resume(xu_process_ref_t self)
{
    // check
    xu_process_t* process = (xu_process_t*)self;
    xu_assert_and_check_return(process);

    // resume it
    if (process->pid > 0)
    {
#ifdef XU_CONFIG_LIBC_HAVE_KILL
        kill(process->pid, SIGCONT);
#else
        // noimpl
        xu_trace_noimpl();
#endif
    }
}
xu_void_t xu_process_suspend(xu_process_ref_t self)
{
    // check
    xu_process_t* process = (xu_process_t*)self;
    xu_assert_and_check_return(process);

    // suspend it
    if (process->pid > 0)
    {
#ifdef XU_CONFIG_LIBC_HAVE_KILL
        kill(process->pid, SIGSTOP);
#else
        // noimpl
        xu_trace_noimpl();
#endif
    }
}
xu_long_t xu_process_wait(xu_process_ref_t self, xu_long_t* pstatus, xu_long_t timeout)
{
    // check
    xu_process_t* process = (xu_process_t*)self;
    xu_assert_and_check_return_val(process, -1);

#if defined(XU_CONFIG_MODULE_HAVE_COROUTINE) && !defined(XU_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine if timeout is non-zero
    if (timeout && xu_coroutine_self())
    {
        xu_poller_object_t object;
        object.type     = XU_POLLER_OBJECT_PROC;
        object.ref.proc = self;
        return xu_coroutine_waitproc(&object, pstatus, timeout);
    }
#endif

    // done
    xu_long_t ok   = 0;
    xu_hong_t time = xu_mclock();
    do
    {
        // wait it
        xu_int_t  status = -1;
        xu_long_t result = waitpid(process->pid, &status, timeout < 0 ? 0 : WNOHANG | WUNTRACED);
        xu_check_return_val(result != -1, -1);

        // exited?
        if (result != 0)
        {
            /* save status, only get 8bits retval
             *
             * tt's limited to 8-bits, which means 1 byte,
             * which means the int from WEXITSTATUS can only range from 0-255.
             *
             * in fact, any unix program will only ever return a max of 255.
             */
            if (pstatus) *pstatus = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

            // clear pid
            process->pid = 0;

            // wait ok
            ok = 1;

            // end
            break;
        }

        // wait some time
        if (timeout > 0) xu_msleep(xu_min(timeout, 60));

    } while (timeout > 0 && xu_mclock() - time < (xu_hong_t)timeout);

    // ok?
    return ok;
}
xu_long_t xu_process_waitlist(xu_process_ref_t const* processes, xu_process_waitinfo_ref_t infolist, xu_size_t infomaxn,
                              xu_long_t timeout)
{
    // check
    xu_assert_and_check_return_val(processes && infolist && infomaxn, -1);

    // done
    xu_long_t infosize = 0;
    xu_hong_t time     = xu_mclock();
    do
    {
        // wait it
        xu_int_t  status = -1;
        xu_long_t result = waitpid(-1, &status, timeout < 0 ? 0 : WNOHANG | WUNTRACED);
        xu_check_return_val(result != -1, -1);

        // exited?
        if (result != 0)
        {
            // find this process
            xu_process_t const** pprocess = (xu_process_t const**)processes;
            for (; *pprocess && (*pprocess)->pid != result; pprocess++)
                ;

            // found?
            if (*pprocess)
            {
                // save process info
                infolist[infosize].index   = (xu_process_ref_t const*)pprocess - processes;
                infolist[infosize].process = (xu_process_ref_t)*pprocess;
                infolist[infosize].status  = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
                infosize++;

                // attempt to wait other processes
                while (infosize < infomaxn)
                {
                    // attempt to wait it
                    status = -1;
                    result = waitpid(-1, &status, WNOHANG | WUNTRACED);

                    // error or timeout? end
                    xu_check_break(result != 0);

                    // find this process
                    xu_process_t const** pprocess = (xu_process_t const**)processes;
                    for (; *pprocess && (*pprocess)->pid != result; pprocess++)
                        ;

                    // found?
                    if (*pprocess)
                    {
                        // save process info
                        infolist[infosize].index   = (xu_process_ref_t const*)pprocess - processes;
                        infolist[infosize].process = (xu_process_ref_t)*pprocess;
                        infolist[infosize].status  = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
                        infosize++;
                    }
                    else
                        break;
                }

                // end
                break;
            }
        }

        /* wait some time, TODO we need use ISGCHLD and select to improve it
         * @see https://stackoverflow.com/questions/282176/waitpid-equivalent-with-timeout
         */
        if (timeout > 0) xu_msleep(xu_min(timeout, 60));

    } while (timeout > 0 && xu_mclock() - time < (xu_hong_t)timeout);

    // ok?
    return infosize;
}
