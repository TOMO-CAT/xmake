#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  includes
 */

#include "xutil/macros/type.h"

#if __xu_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__)
#    include "xutil/libc/atomic.h"
#elif defined(XU_COMPILER_IS_GCC) && defined(__ATOMIC_SEQ_CST)
#    include "xutil/compiler/gcc/atomic.h"
#endif

#include "xutil/platform/arch/atomic.h"

/* *******************************************************
 *  macros
 */

// the atomic flag initialize value
#ifndef XU_ATOMIC_FLAG_INIT
#    define XU_ATOMIC_FLAG_INIT                                                                                        \
        {                                                                                                              \
            0                                                                                                          \
        }
#endif

// no barriers or synchronization.
#ifndef XU_ATOMIC_RELAXED
#    define XU_ATOMIC_RELAXED (1)
#endif

// data dependency only for both barrier and synchronization with another thread.
#ifndef XU_ATOMIC_CONSUME
#    define XU_ATOMIC_CONSUME (2)
#endif

// barrier to hoisting of code and synchronizes with release (or stronger) semantic stores from another thread.
#ifndef XU_ATOMIC_ACQUIRE
#    define XU_ATOMIC_ACQUIRE (3)
#endif

// barrier to sinking of code and synchronizes with acquire (or stronger) semantic loads from another thread.
#ifndef XU_ATOMIC_RELEASE
#    define XU_ATOMIC_RELEASE (4)
#endif

// full barrier in both directions and synchronizes with acquire loads and release stores in another thread.
#ifndef XU_ATOMIC_ACQ_REL
#    define XU_ATOMIC_ACQ_REL (5)
#endif

// full barrier in both directions and synchronizes with acquire loads and release stores in all threads.
#ifndef XU_ATOMIC_SEQ_CST
#    define XU_ATOMIC_SEQ_CST (6)
#endif

// memory barrier (full barrier)
#ifndef XU_memory_barrier
#    define XU_memory_barrier()
#endif

#include "xutil/platform/atomic32.h"
#include "xutil/platform/atomic64.h"

/* *******************************************************
 *  macros
 */
/*! initializes the default-constructed atomic object obj with the value desired.
 *
 * the function is not atomic: concurrent access from another thread, even through an atomic operation, is a data race.
 */
#if XU_CPU_BIT64
#    define xu_atomic_init(a, v) xu_atomic64_init(a, (xu_int64_t)(v))
#else
#    define xu_atomic_init(a, v) xu_atomic32_init(a, (xu_int32_t)(v))
#endif

/*! atomically compares the contents of memory pointed to by obj with the contents of memory pointed to by expected,
 * and if those are bitwise equal, replaces the former with desired (performs read-modify-write operation).
 *
 * otherwise, loads the actual contents of memory pointed to by obj into *p (performs load operation).
 *
 * @param a     pointer to the atomic object to test and modify
 * @param p     pointer to the value expected to be found in the atomic object
 * @param v     the value to store in the atomic object if it is as expected
 *
 * @return the result of the comparison: true if *a was equal to *p, false otherwise.
 * - succ	    the memory synchronization ordering for the read-modify-write operation if the comparison succeeds. All
 * values are permitted.
 * - fail	    the memory synchronization ordering for the load operation if the comparison fails. Cannot be
 * memory_order_release or memory_order_acq_rel and cannot specify stronger ordering than succ
 *
 * @code
 *   xu_atomic_init(&a, 1);
 *
 *   xu_long_t expected = 1;
 *   if (xu_atomic_compare_and_swap(&a, &expected, 2)) {
 *      // *a = 2
 *   } else {
 *      // expected = *a
 *   }
 * @endcode
 */
#if XU_CPU_BIT64
#    define xu_atomic_compare_and_swap_explicit(a, p, v, succ, fail)                                                   \
        xu_atomic64_compare_and_swap_explicit(a, (xu_int64_t*)(p), (xu_int64_t)(v), succ, fail)
