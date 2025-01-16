#pragma once

#include "xutil/config.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/keyword.h"
#include <stdatomic.h>

/* *******************************************************
 *  macros
 * *******************************************************
 */
#define XU_ATOMIC_RELAXED memory_order_relaxed
#define XU_ATOMIC_CONSUME memory_order_consume
#define XU_ATOMIC_ACQUIRE memory_order_acquire
#define XU_ATOMIC_RELEASE memory_order_release
#define XU_ATOMIC_ACQ_REL memory_order_acq_rel
#define XU_ATOMIC_SEQ_CST memory_order_seq_cst

#define xu_memory_barrier() atomic_thread_fence(memory_order_seq_cst)

#define XU_ATOMIC_FLAG_INIT ATOMIC_FLAG_INIT
#define xu_atomic_flag_test_and_set_explicit(a, mo) atomic_flag_test_and_set_explicit(a, mo)
#define xu_atomic_flag_test_and_set(a) atomic_flag_test_and_set(a)
#ifdef atomic_flag_test_explicit
#    define xu_atomic_flag_test_explicit(a, mo) atomic_flag_test_explicit(a, mo)
#else
#    define xu_atomic_flag_test_explicit(a, mo) xu_atomic_flag_test_explicit_libc(a, mo)
#endif
#define xu_atomic_flag_test(a) xu_atomic_flag_test_explicit(a, memory_order_seq_cst)
#define xu_atomic_flag_test_noatomic(a) xu_atomic_flag_test_noatomic_libc(a)
#define xu_atomic_flag_clear_explicit(a, mo) atomic_flag_clear_explicit(a, mo)
#define xu_atomic_flag_clear(a) atomic_flag_clear(a)

/* *******************************************************
 *  inline implementation
 * *******************************************************
 */
// static __xu_inline__ xu_bool_t xu_atomic_flag_test_explicit_libc(xu_atomic_flag_t* a, xu_int_t mo)
// {
//     xu_assert(a);
//     xu_assert_static(sizeof(xu_atomic_flag_t) == sizeof(unsigned char));
//     return (xu_bool_t)atomic_load_explicit((__xu_volatile__ _Atomic unsigned char*)a, mo);
// }
// static __xu_inline__ xu_bool_t xu_atomic_flag_test_noatomic_libc(xu_atomic_flag_t* a)
// {
//     xu_assert(a);
//     xu_assert_static(sizeof(xu_atomic_flag_t) == sizeof(unsigned char));
//     return (xu_bool_t) * ((__xu_volatile__ unsigned char*)a);
// }
