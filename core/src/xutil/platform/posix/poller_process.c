#include "xutil/algorithm/for_if.h"
#include "xutil/container/hash_map.h"
#include "xutil/container/vector.h"
#include "xutil/platform/impl/poller.h"
#include "xutil/platform/poller.h"

#include "xutil/platform/atomic.h"
#include "xutil/platform/process.h"
#include "xutil/platform/semaphore.h"
#include "xutil/platform/spinlock.h"
#include "xutil/platform/thread.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* *******************************************************
 * types
 */

// the processes data type
typedef struct __xu_poller_processes_data_t
{
    // the process reference
    xu_process_ref_t process;

    // the user private data
    xu_cpointer_t priv;

} xu_poller_processes_data_t;

// the processes status type
typedef struct __xu_poller_processes_status_t
{
    // the process id
    xu_int_t pid;

    // the process status
    xu_int_t status;

} xu_poller_processes_status_t;

// the poller process type
typedef struct __xu_poller_process_t
{
    // the main poller
    xu_poller_t* main_poller;

    // the process poller thread
    xu_thread_ref_t thread;

    // the waited processes data, pid => process and user private data
    xu_hash_map_ref_t processes_data;

    // the processes status
    xu_vector_ref_t processes_status;

    // the copied processes status
    xu_vector_ref_t processes_status_copied;

    // the semaphore
    xu_semaphore_ref_t semaphore;

    // is stopped?
    xu_atomic32_t is_stopped;

    // the lock
    xu_spinlock_t lock;

} xu_poller_process_t;

/* *******************************************************
 * declaration
 */

xu_int_t xu_process_pid(xu_process_ref_t self);

/* *******************************************************
 * globals
 */

// the global process poller
static xu_poller_process_t* g_process_poller = xu_null;

/* *******************************************************
 * private implementation
 */

static xu_int_t xu_poller_process_loop(xu_cpointer_t priv)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)priv;
    xu_assert_and_check_return_val(poller && poller->semaphore, -1);

    while (!xu_atomic32_get(&poller->is_stopped))
    {
        // trace
        xu_trace_d("process: waitpid ..");

        // wait semaphore
        xu_long_t wait = xu_semaphore_wait(poller->semaphore, -1);
        xu_assert_and_check_break(wait >= 0);

        // interrupted? continue to wait
        xu_check_continue(wait != 0);

        xu_int_t  result     = -1;
        xu_bool_t has_exited = xu_false;
        do
        {
            // poll processes
            xu_int_t status = -1;
            result          = waitpid(-1, &status, WNOHANG | WUNTRACED);

            // trace
            xu_trace_d("process: finished: %d, status: %d", result, status);

            // has exited process?
            if (result != 0 && result != -1)
            {
                /* get status, only get 8bits retval
                 *
                 * tt's limited to 8-bits, which means 1 byte,
                 * which means the int from WEXITSTATUS can only range from 0-255.
                 *
                 * in fact, any unix program will only ever return a max of 255.
                 */
                xu_poller_processes_status_t object_event;
                object_event.status = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
                object_event.pid    = result;

                // save the process status
                xu_spinlock_enter(&poller->lock);
                xu_vector_insert_tail(poller->processes_status, &object_event);
                has_exited = xu_true;
                xu_spinlock_leave(&poller->lock);
            }

        } while (result != 0 && result != -1);

        // has exited child processes? notify the main poller to poll them
        if (has_exited)
        {
            xu_poller_t* main_poller = poller->main_poller;
            if (main_poller && main_poller->spak) main_poller->spak(main_poller);
        }
    }

    // mark this thread is stopped
    xu_atomic32_set(&poller->is_stopped, 1);
    return 0;
}
static xu_void_t xu_poller_process_signal_handler(xu_int_t signo)
{
    // post semaphore to wait processes
    if (g_process_poller && g_process_poller->semaphore) xu_semaphore_post(g_process_poller->semaphore, 1);
}

/* *******************************************************
 * implementation
 */
