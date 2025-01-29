/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "poller"
#define XU_TRACE_MODULE_DEBUG (1)

/* *******************************************************
 * includes
 */
#include "xutil/platform/poller.h"
#include "xutil/platform/impl/poller.h"
#include "xutil/platform/impl/pollerdata.h"
#include "xutil/platform/time.h"

/* *******************************************************
 * private implementation
 */
#if defined(XU_CONFIG_POSIX_HAVE_EPOLL_CREATE) && defined(XU_CONFIG_POSIX_HAVE_EPOLL_WAIT)
#    include "xutil/platform/linux/poller_epoll.c"
#    define XU_POLLER_ENABLE_EPOLL
#elif defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_BSD)
#    include "xutil/platform/bsd/poller_kqueue.c"
#    define XU_POLLER_ENABLE_KQUEUE
#elif defined(XU_CONFIG_POSIX_HAVE_POLL) &&                                                                            \
    !defined(XU_CONFIG_MICRO_ENABLE) /* TODO remove vector for supporting the micro mode */
#    include "xutil/platform/posix/poller_poll.c"
#    define XU_POLLER_ENABLE_POLL
#elif defined(XU_CONFIG_POSIX_HAVE_SELECT)
#    include "xutil/platform/posix/poller_select.c"
#    define XU_POLLER_ENABLE_SELECT
#endif

#ifndef XU_CONFIG_MICRO_ENABLE
#    if defined(XU_CONFIG_POSIX_HAVE_WAITPID) && defined(XU_CONFIG_LIBC_HAVE_SIGNAL)
#        include "xutil/platform/posix/poller_process.c"
#        define XU_POLLER_ENABLE_PROCESS
#    endif
#endif

#ifndef XU_CONFIG_MICRO_ENABLE
#    if defined(XU_CONFIG_LINUX_HAVE_INOTIFY_INIT) || defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_BSD)
#        include "xutil/algorithm/for_if.h"
#        include "xutil/container/vector.h"
#        include "xutil/platform/atomic.h"
#        include "xutil/platform/impl/poller.h"
#        include "xutil/platform/semaphore.h"
#        include "xutil/platform/spinlock.h"
#        include "xutil/platform/thread.h"

/* *******************************************************
 * types
 */
// the poller fwatcher type
typedef struct __xu_poller_fwatcher_t
{
    // the main poller
    xu_poller_t* main_poller;

    // the fwatcher poller thread
    xu_thread_ref_t thread;

    // is stopped?
    xu_atomic32_t is_stopped;

    // the lock
    xu_spinlock_t lock;

    // the fwatcher
    xu_fwatcher_ref_t fwatcher;

    // the user data
    xu_cpointer_t udata;

    // the semaphore
    xu_semaphore_ref_t semaphore;

    // the waiting event
    xu_fwatcher_event_t waiting_event;

    // the waited events
    xu_vector_ref_t waited_events;
    xu_vector_ref_t waited_events_copied;

} xu_poller_fwatcher_t;

/* *******************************************************
 * private implementation
 */

static xu_int_t xu_poller_fwatcher_loop(xu_cpointer_t priv)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)priv;
    xu_assert_and_check_return_val(poller && poller->semaphore && poller->waited_events, -1);

    // do loop
    xu_fwatcher_ref_t fwatcher = xu_null;
    while (!xu_atomic32_get(&poller->is_stopped))
    {
        // get fwatcher
        xu_spinlock_enter(&poller->lock);
        fwatcher = poller->fwatcher;
        xu_spinlock_leave(&poller->lock);

        if (fwatcher)
        {
            // wait events
            xu_long_t wait = xu_fwatcher_wait(fwatcher, &poller->waiting_event, -1);
            xu_assert_and_check_break(wait >= 0);
            xu_check_continue(wait > 0);

            // save waited event
            xu_spinlock_enter(&poller->lock);
            xu_vector_insert_tail(poller->waited_events, &poller->waiting_event);
            xu_spinlock_leave(&poller->lock);

            // notify the main poller to poll them
            xu_poller_t* main_poller = poller->main_poller;
            if (main_poller && main_poller->spak) main_poller->spak(main_poller);
        }
        else
        {
            // wait semaphore
            xu_long_t wait = xu_semaphore_wait(poller->semaphore, -1);
            xu_assert_and_check_break(wait >= 0);

            // interrupted? continue to wait
            xu_check_continue(wait != 0);
        }
    }

    // mark this thread is stopped
    xu_atomic32_set(&poller->is_stopped, 1);
    return 0;
}