#    define xu_atomic_compare_and_swap(a, p, v) xu_atomic64_compare_and_swap(a, (xu_int64_t*)(p), (xu_int64_t)(v))
#else
#    define xu_atomic_compare_and_swap_explicit(a, p, v, succ, fail)                                                   \
        xu_atomic32_compare_and_swap_explicit(a, (xu_int32_t*)(p), (xu_int32_t)(v), succ, fail)
#    define xu_atomic_compare_and_swap(a, p, v) xu_atomic32_compare_and_swap(a, (xu_int32_t*)(p), (xu_int32_t)(v))
#endif

/*! like xu_atomic_compare_and_swap(), but it's allowed to fail spuriously, that is, act as if *obj != *p even if they
 * are equal.
 *
 * when a compare-and-swap is in a loop, the weak version will yield better performance on some platforms.
 * when a weak compare-and-swap would require a loop and a strong one would not, the strong one is preferable.
 *
 * @param a     pointer to the atomic object to test and modify
 * @param p     pointer to the value expected to be found in the atomic object
 * @param v     the value to store in the atomic object if it is as expected
 *
 * @return the result of the comparison: true if *a was equal to *p, false otherwise.
 * - succ	    the memory synchronization ordering for the read-modify-write operation if the comparison succeeds. All
 * values are permitted.
 * - fail	    the memory synchronization ordering for the load operation if the comparison fails. Cannot be
 * memory_order_release or memory_order_acq_rel and cannot specify stronger ordering than succ
 */
#if XU_CPU_BIT64
#    define xu_atomic_compare_and_swap_weak_explicit(a, p, v, succ, fail)                                              \
        xu_atomic64_compare_and_swap_weak_explicit(a, (xu_int64_t*)(p), (xu_int64_t)(v), succ, fail)
#    define xu_atomic_compare_and_swap_weak(a, p, v)                                                                   \
        xu_atomic64_compare_and_swap_weak(a, (xu_int64_t*)(p), (xu_int64_t)(v))
#else
#    define xu_atomic_compare_and_swap_weak_explicit(a, p, v, succ, fail)                                              \
        xu_atomic32_compare_and_swap_weak_explicit(a, (xu_int32_t*)(p), (xu_int32_t)(v), succ, fail)
#    define xu_atomic_compare_and_swap_weak(a, p, v)                                                                   \
        xu_atomic32_compare_and_swap_weak(a, (xu_int32_t*)(p), (xu_int32_t)(v))
#endif

// fetch the atomic value and compare and set value
#if XU_CPU_BIT64
#    define xu_atomic_fetch_and_cmpset_explicit(a, p, v, succ, fail)                                                   \
        (xu_long_t) xu_atomic64_fetch_and_cmpset_explicit(a, (xu_int64_t)(p), (xu_int64_t)(v), succ, fail)
#    define xu_atomic_fetch_and_cmpset(a, p, v)                                                                        \
        (xu_long_t) xu_atomic64_fetch_and_cmpset(a, (xu_int64_t)(p), (xu_int64_t)(v))
#else
#    define xu_atomic_fetch_and_cmpset_explicit(a, p, v, succ, fail)                                                   \
        (xu_long_t) xu_atomic32_fetch_and_cmpset_explicit(a, (xu_int32_t)(p), (xu_int32_t)(v), succ, fail)
#    define xu_atomic_fetch_and_cmpset(a, p, v)                                                                        \
        (xu_long_t) xu_atomic32_fetch_and_cmpset(a, (xu_int32_t)(p), (xu_long_t)(v))
#endif

// fetch the atomic value and set value
#if XU_CPU_BIT64
#    define xu_atomic_fetch_and_set_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic64_fetch_and_set_explicit(a, (xu_int64_t)(v), mo)
#    define xu_atomic_fetch_and_set(a, v) (xu_long_t) xu_atomic64_fetch_and_set(a, (xu_int64_t)(v))
#else
#    define xu_atomic_fetch_and_set_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic32_fetch_and_set_explicit(a, (xu_int32_t)(v), mo)
#    define xu_atomic_fetch_and_set(a, v) (xu_long_t) xu_atomic32_fetch_and_set(a, (xu_int32_t)(v))
#endif

