#pragma once

#include "xutil/macros/assert.h"
#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */
#ifdef __ATOMIC_SEQ_CST

#    define XU_ATOMIC_RELAXED __ATOMIC_RELAXED
#    define XU_ATOMIC_CONSUME __ATOMIC_CONSUME
#    define XU_ATOMIC_ACQUIRE __ATOMIC_ACQUIRE
#    define XU_ATOMIC_RELEASE __ATOMIC_RELEASE
#    define XU_ATOMIC_ACQ_REL __ATOMIC_ACQ_REL
#    define XU_ATOMIC_SEQ_CST __ATOMIC_SEQ_CST

#    define xu_memory_barrier() __atomic_thread_fence(__ATOMIC_SEQ_CST)

#    define xu_atomic_flag_test_and_set_explicit(a, mo) __atomic_test_and_set(a, mo)
#    define xu_atomic_flag_test_and_set(a) __atomic_test_and_set(a, __ATOMIC_SEQ_CST)
#    define xu_atomic_flag_test_explicit(a, mo) xu_atomic_flag_test_explicit_gcc(a, mo)
#    define xu_atomic_flag_test(a) xu_atomic_flag_test_explicit(a, __ATOMIC_SEQ_CST)
#    define xu_atomic_flag_clear_explicit(a, mo) __atomic_clear(a, mo)
#    define xu_atomic_flag_clear(a) __atomic_clear(a, __ATOMIC_SEQ_CST)
#else
#    define xu_memory_barrier() __sync_synchronize()
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
#ifdef __ATOMIC_SEQ_CST
static __xu_inline__ xu_bool_t xu_atomic_flag_test_explicit_gcc(xu_atomic_flag_t* a, xu_int_t mo)
{
    xu_assert(a);
    xu_assert_static(sizeof(xu_atomic_flag_t) == sizeof(unsigned char));
    unsigned char t;
    __atomic_load((__xu_volatile__ unsigned char*)a, &t, mo);
    return (xu_bool_t)t;
}
#endif
