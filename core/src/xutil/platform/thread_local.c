/* *******************************************************
 * trace
 */

#define XU_TRACE_MODULE_NAME "thread_local"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/platform/thread_local.h"
#include "xutil/algorithm/for_if.h"
#include "xutil/algorithm/walk.h"
#include "xutil/container/single_list_entry.h"
#include "xutil/platform/spinlock.h"

/* *******************************************************
 * globals
 */

// the thread local list
static xu_single_list_entry_head_t g_thread_local_list;

// the thread local list lock
static xu_spinlock_t g_thread_local_lock = XU_SPINLOCK_INIT;

/* *******************************************************
 * private implementation
 */
xu_bool_t xu_thread_local_init_env()
{
    // init lock
    if (!xu_spinlock_init(&g_thread_local_lock)) return xu_false;

    // init the thread local list
    xu_single_list_entry_init(&g_thread_local_list, xu_thread_local_t, entry, xu_null);

    // ok
    return xu_true;
}
xu_void_t xu_thread_local_exit_env()
{
    // enter lock
    xu_spinlock_enter(&g_thread_local_lock);

    // exit all thread locals
    xu_for_all_if(xu_thread_local_ref_t, local, xu_single_list_entry_itor(&g_thread_local_list), local)
    {
        // exit it
        xu_thread_local_exit(local);
    }

    // exit the thread local list
    xu_single_list_entry_exit(&g_thread_local_list);

    // leave lock
    xu_spinlock_leave(&g_thread_local_lock);

    // exit lock
    xu_spinlock_exit(&g_thread_local_lock);
}
xu_void_t xu_thread_local_walk(xu_walk_func_t func, xu_cpointer_t priv)
{
    // enter lock
    xu_spinlock_enter(&g_thread_local_lock);

    // walk all thread locals
    xu_walk_all(xu_single_list_entry_itor(&g_thread_local_list), func, priv);

    // leave lock
    xu_spinlock_leave(&g_thread_local_lock);
}

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_POSIX_HAVE_PTHREAD_SETSPECIFIC) && defined(XU_CONFIG_POSIX_HAVE_PTHREAD_GETSPECIFIC) &&          \
    defined(XU_CONFIG_POSIX_HAVE_PTHREAD_KEY_CREATE) && defined(XU_CONFIG_POSIX_HAVE_PTHREAD_KEY_DELETE)
#    include "xutil/platform/posix/thread_local.c"
#else
xu_bool_t xu_thread_local_init(xu_thread_local_ref_t local, xu_thread_local_free_t func)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_void_t xu_thread_local_exit(xu_thread_local_ref_t local) { xu_trace_noimpl(); }
xu_bool_t xu_thread_local_has(xu_thread_local_ref_t local)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_pointer_t xu_thread_local_get(xu_thread_local_ref_t local)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_bool_t xu_thread_local_set(xu_thread_local_ref_t local, xu_cpointer_t priv)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
