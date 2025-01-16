#pragma once

#include "xutil/macros/assert.h"
#include "xutil/macros/keyword.h"
#include "xutil/macros/type.h"
#include "xutil/platform/atomic.h"
#include "xutil/platform/sched.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */

// the initial value
#define XU_SPINLOCK_INIT XU_ATOMIC_FLAG_INIT

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

/*! init spinlock
 *
 * @param lock      the lock
 *
 * @return          xu_true or xu_false
 */
static __xu_inline_force__ xu_bool_t xu_spinlock_init(xu_spinlock_ref_t lock)
{
    // check
    xu_assert(lock);
    xu_atomic_flag_clear_explicit(lock, XU_ATOMIC_RELAXED);
    return xu_true;
}

/*! exit spinlock
 *
 * @param lock      the lock
 */
static __xu_inline_force__ xu_void_t xu_spinlock_exit(xu_spinlock_ref_t lock)
{
    // check
    xu_assert(lock);
    xu_atomic_flag_clear_explicit(lock, XU_ATOMIC_RELAXED);
}

/*! enter spinlock
 *
 * @param lock      the lock
 */
static __xu_inline_force__ xu_void_t xu_spinlock_enter(xu_spinlock_ref_t lock)
{
    // check
    xu_assert(lock);

    // init occupied
#ifdef XU_LOCK_PROFILER_ENABLE
    xu_bool_t occupied = xu_false;
#endif

    // get cpu count
#if defined(xu_cpu_pause) && !defined(XU_CONFIG_MICRO_ENABLE)
    xu_size_t ncpu = xu_cpu_count();
#endif

    // lock it
    while (1)
    {
        /* try non-atomic directly reading to reduce the performance loss of atomic synchronization,
         * this maybe read some dirty data, but only leads to enter wait state fast,
         * but does not affect to acquire lock.
         */
        if (!xu_atomic_flag_test_noatomic(lock) && !xu_atomic_flag_test_and_set(lock)) return;

#ifdef XU_LOCK_PROFILER_ENABLE
        // occupied
        if (!occupied)
        {
            // occupied++
            occupied = xu_true;
            xu_lock_profiler_occupied(xu_lock_profiler(), (xu_pointer_t)lock);

            // dump backtrace
#    if 0 // def __xu_debug__
            xu_backtrace_dump("spinlock", xu_null, 10);
#    endif
        }
#endif

#if defined(xu_cpu_pause) && !defined(XU_CONFIG_MICRO_ENABLE)
        if (ncpu > 1)
        {
            xu_size_t i, n;
            for (n = 1; n < 2048; n <<= 1)
            {
                /* spin_Lock:
                 *    cmp lockvar, 0   ; check if lock is free
                 *    je get_Lock
                 *    pause            ; wait for memory pipeline to become empty
                 *    jmp spin_Lock
                 * get_Lock:
                 *
                 * The PAUSE instruction will "de-pipeline" the memory reads,
                 * so that the pipeline is not filled with speculative CMP (2) instructions like in the first example.
                 * (I.e. it could block the pipeline until all older memory instructions are committed.)
                 * Because the CMP instructions (2) execute sequentially it is unlikely (i.e. the time window is much
                 * shorter) that an external write occurs after the CMP instruction (2) read lockvar but before the CMP
                 * is committed.
                 */
                for (i = 0; i < n; i++)
                    xu_cpu_pause();

                if (!xu_atomic_flag_test_noatomic(lock) && !xu_atomic_flag_test_and_set(lock)) return;
            }
        }
#endif
        xu_sched_yield();
    }
}

/*! enter spinlock without the lock profiler
 *
 * @param lock      the lock
 */
static __xu_inline_force__ xu_void_t xu_spinlock_enter_without_profiler(xu_spinlock_ref_t lock)
{
    // check
    xu_assert(lock);

    // lock it
#if defined(xu_cpu_pause) && !defined(XU_CONFIG_MICRO_ENABLE)
    xu_size_t ncpu = xu_cpu_count();
#endif
    while (1)
    {
        if (!xu_atomic_flag_test_noatomic(lock) && !xu_atomic_flag_test_and_set(lock)) return;

#if defined(xu_cpu_pause) && !defined(XU_CONFIG_MICRO_ENABLE)
        if (ncpu > 1)
        {
            xu_size_t i, n;
            for (n = 1; n < 2048; n <<= 1)
            {
                for (i = 0; i < n; i++)
                    xu_cpu_pause();

                if (!xu_atomic_flag_test_noatomic(lock) && !xu_atomic_flag_test_and_set(lock)) return;
            }
        }
#endif
        xu_sched_yield();
    }
}

/*! try to enter spinlock
 *
 * @param lock      the lock
 *
 * @return          xu_true or xu_false
 */
static __xu_inline_force__ xu_bool_t xu_spinlock_enter_try(xu_spinlock_ref_t lock)
{
    // check
    xu_assert(lock);

#ifndef XU_LOCK_PROFILER_ENABLE
    // try locking it
    return !xu_atomic_flag_test_and_set(lock);
#else
    // try locking it
    xu_bool_t ok = !xu_atomic_flag_test_and_set(lock);

    // occupied?
    if (!ok) xu_lock_profiler_occupied(xu_lock_profiler(), (xu_pointer_t)lock);

    // ok?
    return ok;
#endif
}

/*! try to enter spinlock without the lock profiler
 *
 * @param lock      the lock
 *
 * @return          xu_true or xu_false
 */
static __xu_inline_force__ xu_bool_t xu_spinlock_enter_try_without_profiler(xu_spinlock_ref_t lock)
{
    // check
    xu_assert(lock);

    // try locking it
    return !xu_atomic_flag_test_and_set(lock);
}

/*! leave spinlock
 *
 * @param lock      the lock
 */
static __xu_inline_force__ xu_void_t xu_spinlock_leave(xu_spinlock_ref_t lock)
{
    // check
    xu_assert(lock);

    // leave
    xu_atomic_flag_clear(lock);
}
