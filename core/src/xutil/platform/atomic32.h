#pragma once

#include "xutil/macros/type.h"
#include "xutil/platform/atomic.h"
#include "xutil/prefix.h"

#if __xu_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__)
#    include "libc/atomic32.h"
#elif defined(XU_COMPILER_IS_GCC) && (defined(__ATOMIC_SEQ_CST) || defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4))
#    include "xutil/compiler/gcc/atomic32.h"
#endif
#include "xutil/platform/arch/atomic32.h"

#ifndef xu_atomic32_init
#    define xu_atomic32_init(a, v)                                                                                     \
        do                                                                                                             \
        {                                                                                                              \
            *(a) = (v);                                                                                                \
        } while (0)
#endif

#ifndef xu_atomic32_compare_and_swap_explicit
#    define xu_atomic32_compare_and_swap_explicit(a, p, v, succ, fail)                                                 \
        xu_atomic32_compare_and_swap_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef xu_atomic32_compare_and_swap
#    define xu_atomic32_compare_and_swap(a, p, v)                                                                      \
        xu_atomic32_compare_and_swap_explicit(a, p, v, XU_ATOMIC_SEQ_CST, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_compare_and_swap_weak_explicit
#    define xu_atomic32_compare_and_swap_weak_explicit(a, p, v, succ, fail)                                            \
        xu_atomic32_compare_and_swap_explicit(a, p, v, succ, fail)
#endif
#ifndef xu_atomic32_compare_and_swap_weak
#    define xu_atomic32_compare_and_swap_weak(a, p, v)                                                                 \
        xu_atomic32_compare_and_swap_weak_explicit(a, p, v, XU_ATOMIC_SEQ_CST, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_fetch_and_cmpset_explicit
#    define xu_atomic32_fetch_and_cmpset_explicit(a, p, v, succ, fail)                                                 \
        xu_atomic32_fetch_and_cmpset_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef xu_atomic32_fetch_and_cmpset
#    define xu_atomic32_fetch_and_cmpset(a, p, v)                                                                      \
        xu_atomic32_fetch_and_cmpset_explicit(a, p, v, XU_ATOMIC_SEQ_CST, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_fetch_and_set_explicit
#    define xu_atomic32_fetch_and_set_explicit(a, v, mo) xu_atomic32_fetch_and_set_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic32_fetch_and_set
#    define xu_atomic32_fetch_and_set(a, v) xu_atomic32_fetch_and_set_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_fetch_and_add_explicit
#    define xu_atomic32_fetch_and_add_explicit(a, v, mo) xu_atomic32_fetch_and_add_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic32_fetch_and_add
#    define xu_atomic32_fetch_and_add(a, v) xu_atomic32_fetch_and_add_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_fetch_and_sub_explicit
#    define xu_atomic32_fetch_and_sub_explicit(a, v, mo) xu_atomic32_fetch_and_add_explicit(a, -(v), mo)
#endif
#ifndef xu_atomic32_fetch_and_sub
#    define xu_atomic32_fetch_and_sub(a, v) xu_atomic32_fetch_and_sub_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_fetch_and_or_explicit
#    define xu_atomic32_fetch_and_or_explicit(a, v, mo) xu_atomic32_fetch_and_or_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic32_fetch_and_or
#    define xu_atomic32_fetch_and_or(a, v) xu_atomic32_fetch_and_or_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_fetch_and_xor_explicit
#    define xu_atomic32_fetch_and_xor_explicit(a, v, mo) xu_atomic32_fetch_and_xor_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic32_fetch_and_xor
#    define xu_atomic32_fetch_and_xor(a, v) xu_atomic32_fetch_and_xor_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_fetch_and_and_explicit
#    define xu_atomic32_fetch_and_and_explicit(a, v, mo) xu_atomic32_fetch_and_and_explicit_generic(a, v, mo)
#endif
#ifndef xu_atomic32_fetch_and_and
#    define xu_atomic32_fetch_and_and(a, v) xu_atomic32_fetch_and_and_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_get_explicit
#    define xu_atomic32_get_explicit(a, mo) xu_atomic32_fetch_and_cmpset_explicit(a, 0, 0, mo, mo)
#endif
#ifndef xu_atomic32_get
#    define xu_atomic32_get(a) xu_atomic32_get_explicit(a, XU_ATOMIC_SEQ_CST)
#endif

#ifndef xu_atomic32_set_explicit
#    define xu_atomic32_set_explicit(a, v, mo) xu_atomic32_fetch_and_set_explicit(a, v, mo)
#endif
#ifndef xu_atomic32_set
#    define xu_atomic32_set(a, v) xu_atomic32_set_explicit(a, v, XU_ATOMIC_SEQ_CST)
#endif

/* *******************************************************
 * inlines
 */
static __xu_inline__ xu_bool_t xu_atomic32_compare_and_swap_explicit_generic(xu_atomic32_t* a, xu_int32_t* p,
                                                                             xu_int32_t v, xu_int_t succ, xu_int_t fail)
{
    // FIXME
    // no safe

    xu_atomic32_t o = *a;
    if (o == *p)
    {
        *a = v;
        return xu_true;
    }
    else
    {
        *p = o;
        return xu_false;
    }
}
static __xu_inline__ xu_int32_t xu_atomic32_fetch_and_cmpset_explicit_generic(xu_atomic32_t* a, xu_int32_t p,
                                                                              xu_int32_t v, xu_int_t succ,
                                                                              xu_int_t fail)
{
    xu_atomic32_compare_and_swap_explicit(a, &p, v, succ, fail);
    return p;
}
static __xu_inline__ xu_int32_t xu_atomic32_fetch_and_set_explicit_generic(xu_atomic32_t* a, xu_int32_t v, xu_int_t mo)
{
    xu_int32_t o;
    do
    {
        o = *a;
    } while (!xu_atomic32_compare_and_swap_weak_explicit(a, &o, v, mo, mo));
    return o;
}
static __xu_inline__ xu_int32_t xu_atomic32_fetch_and_add_explicit_generic(xu_atomic32_t* a, xu_int32_t v, xu_int_t mo)
{
    xu_int32_t o;
    do
    {
        o = *a;
    } while (!xu_atomic32_compare_and_swap_weak_explicit(a, &o, o + v, mo, mo));
    return o;
}
static __xu_inline__ xu_int32_t xu_atomic32_fetch_and_xor_explicit_generic(xu_atomic32_t* a, xu_int32_t v, xu_int_t mo)
{
    xu_int32_t o;
    do
    {
        o = *a;
    } while (!xu_atomic32_compare_and_swap_weak_explicit(a, &o, o ^ v, mo, mo));
    return o;
}
static __xu_inline__ xu_int32_t xu_atomic32_fetch_and_and_explicit_generic(xu_atomic32_t* a, xu_int32_t v, xu_int_t mo)
{
    xu_int32_t o;
    do
    {
        o = *a;
    } while (!xu_atomic32_compare_and_swap_weak_explicit(a, &o, o & v, mo, mo));
    return o;
}
static __xu_inline__ xu_int32_t xu_atomic32_fetch_and_or_explicit_generic(xu_atomic32_t* a, xu_int32_t v, xu_int_t mo)
{
    xu_int32_t o;
    do
    {
        o = *a;
    } while (!xu_atomic32_compare_and_swap_weak_explicit(a, &o, o | v, mo, mo));
    return o;
}