// fetch the atomic value and compute add value
#if XU_CPU_BIT64
#    define xu_atomic_fetch_and_add_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic64_fetch_and_add_explicit(a, (xu_int64_t)(v), mo)
#    define xu_atomic_fetch_and_add(a, v) (xu_long_t) xu_atomic64_fetch_and_add(a, (xu_int64_t)(v))
#else
#    define xu_atomic_fetch_and_add_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic32_fetch_and_add_explicit(a, (xu_int32_t)(v), mo)
#    define xu_atomic_fetch_and_add(a, v) (xu_long_t) xu_atomic32_fetch_and_add(a, (xu_int32_t)(v))
#endif

// fetch the atomic value and compute sub value
#if XU_CPU_BIT64
#    define xu_atomic_fetch_and_sub_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic64_fetch_and_sub_explicit(a, (xu_int64_t)(v), mo)
#    define xu_atomic_fetch_and_sub(a, v) (xu_long_t) xu_atomic64_fetch_and_sub(a, (xu_int64_t)(v))
#else
#    define xu_atomic_fetch_and_sub_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic32_fetch_and_sub_explicit(a, (xu_int32_t)(v), mo)
#    define xu_atomic_fetch_and_sub(a, v) (xu_long_t) xu_atomic32_fetch_and_sub(a, (xu_int32_t)(v))
#endif

// fetch the atomic value and compute or value
#if XU_CPU_BIT64
#    define xu_atomic_fetch_and_or_explicit(a, v, mo)                                                                  \
        (xu_long_t) xu_atomic64_fetch_and_or_explicit(a, (xu_int64_t)(v), mo)
#    define xu_atomic_fetch_and_or(a, v) (xu_long_t) xu_atomic64_fetch_and_or(a, (xu_int64_t)(v))
#else
#    define xu_atomic_fetch_and_or_explicit(a, v, mo)                                                                  \
        (xu_long_t) xu_atomic32_fetch_and_or_explicit(a, (xu_int32_t)(v), mo)
#    define xu_atomic_fetch_and_or(a, v) (xu_long_t) xu_atomic32_fetch_and_or(a, (xu_int32_t)(v))
#endif

// fetch the atomic value and compute xor operation
#if XU_CPU_BIT64
#    define xu_atomic_fetch_and_xor_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic64_fetch_and_xor_explicit(a, (xu_int64_t)(v), mo)
#    define xu_atomic_fetch_and_xor(a, v) (xu_long_t) xu_atomic64_fetch_and_xor(a, (xu_int64_t)(v))
#else
#    define xu_atomic_fetch_and_xor_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic32_fetch_and_xor_explicit(a, (xu_int32_t)(v), mo)
#    define xu_atomic_fetch_and_xor(a, v) (xu_long_t) xu_atomic32_fetch_and_xor(a, (xu_int32_t)(v))
#endif

// fetch the atomic value and compute and operation
#if XU_CPU_BIT64
#    define xu_atomic_fetch_and_and_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic64_fetch_and_and_explicit(a, (xu_int64_t)(v), mo)
#    define xu_atomic_fetch_and_and(a, v) (xu_long_t) xu_atomic64_fetch_and_and(a, (xu_int64_t)(v))
#else
#    define xu_atomic_fetch_and_and_explicit(a, v, mo)                                                                 \
        (xu_long_t) xu_atomic32_fetch_and_and_explicit(a, (xu_int32_t)(v), mo)
#    define xu_atomic_fetch_and_and(a, v) (xu_long_t) xu_atomic32_fetch_and_and(a, (xu_int32_t)(v))
#endif

// get the atomic value
#if XU_CPU_BIT64
#    define xu_atomic_get_explicit(a, mo) (xu_long_t) xu_atomic64_get_explicit(a, mo)
#    define xu_atomic_get(a) (xu_long_t) xu_atomic64_get(a)
#else
#    define xu_atomic_get_explicit(a, mo) (xu_long_t) xu_atomic32_get_explicit(a, mo)
#    define xu_atomic_get(a) (xu_long_t) xu_atomic32_get(a)
#endif

