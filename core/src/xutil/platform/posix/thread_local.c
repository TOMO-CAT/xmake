#include "xutil/platform/thread_local.h"

#include "xutil/platform/spinlock.h"
#include "xutil/platform/thread.h"
#include <pthread.h>

extern xu_spinlock_t               g_thread_local_lock;
extern xu_single_list_entry_head_t g_thread_local_list;

// the once function
static xu_bool_t xu_thread_local_once(xu_cpointer_t priv)
{
    // check
    xu_value_ref_t tuple = (xu_value_ref_t)priv;
    xu_check_return_val(tuple, xu_false);

    // the thread local
    xu_thread_local_ref_t local = (xu_thread_local_ref_t)tuple[0].ptr;
    xu_check_return_val(local, xu_false);

    // save the free function
    local->free = (xu_thread_local_free_t)tuple[1].ptr;

    // check the pthread key space size
    xu_assert_static(sizeof(pthread_key_t) * 2 <= sizeof(local->priv));

    // create the pthread key for data
    xu_bool_t ok = pthread_key_create(&((pthread_key_t*)local->priv)[0], xu_null) == 0;
    if (ok)
    {
        // create the pthread key for mark
        ok = pthread_key_create(&((pthread_key_t*)local->priv)[1], xu_null) == 0;

        // failed? remove the data key
        if (!ok) pthread_key_delete(((pthread_key_t*)local->priv)[0]);
    }

    // save this thread local to list if ok
    if (ok)
    {
        xu_spinlock_enter(&g_thread_local_lock);
        xu_single_list_entry_insert_tail(&g_thread_local_list, &local->entry);
        xu_spinlock_leave(&g_thread_local_lock);
    }

    // init ok
    local->inited = xu_true;

    // ok?
    return ok;
}

xu_bool_t xu_thread_local_init(xu_thread_local_ref_t local, xu_thread_local_free_t func)
{
    // check
    xu_assert_and_check_return_val(local, xu_false);

    // run the once function
    xu_value_t tuple[2];
    tuple[0].ptr = (xu_pointer_t)local;
    tuple[1].ptr = (xu_pointer_t)func;
    return xu_thread_once(&local->once, xu_thread_local_once, tuple);
}
xu_void_t xu_thread_local_exit(xu_thread_local_ref_t local)
{
    // check
    xu_assert(local);

    // free the previous data first
    if (local->free && xu_thread_local_has(local)) local->free(xu_thread_local_get(local));

    // exit it
    pthread_key_delete(((pthread_key_t*)local->priv)[0]);
    pthread_key_delete(((pthread_key_t*)local->priv)[1]);
}
xu_bool_t xu_thread_local_has(xu_thread_local_ref_t local)
{
    // check
    xu_assert(local);

    // have been not initialized?
    xu_check_return_val(local->inited, xu_false);

    // get it
    return pthread_getspecific(((pthread_key_t*)local->priv)[1]) != xu_null;
}
xu_pointer_t xu_thread_local_get(xu_thread_local_ref_t local)
{
    // check
    xu_assert(local);

    // have been not initialized?
    xu_check_return_val(local->inited, xu_null);

    // get it
    return pthread_getspecific(((pthread_key_t*)local->priv)[0]);
}
xu_bool_t xu_thread_local_set(xu_thread_local_ref_t local, xu_cpointer_t priv)
{
    // check
    xu_assert(local);

    // have been not initialized?
    xu_assert_and_check_return_val(local->inited, xu_false);

    // free the previous data first
    if (local->free && xu_thread_local_has(local)) local->free(xu_thread_local_get(local));

    // set it
    xu_bool_t ok = pthread_setspecific(((pthread_key_t*)local->priv)[0], priv) == 0;
    if (ok)
    {
        /* mark exists and we use dummy variable to silence glibc 2.34 warnings
         * https://github.com/SELinuxProject/selinux/issues/311
         */
        xu_int_t dummy;
        ok = pthread_setspecific(((pthread_key_t*)local->priv)[1], &dummy) == 0;
    }
    return ok;
}
