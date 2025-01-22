#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

#if __xu_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__)
#    include "libc/atomic64.h"
#elif defined(XU_COMPILER_IS_GCC) && (defined(__ATOMIC_SEQ_CST) || defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8))
#    include "xutil/compiler/gcc/atomic64.h"
#endif

/* *******************************************************
 *  macros
 */
#ifndef xu_atomic64_init
#    define xu_atomic64_init(a, v)                                                                                     \
        do                                                                                                             \
        {                                                                                                              \
            *(a) = (v);                                                                                                \
        } while (0)
#endif

#ifndef xu_atomic64_compare_and_swap_explicit
#    define xu_atomic64_compare_and_swap_explicit(a, p, v, succ, fail)                                                 \
        xu_atomic64_compare_and_swap_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef xu_atomic64_compare_and_swap
#    define xu_atomic64_compare_and_swap(a, p, v)                                                                      \
        xu_atomic64_compare_and_swap_explicit(a, p, v, XU_ATOMIC_SEQ_CST, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_compare_and_swap_weak_explicit
#    define xu_atomic64_compare_and_swap_weak_explicit(a, p, v, succ, fail)                                            \
        xu_atomic64_compare_and_swap_explicit(a, p, v, succ, fail)
#endif
#ifndef xu_atomic64_compare_and_swap_weak
#    define xu_atomic64_compare_and_swap_weak(a, p, v)                                                                 \
        xu_atomic64_compare_and_swap_weak_explicit(a, p, v, XU_ATOMIC_SEQ_CST, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_fetch_and_cmpset_explicit
#    define xu_atomic64_fetch_and_cmpset_explicit(a, p, v, succ, fail)                                                 \
        xu_atomic64_fetch_and_cmpset_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef xu_atomic64_fetch_and_cmpset
#    define xu_atomic64_fetch_and_cmpset(a, p, v)                                                                      \
        xu_atomic64_fetch_and_cmpset_explicit(a, p, v, XU_ATOMIC_SEQ_CST, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_fetch_and_set_explicit
#    define xu_atomic64_fetch_and_set_explicit(a, v, mo) xu_atomic64_fetch_and_set_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic64_fetch_and_set
#    define xu_atomic64_fetch_and_set(a, v) xu_atomic64_fetch_and_set_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_fetch_and_add_explicit
#    define xu_atomic64_fetch_and_add_explicit(a, v, mo) xu_atomic64_fetch_and_add_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic64_fetch_and_add
#    define xu_atomic64_fetch_and_add(a, v) xu_atomic64_fetch_and_add_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_fetch_and_sub_explicit
#    define xu_atomic64_fetch_and_sub_explicit(a, v, mo) xu_atomic64_fetch_and_add_explicit(a, -(v), mo)
#endif
#ifndef xu_atomic64_fetch_and_sub
#    define xu_atomic64_fetch_and_sub(a, v) xu_atomic64_fetch_and_sub_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_fetch_and_or_explicit
#    define xu_atomic64_fetch_and_or_explicit(a, v, mo) xu_atomic64_fetch_and_or_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic64_fetch_and_or
#    define xu_atomic64_fetch_and_or(a, v) xu_atomic64_fetch_and_or_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_fetch_and_xor_explicit
#    define xu_atomic64_fetch_and_xor_explicit(a, v, mo) xu_atomic64_fetch_and_xor_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic64_fetch_and_xor
#    define xu_atomic64_fetch_and_xor(a, v) xu_atomic64_fetch_and_xor_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_fetch_and_and_explicit
#    define xu_atomic64_fetch_and_and_explicit(a, v, mo) xu_atomic64_fetch_and_and_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic64_fetch_and_and
#    define xu_atomic64_fetch_and_and(a, v) xu_atomic64_fetch_and_and_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_get_explicit
#    define xu_atomic64_get_explicit(a, mo) xu_atomic64_fetch_and_cmpset_explicit(a, 0, 0, mo, mo)
#endif
#ifndef xu_atomic64_get
#    define xu_atomic64_get(a) xu_atomic64_get_explicit(a, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic64_set_explicit
#    define xu_atomic64_set_explicit(a, v, mo) xu_atomic64_fetch_and_set_explicit(a, v, mo)
#endif
#ifndef xu_atomic64_set
#    define xu_atomic64_set(a, v) xu_atomic64_set_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifdef XU_CONFIG_API_HAVE_DEPRECATED
#    include "deprecated/atomic64.h"
#endif

/* *******************************************************
 *  interfaces
 */
/* compare and set the 64bits value
 *
 * @param a     pointer to the atomic object to test and modify
 * @param p     pointer to the value expected to be found in the atomic object
 * @param v     the value to store in the atomic object if it is as expected
 * @param succ  the memory order if be successful
 * @param fail  the memory order if be failed
 *
 * @return      the result of the comparison: true if *a was equal to *p, false otherwise.
 */
xu_bool_t xu_atomic64_compare_and_swap_explicit_generic(xu_atomic64_t* a, xu_int64_t* p, xu_int64_t v, xu_int_t succ,
                                                        xu_int_t fail);

/* *******************************************************
 *  inline interfaces
 */

static __xu_inline__ xu_int64_t xu_atomic64_fetch_and_cmpset_explicit_generic(xu_atomic64_t* a, xu_int64_t p,
                                                                              xu_int64_t v, xu_int_t succ,
                                                                              xu_int_t fail)
{
    xu_atomic64_compare_and_swap_explicit(a, &p, v, succ, fail);
    return p;
}
static __xu_inline__ xu_int64_t xu_atomic64_fetch_and_set_explicit_generic(xu_atomic64_t* a, xu_int64_t v, xu_int_t mo)
{
    xu_int64_t o;
    do
    {
        o = *a;
    } while (!xu_atomic64_compare_and_swap_weak_explicit(a, &o, v, mo, mo));
    return o;
}
static __xu_inline__ xu_int64_t xu_atomic64_fetch_and_add_explicit_generic(xu_atomic64_t* a, xu_int64_t v, xu_int_t mo)
{
    xu_int64_t o;
    do
    {
        o = *a;
    } while (!xu_atomic64_compare_and_swap_weak_explicit(a, &o, o + v, mo, mo));
    return o;
}
static __xu_inline__ xu_int64_t xu_atomic64_fetch_and_xor_explicit_generic(xu_atomic64_t* a, xu_int64_t v, xu_int_t mo)
{
    xu_int64_t o;
    do
    {
        o = *a;
    } while (!xu_atomic64_compare_and_swap_weak_explicit(a, &o, o ^ v, mo, mo));
    return o;
}
static __xu_inline__ xu_int64_t xu_atomic64_fetch_and_and_explicit_generic(xu_atomic64_t* a, xu_int64_t v, xu_int_t mo)
{
    xu_int64_t o;
    do
    {
        o = *a;
    } while (!xu_atomic64_compare_and_swap_weak_explicit(a, &o, o & v, mo, mo));
    return o;
}
static __xu_inline__ xu_int64_t xu_atomic64_fetch_and_or_explicit_generic(xu_atomic64_t* a, xu_int64_t v, xu_int_t mo)
{
    xu_int64_t o;
    do
    {
        o = *a;
    } while (!xu_atomic64_compare_and_swap_weak_explicit(a, &o, o | v, mo, mo));
    return o;
}
