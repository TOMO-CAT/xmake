/* *******************************************************
 *  trace
 */
#define XU_TRACE_MODULE_NAME "singleton"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/utils/singleton.h"
#include "xutil/libc/string/string.h"
#include "xutil/platform/atomic.h"
#include "xutil/platform/time.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 *  types
 */
// the singleton type
typedef struct __xu_singleton_t
{
    // the exit func
    xu_singleton_exit_func_t exit;

    // the kill func
    xu_singleton_kill_func_t kill;

    // the priv data
    xu_cpointer_t priv;

    // the instance
    xu_atomic_t instance;

} xu_singleton_t;

/* *******************************************************
 *  globals
 */
// the singletons
static xu_singleton_t g_singletons[XU_SINGLETON_TYPE_MAXN] = {{0}};

/* *******************************************************
 *  implementation
 */
xu_bool_t xu_singleton_init()
{
    /* init it
     *
     * @note
     * this is thread safe, because xu_singleton_init() only will be called in/before the xu_init()
     */
    static xu_bool_t binited = xu_false;
    if (!binited)
    {
        // init it
        xu_memset(&g_singletons, 0, sizeof(g_singletons));

        // ok
        binited = xu_true;
    }

    // ok
    return xu_true;
}
xu_void_t xu_singleton_kill()
{
    xu_size_t i = XU_SINGLETON_TYPE_MAXN;
    while (i--)
    {
        if (g_singletons[i].kill)
        {
            // the instance
            xu_handle_t instance = (xu_handle_t)xu_atomic_get(&g_singletons[i].instance);
            if (instance && instance != (xu_handle_t)1 && instance != (xu_handle_t)-1)
            {
                // trace
                xu_trace_d("instance: kill: %lu: ..", i);

                // kill it
                g_singletons[i].kill(instance, g_singletons[i].priv);
            }
        }
    }
}
xu_void_t xu_singleton_exit()
{
    // done
    xu_size_t i = XU_SINGLETON_TYPE_MAXN;
    while (i--)
    {
        if (g_singletons[i].exit)
        {
            // the instance
            xu_handle_t instance = (xu_handle_t)xu_atomic_fetch_and_set(&g_singletons[i].instance, 0);
            if (instance && instance != (xu_handle_t)1 && instance != (xu_handle_t)-1)
            {
                // trace
                xu_trace_d("instance: exit: %lu: ..", i);

                // exit it
                g_singletons[i].exit(instance, g_singletons[i].priv);
            }
        }
    }

    // clear it
    xu_memset(&g_singletons, 0, sizeof(g_singletons));
}
xu_handle_t xu_singleton_instance(xu_size_t type, xu_singleton_init_func_t init, xu_singleton_exit_func_t exit,
                                  xu_singleton_kill_func_t kill, xu_cpointer_t priv)
{
    // check, @note cannot use trace, assert and memory
    xu_check_return_val(type < XU_SINGLETON_TYPE_MAXN, xu_null);

    // the instance
    xu_handle_t instance = (xu_handle_t)xu_atomic_fetch_and_cmpset(&g_singletons[type].instance, 0, 1);

    // ok or failed?
    if (instance && instance != (xu_handle_t)1) return (instance != (xu_handle_t)-1) ? instance : xu_null;
    // null? init it
    else if (!instance)
    {
        // check
        xu_check_return_val(init && exit, xu_null);

        // init priv
        g_singletons[type].priv = priv;

        // init it
        instance = init(&g_singletons[type].priv);

        // init func
        g_singletons[type].exit = exit;
        g_singletons[type].kill = kill;

        // register instance
        xu_atomic_set(&g_singletons[type].instance, instance ? (xu_long_t)instance : (xu_long_t)-1);
    }
    // initing? wait it
    else
    {
        // try getting it
        xu_size_t tryn = 50;
        while ((instance = (xu_handle_t)xu_atomic_get(&g_singletons[type].instance)) && (instance == (xu_handle_t)1) &&
               tryn--)
        {
            // wait some time
            xu_msleep(100);
        }

        // failed?
        if (instance == (xu_handle_t)1 || instance == (xu_handle_t)-1) return xu_null;
    }

    // ok?
    return instance;
}
xu_bool_t xu_singleton_static_init(xu_atomic32_t* binited, xu_handle_t instance, xu_singleton_static_init_func_t init,
                                   xu_cpointer_t priv)
{
    // check
    xu_check_return_val(binited && instance, xu_false);

    // inited?
    xu_atomic32_t inited = xu_atomic32_fetch_and_cmpset(binited, 0, 1);

    // ok or failed?
    if (inited && inited != 1) return inited != -1;
    // null? init it
    else if (!inited)
    {
        // check
        xu_check_return_val(init, xu_false);

        // init it
        if (!init(instance, priv))
        {
            // failed
            xu_atomic32_set(binited, -1);
            return xu_false;
        }

        // init ok
        xu_atomic32_set(binited, 2);
    }
    // initing? wait it
    else
    {
        // try getting it
        xu_size_t tryn = 50;
        while ((1 == xu_atomic32_get(binited)) && tryn--)
        {
            // wait some time
            xu_msleep(100);
        }

        // failed?
        if (xu_atomic32_get(binited) != 2) return xu_false;
    }

    // ok?
    return xu_true;
}