static xu_void_t xu_poller_process_kill(xu_poller_process_ref_t self)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)self;
    xu_assert_and_check_return(poller && poller->semaphore);

    // trace
    xu_trace_d("process: kill ..");

    // stop thread and post it
    if (!xu_atomic32_fetch_and_set(&poller->is_stopped, 1)) xu_semaphore_post(poller->semaphore, 1);
}
static xu_void_t xu_poller_process_exit(xu_poller_process_ref_t self)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)self;
    xu_assert_and_check_return(poller);

    // kill the process poller first
    xu_poller_process_kill(self);

    // exit the process poller thread
    if (poller->thread)
    {
        // wait it
        xu_long_t wait = 0;
        if ((wait = xu_thread_wait(poller->thread, 5000, xu_null)) <= 0)
        {
            // trace
            xu_trace_e("wait process poller thread failed: %ld!", wait);
        }

        // exit it
        xu_thread_exit(poller->thread);
        poller->thread = xu_null;
    }

    // clear signal
    signal(SIGCHLD, SIG_DFL);
    g_process_poller = xu_null;

    // exit the processes data
    if (poller->processes_data) xu_hash_map_exit(poller->processes_data);
    poller->processes_data = xu_null;

    // exit the processes status
    if (poller->processes_status) xu_vector_exit(poller->processes_status);
    poller->processes_status = xu_null;

    if (poller->processes_status_copied) xu_vector_exit(poller->processes_status_copied);
    poller->processes_status_copied = xu_null;

    // exit semaphore
    if (poller->semaphore) xu_semaphore_exit(poller->semaphore);
    poller->semaphore = xu_null;

    // exit lock
    xu_spinlock_exit(&poller->lock);

    // exit poller
    xu_free(poller);
}
static xu_poller_process_ref_t xu_poller_process_init(xu_poller_t* main_poller)
{
    xu_bool_t            ok     = xu_false;
    xu_poller_process_t* poller = xu_null;
    do
    {
        // @note only support one process poller instance
        static xu_size_t s_poller_process_num = 0;
        if (s_poller_process_num++)
        {
            xu_trace_e("only support one process poller!");
            break;
        }

        // make the process poller
        poller = xu_malloc0_type(xu_poller_process_t);
        xu_assert_and_check_break(poller);

        // save the main poller
        poller->main_poller = main_poller;

        // init the processes data first
        poller->processes_data = xu_hash_map_init(XU_HASH_MAP_BUCKET_SIZE_MICRO, xu_element_uint32(),
                                                  xu_element_mem(sizeof(xu_poller_processes_data_t), xu_null, xu_null));
        xu_assert_and_check_break(poller->processes_data);

        // init semaphore
        poller->semaphore = xu_semaphore_init(0);
        xu_assert_and_check_break(poller->semaphore);

        // init lock
        xu_spinlock_init(&poller->lock);

        // init the processes status
        poller->processes_status =
            xu_vector_init(0, xu_element_mem(sizeof(xu_poller_processes_status_t), xu_null, xu_null));
        xu_assert_and_check_break(poller->processes_status);

        // init the copied processes status
        poller->processes_status_copied =
            xu_vector_init(0, xu_element_mem(sizeof(xu_poller_processes_status_t), xu_null, xu_null));
        xu_assert_and_check_break(poller->processes_status_copied);

        // start the poller thread for processes first
        poller->thread = xu_thread_init(xu_null, xu_poller_process_loop, poller, 0);
        xu_assert_and_check_break(poller->thread);

        // register signal
        signal(SIGCHLD, xu_poller_process_signal_handler);
        g_process_poller = poller;

        // ok
        ok = xu_true;

    } while (0);

    // failed? exit the poller
    if (!ok)
    {
        if (poller) xu_poller_process_exit((xu_poller_process_ref_t)poller);
        poller = xu_null;
    }
    return (xu_poller_process_ref_t)poller;
}
static xu_void_t xu_poller_process_spak(xu_poller_process_ref_t self)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)self;
    xu_assert_and_check_return(poller && poller->semaphore);

    // trace
    xu_trace_d("process: spak ..");

    // post it
    xu_semaphore_post(poller->semaphore, 1);
}
static xu_bool_t xu_poller_process_insert(xu_poller_process_ref_t self, xu_process_ref_t process, xu_cpointer_t priv)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)self;
    xu_assert_and_check_return_val(poller && poller->processes_data && process, xu_false);

    // get pid
    xu_int_t pid = xu_process_pid(process);
    xu_assert_and_check_return_val(pid != -1, xu_false);

    // trace
    xu_trace_d("process: insert pid: %d with priv: %p", pid, priv);

    // insert this process and the user private data
    xu_poller_processes_data_t procdata;
    procdata.process = process;
    procdata.priv    = priv;
    xu_hash_map_insert(poller->processes_data, xu_i2p(pid), &procdata);
    return xu_true;
}
static xu_bool_t xu_poller_process_modify(xu_poller_process_ref_t self, xu_process_ref_t process, xu_cpointer_t priv)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)self;
    xu_assert_and_check_return_val(poller && poller->processes_data && process, xu_false);

    // get pid
    xu_int_t pid = xu_process_pid(process);
    xu_assert_and_check_return_val(pid != -1, xu_false);

    // trace
    xu_trace_d("process: modify pid: %d with priv: %p", pid, priv);

    // modify the user private data of this process
    if (xu_hash_map_find(poller->processes_data, xu_i2p(pid)) != xu_iterator_tail(poller->processes_data))
    {
        xu_poller_processes_data_t procdata;
        procdata.process = process;
        procdata.priv    = priv;
        xu_hash_map_insert(poller->processes_data, xu_i2p(pid), &procdata);
    }
    return xu_true;
}
static xu_bool_t xu_poller_process_remove(xu_poller_process_ref_t self, xu_process_ref_t process)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)self;
    xu_assert_and_check_return_val(poller && poller->processes_data && process, xu_false);

    // get pid
    xu_int_t pid = xu_process_pid(process);
    xu_assert_and_check_return_val(pid != -1, xu_false);

    // trace
    xu_trace_d("process: remove pid: %d", pid);

    // remove this process and the user private data
    xu_hash_map_remove(poller->processes_data, xu_i2p(pid));
    return xu_true;
}
static xu_bool_t xu_poller_process_wait_prepare(xu_poller_process_ref_t self)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)self;
    xu_assert_and_check_return_val(poller && poller->processes_data && poller->semaphore, xu_false);

    // trace
    xu_trace_d("process: prepare %lu", xu_hash_map_size(poller->processes_data));

    // some processes maybe have been exited before starting the loop thread, so we need notify to waitpid
    if (xu_hash_map_size(poller->processes_data) && !xu_semaphore_post(poller->semaphore, 1)) return xu_false;

    // is stopped?
    return !xu_atomic32_get(&poller->is_stopped);
}
static xu_long_t xu_poller_process_wait_poll(xu_poller_process_ref_t self, xu_poller_event_func_t func)
{
    // check
    xu_poller_process_t* poller = (xu_poller_process_t*)self;
    xu_assert_and_check_return_val(poller && poller->processes_data && func, -1);
    xu_assert_and_check_return_val(poller->processes_status && poller->processes_status_copied, -1);

    // get all processes status
    xu_vector_clear(poller->processes_status_copied);
    xu_spinlock_enter(&poller->lock);
    if (xu_vector_size(poller->processes_status))
    {
        xu_vector_copy(poller->processes_status_copied, poller->processes_status);
        xu_vector_clear(poller->processes_status);
    }
    xu_spinlock_leave(&poller->lock);

    // trace
    xu_trace_d("process: poll %lu", xu_vector_size(poller->processes_status_copied));

    // poll all waited processes status
    xu_long_t          wait = 0;
    xu_poller_object_t object;
    object.type = XU_POLLER_OBJECT_PROC;
    xu_for_all_if(xu_poller_processes_status_t*, object_event, poller->processes_status_copied, object_event)
    {
        // trace
        xu_trace_d("process: pid: %d", object_event->pid);

        xu_poller_processes_data_t* proc_data = xu_hash_map_get(poller->processes_data, xu_i2p(object_event->pid));
        if (proc_data)
        {
            object.ref.proc = proc_data->process;
            func((xu_poller_ref_t)poller->main_poller, &object, object_event->status, proc_data->priv);
            wait++;
        }
    }

    // trace
    xu_trace_d("process: poll wait %ld", wait);
    return wait;
}
