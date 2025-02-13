#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */

// the loop maxn
#define XU_TEST_LOOP_MAXN (20)

// the lock type
#define XU_TEST_LOCK_MUTEX
//#define XU_TEST_LOCK_SPINLOCK
//#define XU_TEST_LOCK_ATOMIC

/* *******************************************************
 * globals
 */
static __xu_volatile__ xu_atomic32_t g_value = 0;

/* *******************************************************
 * loop
 */
static xu_int_t xu_test_mutx_loop(xu_cpointer_t priv)
{
    // check
    xu_uint32_t self = (xu_uint32_t)xu_thread_self();
    xu_used(&self);

    // get lock
#if defined(XU_TEST_LOCK_MUTEX)
    xu_mutex_ref_t lock = (xu_mutex_ref_t)priv;
#elif defined(XU_TEST_LOCK_SPINLOCK)
    xu_spinlock_ref_t lock = (xu_spinlock_ref_t)priv;
#endif

    // get cpu core index
    static xu_size_t cpuidx = 0;
    xu_size_t        cpu    = cpuidx;
    cpuidx                  = (cpuidx + 1) % xu_cpu_count();

    // set thread affinity
    xu_cpuset_t cpuset;
    XU_CPUSET_ZERO(&cpuset);
    XU_CPUSET_SET(cpu, &cpuset);
    if (!xu_thread_setaffinity(xu_null, &cpuset))
    {
        xu_trace_e("thread[%x]: set cpu core(%zu) failed!", self, cpu);
    }

    // get cpu core again
    XU_CPUSET_ZERO(&cpuset);
    if (xu_thread_getaffinity(xu_null, &cpuset))
    {
        xu_size_t i;
        for (i = 0; i < XU_CPUSET_SIZE; i++)
        {
            if (XU_CPUSET_ISSET(i, &cpuset))
            {
                xu_trace_d("thread[%x]: init cpu core(%zu), cpu: %zu", self, i, cpu);
                break;
            }
        }
    }

    // loop
    __xu_volatile__ xu_size_t n = 100000;
    while (n--)
    {
#if defined(XU_TEST_LOCK_MUTEX)
        {
            // enter
            xu_mutex_enter(lock);

            // value++
            __xu_volatile__ xu_size_t n = 100;
            while (n--)
                g_value++;

            // leave
            xu_mutex_leave(lock);
        }
#elif defined(XU_TEST_LOCK_SPINLOCK)
        {
            // enter
            xu_spinlock_enter(lock);

            // value++
            __xu_volatile__ xu_size_t n = 100;
            while (n--)
                g_value++;

            // leave
            xu_spinlock_leave(lock);
        }
#elif defined(XU_TEST_LOCK_ATOMIC)
        xu_atomic32_fetch_and_add_explicit(&g_value, 1, XU_ATOMIC_RELAXED);
#else
        // value++
        __xu_volatile__ xu_size_t n = 100;
        while (n--)
            g_value++;
#endif
    }

    xu_trace_d("thread[%x]: exit cpu core(%zu)", self, cpu);
    return 0;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_lock_main(xu_int_t argc, xu_char_t** argv)
{
    // init lock
#if defined(XU_TEST_LOCK_MUTEX)
    xu_mutex_ref_t lock = xu_mutex_init();
    xu_lock_profiler_register(xu_lock_profiler(), (xu_pointer_t)lock, "demo_mutex");
#elif defined(XU_TEST_LOCK_SPINLOCK)
    xu_spinlock_t lock = XU_SPINLOCK_INIT;
    xu_lock_profiler_register(xu_lock_profiler(), (xu_pointer_t)&lock, "demo_spinlock");
#endif

    // init time
    xu_hong_t time = xu_mclock();

    // init loop
    xu_size_t       i                       = 0;
    xu_size_t       n                       = argv[1] ? xu_atoi(argv[1]) : XU_TEST_LOOP_MAXN;
    xu_thread_ref_t loop[XU_TEST_LOOP_MAXN] = {0};
    for (i = 0; i < n; i++)
    {
#if defined(XU_TEST_LOCK_MUTEX)
        loop[i] = xu_thread_init(xu_null, xu_test_mutx_loop, lock, 0);
#elif defined(XU_TEST_LOCK_SPINLOCK)
        loop[i] = xu_thread_init(xu_null, xu_test_mutx_loop, (xu_pointer_t)&lock, 0);
#else
        loop[i] = xu_thread_init(xu_null, xu_test_mutx_loop, xu_null, 0);
#endif
        xu_assert_and_check_break(loop[i]);
    }

    // exit thread
    for (i = 0; i < XU_TEST_LOOP_MAXN; i++)
    {
        // kill thread
        if (loop[i])
        {
            xu_thread_wait(loop[i], -1, xu_null);
            xu_thread_exit(loop[i]);
            loop[i] = xu_null;
        }

        // exit lock
#if defined(XU_TEST_LOCK_MUTEX)
        if (lock) xu_mutex_exit(lock);
        lock = xu_null;
#elif defined(XU_TEST_LOCK_SPINLOCK)
        xu_spinlock_exit(&lock);
#endif
    }

    // exit time
    time = xu_mclock() - time;

    // trace
    xu_trace_i("time: %lld ms", time);

    return 0;
}