// set the atomic value
#if XU_CPU_BIT64
#    define xu_atomic_set_explicit(a, v, mo) xu_atomic64_set_explicit(a, (xu_int64_t)(v), mo)
#    define xu_atomic_set(a, v) xu_atomic64_set(a, (xu_int64_t)(v))
#else
#    define xu_atomic_set_explicit(a, v, mo) xu_atomic32_set_explicit(a, (xu_int32_t)(v), mo)
#    define xu_atomic_set(a, v) xu_atomic32_set(a, (xu_int32_t)(v))
#endif

// sets an atomic_flag to true and returns the old value
#ifndef xu_atomic_flag_test_and_set_explicit
#    define xu_atomic_flag_test_and_set_explicit_generic_impl
#    define xu_atomic_flag_test_and_set_explicit(a, mo) xu_atomic_flag_test_and_set_explicit_generic(a, mo)
#endif
#ifndef xu_atomic_flag_test_and_set
#    define xu_atomic_flag_test_and_set(a) xu_atomic_flag_test_and_set_explicit(a, XU_ATOMIC_SEQ_CST)
#endif

// returns the test result of an atomic_flag
#ifndef xu_atomic_flag_test_explicit
#    define xu_atomic_flag_test_explicit_generic_impl
#    define xu_atomic_flag_test_explicit(a, mo) xu_atomic_flag_test_explicit_generic(a, mo)
#endif
#ifndef xu_atomic_flag_test
#    define xu_atomic_flag_test(a) xu_atomic_flag_test_explicit(a, XU_ATOMIC_SEQ_CST)
#endif

// sets an atomic_flag to false
#ifndef xu_atomic_flag_clear_explicit
#    define xu_atomic_flag_clear_explicit_generic_impl
#    define xu_atomic_flag_clear_explicit(a, mo) xu_atomic_flag_clear_explicit_generic(a, mo)
#endif
#ifndef xu_atomic_flag_clear
#    define xu_atomic_flag_clear(a) xu_atomic_flag_clear_explicit(a, XU_ATOMIC_SEQ_CST)
#endif

// get value of an atomic_flag directly (non-atomic)
#ifndef xu_atomic_flag_test_noatomic
#    define xu_atomic_flag_test_noatomic(a) ((a)->__val)
#endif

/* *******************************************************
 *  inline implementation
 */
#ifdef xu_atomic_flag_test_and_set_explicit_generic_impl
static __xu_inline__ xu_bool_t xu_atomic_flag_test_and_set_explicit_generic(xu_atomic_flag_t* a, xu_int_t mo)
{
    xu_assert(a);
    xu_assert_static(sizeof(xu_atomic_flag_t) == sizeof(xu_atomic32_t));
    return (xu_bool_t)xu_atomic32_fetch_and_set_explicit((xu_atomic32_t*)a, 1, mo);
}
#endif

#ifdef xu_atomic_flag_test_explicit_generic_impl
static __xu_inline__ xu_bool_t xu_atomic_flag_test_explicit_generic(xu_atomic_flag_t* a, xu_int_t mo)
{
    xu_assert(a);
    xu_assert_static(sizeof(xu_atomic_flag_t) == sizeof(xu_atomic32_t));
    return (xu_bool_t)xu_atomic32_get_explicit((xu_atomic32_t*)a, mo);
}
#endif

#ifdef xu_atomic_flag_clear_explicit_generic_impl
static __xu_inline__ xu_void_t xu_atomic_flag_clear_explicit_generic(xu_atomic_flag_t* a, xu_int_t mo)
{
    xu_assert(a);
    xu_assert_static(sizeof(xu_atomic_flag_t) == sizeof(xu_atomic32_t));
    xu_atomic32_set_explicit((xu_atomic32_t*)a, 0, mo);
}
#endif