/* *******************************************************
 * implementation
 */
static xu_void_t xu_poller_fwatcher_kill(xu_poller_fwatcher_ref_t self)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)self;
    xu_assert_and_check_return(poller && poller->semaphore);

    // trace
    xu_trace_d("fwatcher: kill ..");

    // stop thread and post it
    if (!xu_atomic32_fetch_and_set(&poller->is_stopped, 1))
    {
        if (poller->fwatcher) xu_fwatcher_spak(poller->fwatcher);
        xu_semaphore_post(poller->semaphore, 1);
    }
}
static xu_void_t xu_poller_fwatcher_exit(xu_poller_fwatcher_ref_t self)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)self;
    xu_assert_and_check_return(poller);

    // kill the fwatcher poller first
    xu_poller_fwatcher_kill(self);

    // exit the fwatcher poller thread
    if (poller->thread)
    {
        // wait it
        xu_long_t wait = 0;
        if ((wait = xu_thread_wait(poller->thread, 5000, xu_null)) <= 0)
        {
            // trace
            xu_trace_e("wait fwatcher poller thread failed: %ld!", wait);
        }

        // exit it
        xu_thread_exit(poller->thread);
        poller->thread = xu_null;
    }

    // exit waited events
    if (poller->waited_events) xu_vector_exit(poller->waited_events);
    if (poller->waited_events_copied) xu_vector_exit(poller->waited_events_copied);
    poller->waited_events        = xu_null;
    poller->waited_events_copied = xu_null;

    // exit semaphore
    if (poller->semaphore) xu_semaphore_exit(poller->semaphore);
    poller->semaphore = xu_null;

    // exit lock
    xu_spinlock_exit(&poller->lock);

    // reset fwatcher
    poller->fwatcher = xu_null;
    poller->udata    = xu_null;

    // exit poller
    xu_free(poller);
}
static xu_poller_fwatcher_ref_t xu_poller_fwatcher_init(xu_poller_t* main_poller)
{
    xu_bool_t             ok     = xu_false;
    xu_poller_fwatcher_t* poller = xu_null;
    do
    {
        // @note only support one fwatcher poller instance
        static xu_size_t s_poller_fwatcher_num = 0;
        if (s_poller_fwatcher_num++)
        {
            xu_trace_e("only support one fwatcher poller!");
            break;
        }

        // make the fwatcher poller
        poller = xu_malloc0_type(xu_poller_fwatcher_t);
        xu_assert_and_check_break(poller);

        // save the main poller
        poller->main_poller = main_poller;

        // init lock
        xu_spinlock_init(&poller->lock);

        // init semaphore
        poller->semaphore = xu_semaphore_init(0);
        xu_assert_and_check_break(poller->semaphore);

        // init waited events
        poller->waited_events        = xu_vector_init(0, xu_element_mem(sizeof(xu_fwatcher_event_t), xu_null, xu_null));
        poller->waited_events_copied = xu_vector_init(0, xu_element_mem(sizeof(xu_fwatcher_event_t), xu_null, xu_null));
        xu_assert_and_check_break(poller->waited_events && poller->waited_events_copied);

        // start the poller thread for fwatchers first
        poller->thread = xu_thread_init(xu_null, xu_poller_fwatcher_loop, poller, 0);
        xu_assert_and_check_break(poller->thread);

        // ok
        ok = xu_true;

    } while (0);

    // failed? exit the poller
    if (!ok)
    {
        if (poller) xu_poller_fwatcher_exit((xu_poller_fwatcher_ref_t)poller);
        poller = xu_null;
    }
    return (xu_poller_fwatcher_ref_t)poller;
}
static xu_void_t xu_poller_fwatcher_spak(xu_poller_fwatcher_ref_t self)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)self;
    xu_assert_and_check_return(poller && poller->semaphore);

    if (poller->fwatcher) xu_fwatcher_spak(poller->fwatcher);
    xu_semaphore_post(poller->semaphore, 1);
}
static xu_bool_t xu_poller_fwatcher_insert(xu_poller_fwatcher_ref_t self, xu_fwatcher_ref_t fwatcher,
                                           xu_cpointer_t priv)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)self;
    xu_assert_and_check_return_val(poller && poller->semaphore && fwatcher, xu_false);

    // attach fwatcher
    xu_bool_t ok     = xu_false;
    xu_bool_t notify = xu_false;
    xu_spinlock_enter(&poller->lock);
    xu_fwatcher_ref_t fwatcher_old = poller->fwatcher;
    if (!poller->fwatcher)
    {
        poller->fwatcher = fwatcher;
        poller->udata    = priv;
        ok               = xu_true;
        notify           = xu_true;
    }
    else if (poller->fwatcher == fwatcher)
    {
        poller->udata = priv;
        ok            = xu_true;
    }
    xu_spinlock_leave(&poller->lock);

    // notify thread to update fwatcher
    if (notify)
    {
        if (fwatcher_old) xu_fwatcher_spak(fwatcher_old);
        xu_semaphore_post(poller->semaphore, 1);
    }

    // we can insert only one fwatcher
    return ok;
}
static xu_bool_t xu_poller_fwatcher_modify(xu_poller_fwatcher_ref_t self, xu_fwatcher_ref_t fwatcher,
                                           xu_cpointer_t priv)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)self;
    xu_assert_and_check_return_val(poller && poller->semaphore && fwatcher, xu_false);

    // update fwatcher
    xu_bool_t notify = xu_false;
    xu_spinlock_enter(&poller->lock);
    xu_fwatcher_ref_t fwatcher_old = poller->fwatcher;
    if (poller->fwatcher != fwatcher)
    {
        poller->fwatcher = fwatcher;
        notify           = xu_true;
    }
    poller->udata = priv;
    xu_spinlock_leave(&poller->lock);

    // notify thread to update fwatcher
    if (notify)
    {
        if (fwatcher_old) xu_fwatcher_spak(fwatcher_old);
        xu_semaphore_post(poller->semaphore, 1);
    }
    return xu_true;
}
static xu_bool_t xu_poller_fwatcher_remove(xu_poller_fwatcher_ref_t self, xu_fwatcher_ref_t fwatcher)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)self;
    xu_assert_and_check_return_val(poller && poller->semaphore && fwatcher, xu_false);

    // remove fwatcher
    xu_bool_t ok     = xu_false;
    xu_bool_t notify = xu_false;
    xu_spinlock_enter(&poller->lock);
    xu_fwatcher_ref_t fwatcher_old = poller->fwatcher;
    if (poller->fwatcher == fwatcher)
    {
        poller->fwatcher = xu_null;
        poller->udata    = xu_null;
        ok               = xu_true;
        notify           = xu_true;
    }
    xu_spinlock_leave(&poller->lock);

    // notify thread to update fwatcher
    if (notify)
    {
        if (fwatcher_old) xu_fwatcher_spak(fwatcher_old);
        xu_semaphore_post(poller->semaphore, 1);
    }
    return ok;
}
static xu_bool_t xu_poller_fwatcher_wait_prepare(xu_poller_fwatcher_ref_t self)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)self;
    xu_assert_and_check_return_val(poller, xu_false);

    // is stopped?
    return !xu_atomic32_get(&poller->is_stopped) && poller->fwatcher;
}
static xu_long_t xu_poller_fwatcher_wait_poll(xu_poller_fwatcher_ref_t self, xu_poller_event_func_t func)
{
    // check
    xu_poller_fwatcher_t* poller = (xu_poller_fwatcher_t*)self;
    xu_assert_and_check_return_val(poller && func, -1);
    xu_assert_and_check_return_val(poller->waited_events && poller->waited_events_copied, -1);

    // get all waited events
    xu_vector_clear(poller->waited_events_copied);
    xu_spinlock_enter(&poller->lock);
    if (xu_vector_size(poller->waited_events))
    {
        xu_vector_copy(poller->waited_events_copied, poller->waited_events);
        xu_vector_clear(poller->waited_events);
    }
    xu_spinlock_leave(&poller->lock);

    // trace
    xu_trace_d("fwatcher: poll %lu", xu_vector_size(poller->waited_events_copied));

    // poll all waited events
    xu_long_t          wait = 0;
    xu_poller_object_t object;
    object.type         = XU_POLLER_OBJECT_FWATCHER;
    object.ref.fwatcher = poller->fwatcher;
    xu_for_all_if(xu_fwatcher_event_t*, event, poller->waited_events_copied, event)
    {
        func((xu_poller_ref_t)poller->main_poller, &object, (xu_long_t)event, poller->udata);
        wait++;
    }

    // trace
    xu_trace_d("fwatcher: poll wait %ld", wait);
    return wait;
}
#        define XU_POLLER_ENABLE_FWATCHER
#    endif
#endif

