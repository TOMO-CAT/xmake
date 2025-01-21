#pragma once

#include "xutil/macros/assert.h"
#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */

#ifdef __ATOMIC_SEQ_CST

#    define xu_atomic64_init(a, v) xu_atomic64_set_explicit_gcc(a, v, __ATOMIC_RELAXED)
#    define xu_atomic64_get(a) xu_atomic64_get_explicit_gcc(a, __ATOMIC_SEQ_CST)
#    define xu_atomic64_get_explicit(a, mo) xu_atomic64_get_explicit_gcc(a, mo)
#    define xu_atomic64_set(a, v) xu_atomic64_set_explicit_gcc(a, v, __ATOMIC_SEQ_CST)
#    define xu_atomic64_set_explicit(a, v, mo) xu_atomic64_set_explicit_gcc(a, v, mo)
#    define xu_atomic64_compare_and_swap(a, p, v)                                                                      \
        xu_atomic64_compare_and_swap_explicit_gcc(a, p, v, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#    define xu_atomic64_compare_and_swap_explicit(a, p, v, succ, fail)                                                 \
        xu_atomic64_compare_and_swap_explicit_gcc(a, p, v, succ, fail)
#    define xu_atomic64_compare_and_swap_weak(a, p, v)                                                                 \
        xu_atomic64_compare_and_swap_weak_explicit_gcc(a, p, v, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#    define xu_atomic64_compare_and_swap_weak_explicit(a, p, v, succ, fail)                                            \
        xu_atomic64_compare_and_swap_weak_explicit_gcc(a, p, v, succ, fail)
#    define xu_atomic64_fetch_and_set(a, v) xu_atomic64_fetch_and_set_explicit_gcc(a, v, __ATOMIC_SEQ_CST)
#    define xu_atomic64_fetch_and_set_explicit(a, v, mo) xu_atomic64_fetch_and_set_explicit_gcc(a, v, mo)
#    define xu_atomic64_fetch_and_add(a, v) __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST)
#    define xu_atomic64_fetch_and_add_explicit(a, v, mo) __atomic_fetch_add(a, v, mo)
#    define xu_atomic64_fetch_and_sub(a, v) __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST)
#    define xu_atomic64_fetch_and_sub_explicit(a, v, mo) __atomic_fetch_sub(a, v, mo)
#    define xu_atomic64_fetch_and_or(a, v) __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST)
#    define xu_atomic64_fetch_and_or_explicit(a, v, mo) __atomic_fetch_or(a, v, mo)
#    define xu_atomic64_fetch_and_and(a, v) __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST)
#    define xu_atomic64_fetch_and_and_explicit(a, v, mo) __atomic_fetch_and(a, v, mo)
#    define xu_atomic64_fetch_and_xor(a, v) __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST)
#    define xu_atomic64_fetch_and_xor_explicit(a, v, mo) __atomic_fetch_xor(a, v, mo)

#else
#    define xu_atomic64_compare_and_swap(a, p, v) xu_atomic64_compare_and_swap_gcc(a, p, v)
#    define xu_atomic64_fetch_and_cmpset(a, p, v) __sync_val_compare_and_swap_8(a, p, v)

#    define xu_atomic64_fetch_and_add(a, v) __sync_fetch_and_add_8(a, v)
#    define xu_atomic64_fetch_and_sub(a, v) __sync_fetch_and_sub_8(a, v)
#    define xu_atomic64_fetch_and_or(a, v) __sync_fetch_and_or_8(a, v)
#    define xu_atomic64_fetch_and_and(a, v) __sync_fetch_and_and_8(a, v)

// ios armv6: no defined reference?
#    if !(defined(XU_CONFIG_OS_IOS) && XU_ARCH_ARM_VERSION < 7)
#        define xu_atomic64_fetch_and_xor(a, v) __sync_fetch_and_xor_8(a, v)
#    endif
#endif

/* *******************************************************
 * inline implementation
 * *******************************************************
 */

#ifdef __ATOMIC_SEQ_CST
static __xu_inline__ xu_int64_t xu_atomic64_get_explicit_gcc(xu_atomic64_t* a, xu_int_t mo)
{
    xu_assert(a);
    xu_int64_t t;
    __atomic_load(a, &t, mo);
    return t;
}
static __xu_inline__ xu_void_t xu_atomic64_set_explicit_gcc(xu_atomic64_t* a, xu_int64_t v, xu_int_t mo)
{
    xu_assert(a);
    __atomic_store(a, &v, mo);
}
static __xu_inline__ xu_bool_t xu_atomic64_compare_and_swap_explicit_gcc(xu_atomic64_t* a, xu_int64_t* p, xu_int64_t v,
                                                                         xu_int_t succ, xu_int_t fail)
{
    xu_assert(a);
    return __atomic_compare_exchange(a, p, &v, 0, succ, fail);
}
static __xu_inline__ xu_bool_t xu_atomic64_compare_and_swap_weak_explicit_gcc(xu_atomic64_t* a, xu_int64_t* p,
                                                                              xu_int64_t v, xu_int_t succ,
                                                                              xu_int_t fail)
{
    xu_assert(a);
    return __atomic_compare_exchange(a, p, &v, 1, succ, fail);
}
static __xu_inline__ xu_int64_t xu_atomic64_fetch_and_set_explicit_gcc(xu_atomic64_t* a, xu_int64_t v, xu_int_t mo)
{
    xu_assert(a);
    xu_int64_t o;
    __atomic_exchange(a, &v, &o, mo);
    return o;
}
#else
static __xu_inline__ xu_bool_t xu_atomic64_compare_and_swap_gcc(xu_atomic64_t* a, xu_int64_t* p, xu_int64_t v)
{
    xu_assert(a && p);
    xu_int64_t e = *p;
    *p           = __sync_val_compare_and_swap_8(a, e, v);
    return *p == e;
}
#endif