/* *******************************************************
 * implementation
 */
xu_poller_ref_t xu_poller_init(xu_cpointer_t priv)
{
    xu_bool_t    ok     = xu_false;
    xu_poller_t* poller = xu_null;
    do
    {
        // init poller
#if defined(XU_POLLER_ENABLE_EPOLL)
        poller = xu_poller_epoll_init();
#elif defined(XU_POLLER_ENABLE_KQUEUE)
        poller = xu_poller_kqueue_init();
#elif defined(XU_POLLER_ENABLE_IOCP)
        poller = xu_poller_iocp_init();
#elif defined(XU_POLLER_ENABLE_POLL)
        poller = xu_poller_poll_init();
#elif defined(XU_POLLER_ENABLE_SELECT)
        poller = xu_poller_select_init();
#endif
        xu_assert_and_check_break(poller);

        // save the user private data
        poller->priv = priv;

        // ok
        ok = xu_true;

    } while (0);

    // failed? exit the poller
    if (!ok)
    {
        if (poller) xu_poller_exit((xu_poller_ref_t)poller);
        poller = xu_null;
    }
    return (xu_poller_ref_t)poller;
}
xu_void_t xu_poller_exit(xu_poller_ref_t self)
{
    // check
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return(poller);

    // kill the poller first
    xu_poller_kill(self);

#ifdef XU_POLLER_ENABLE_PROCESS
    // exit the process poller
    if (poller->process_poller) xu_poller_process_exit(poller->process_poller);
    poller->process_poller = xu_null;
#endif

#ifdef XU_POLLER_ENABLE_FWATCHER
    // exit the fwatcher poller
    if (poller->fwatcher_poller) xu_poller_fwatcher_exit(poller->fwatcher_poller);
    poller->fwatcher_poller = xu_null;
#endif

    // exit poller
    if (poller->exit) poller->exit(poller);
}
xu_cpointer_t xu_poller_priv(xu_poller_ref_t self)
{
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return_val(poller, xu_null);

    return poller->priv;
}
xu_size_t xu_poller_type(xu_poller_ref_t self)
{
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return_val(poller, XU_POLLER_TYPE_NONE);

    return poller->type;
}
xu_void_t xu_poller_kill(xu_poller_ref_t self)
{
    // check
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return(poller);

#ifdef XU_POLLER_ENABLE_PROCESS
    // kill the process poller
    if (poller->process_poller) xu_poller_process_kill(poller->process_poller);
#endif

#ifdef XU_POLLER_ENABLE_FWATCHER
    // kill the fwatcher poller
    if (poller->fwatcher_poller) xu_poller_fwatcher_kill(poller->fwatcher_poller);
#endif

    // kill the poller
    if (poller->kill) poller->kill(poller);
}
xu_void_t xu_poller_spak(xu_poller_ref_t self)
{
    // check
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return(poller);

#ifdef XU_POLLER_ENABLE_PROCESS
    // spank the process poller
    if (poller->process_poller) xu_poller_process_spak(poller->process_poller);
#endif

#ifdef XU_POLLER_ENABLE_FWATCHER
    // spank the fwatcher poller
    if (poller->fwatcher_poller) xu_poller_fwatcher_spak(poller->fwatcher_poller);
#endif

    // spank the poller
    if (poller->spak) poller->spak(poller);
}
xu_bool_t xu_poller_support(xu_poller_ref_t self, xu_size_t events)
{
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return_val(poller, xu_false);

    return (poller->supported_events & events) == events;
}
xu_bool_t xu_poller_insert(xu_poller_ref_t self, xu_poller_object_ref_t object, xu_size_t events, xu_cpointer_t priv)
{
    // check
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return_val(poller && poller->insert && object, xu_false);

#ifdef XU_POLLER_ENABLE_PROCESS
    // is the process object?
    if (object->type == XU_POLLER_OBJECT_PROC)
    {
        // init the process poller first
        if (!poller->process_poller) poller->process_poller = xu_poller_process_init(poller);
        xu_assert_and_check_return_val(poller->process_poller, xu_false);

        // insert this process and the user private data
        return xu_poller_process_insert(poller->process_poller, object->ref.proc, priv);
    }
#else
    xu_assert_and_check_return_val(object->type != XU_POLLER_OBJECT_PROC, xu_false);
#endif

#ifdef XU_POLLER_ENABLE_FWATCHER
    // is the fwatcher object?
    if (object->type == XU_POLLER_OBJECT_FWATCHER)
    {
        // init the fwatcher poller first
        if (!poller->fwatcher_poller) poller->fwatcher_poller = xu_poller_fwatcher_init(poller);
        xu_assert_and_check_return_val(poller->fwatcher_poller, xu_false);

        // insert this fwatcher and the user private data
        return xu_poller_fwatcher_insert(poller->fwatcher_poller, object->ref.fwatcher, priv);
    }
#else
    xu_assert_and_check_return_val(object->type != XU_POLLER_OBJECT_FWATCHER, xu_false);
#endif

    // insert the poller object
    return poller->insert(poller, object, events, priv);
}
xu_bool_t xu_poller_remove(xu_poller_ref_t self, xu_poller_object_ref_t object)
{
    // check
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return_val(poller && poller->remove && object, xu_false);

#ifdef XU_POLLER_ENABLE_PROCESS
    // is the process object?
    if (object->type == XU_POLLER_OBJECT_PROC)
    {
        // remove this process and the user private data
        if (poller->process_poller) return xu_poller_process_remove(poller->process_poller, object->ref.proc);
        return xu_true;
    }
#else
    xu_assert_and_check_return_val(object->type != XU_POLLER_OBJECT_PROC, xu_false);
#endif

#ifdef XU_POLLER_ENABLE_FWATCHER
    // is the fwatcher object?
    if (object->type == XU_POLLER_OBJECT_FWATCHER)
    {
        // remove this fwatcher and the user private data
        if (poller->fwatcher_poller) return xu_poller_fwatcher_remove(poller->fwatcher_poller, object->ref.fwatcher);
        return xu_true;
    }
#else
    xu_assert_and_check_return_val(object->type != XU_POLLER_OBJECT_FWATCHER, xu_false);
#endif

    // remove the poller object
    return poller->remove(poller, object);
}
xu_bool_t xu_poller_modify(xu_poller_ref_t self, xu_poller_object_ref_t object, xu_size_t events, xu_cpointer_t priv)
{
    // check
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return_val(poller && poller->modify && object, xu_false);

#ifdef XU_POLLER_ENABLE_PROCESS
    // is the process object?
    if (object->type == XU_POLLER_OBJECT_PROC)
    {
        // modify the user private data of this process
        if (poller->process_poller) return xu_poller_process_modify(poller->process_poller, object->ref.proc, priv);
        return xu_true;
    }
#else
    xu_assert_and_check_return_val(object->type != XU_POLLER_OBJECT_PROC, xu_false);
#endif

#ifdef XU_POLLER_ENABLE_FWATCHER
    // is the fwatcher object?
    if (object->type == XU_POLLER_OBJECT_FWATCHER)
    {
        // modify the user private data of this fwatcher
        if (poller->fwatcher_poller)
            return xu_poller_fwatcher_modify(poller->fwatcher_poller, object->ref.fwatcher, priv);
        return xu_true;
    }
#else
    xu_assert_and_check_return_val(object->type != XU_POLLER_OBJECT_FWATCHER, xu_false);
#endif

    // modify the poller object
    return poller->modify(poller, object, events, priv);
}
xu_long_t xu_poller_wait(xu_poller_ref_t self, xu_poller_event_func_t func, xu_long_t timeout)
{
    // check
    xu_poller_t* poller = (xu_poller_t*)self;
    xu_assert_and_check_return_val(poller && poller->wait && func, -1);

#ifdef XU_POLLER_ENABLE_PROCESS
    // prepare to wait the processes
    if (poller->process_poller)
    {
        // prepare to wait processes
        if (!xu_poller_process_wait_prepare(poller->process_poller)) return -1;
    }
#endif

#ifdef XU_POLLER_ENABLE_FWATCHER
    // prepare to wait the fwatchers
    if (poller->fwatcher_poller)
    {
        // prepare to wait fwatchers
        if (!xu_poller_fwatcher_wait_prepare(poller->fwatcher_poller)) return -1;
    }
#endif

    // wait the poller objects
    xu_long_t wait = poller->wait(poller, func, timeout);
    xu_check_return_val(wait >= 0, -1);

#ifdef XU_POLLER_ENABLE_PROCESS
    // poll all waited processes
    if (poller->process_poller)
    {
        xu_long_t proc_wait = xu_poller_process_wait_poll(poller->process_poller, func);
        xu_check_return_val(proc_wait >= 0, -1);
        wait += proc_wait;
    }
#endif

#ifdef XU_POLLER_ENABLE_FWATCHER
    // poll all waited fwatchers
    if (poller->fwatcher_poller)
    {
        xu_long_t fwatcher_wait = xu_poller_fwatcher_wait_poll(poller->fwatcher_poller, func);
        xu_check_return_val(fwatcher_wait >= 0, -1);
        wait += fwatcher_wait;
    }
#endif
    return wait;
}
