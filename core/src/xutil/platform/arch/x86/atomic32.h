#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */

#ifdef XU_ASSEMBLER_IS_GAS

#    ifndef xu_atomic32_fetch_and_set_explicit
#        define xu_atomic32_fetch_and_set_explicit(a, v, mo) xu_atomic32_fetch_and_set_explicit_x86(a, v, mo)
#    endif

#    ifndef xu_atomic32_compare_and_swap_explicit
#        define xu_atomic32_compare_and_swap_explicit(a, p, v, succ, fail)                                             \
            xu_atomic32_compare_and_swap_explicit_x86(a, p, v, succ, fail)
#    endif

#    ifndef xu_atomic32_fetch_and_add_explicit
#        define xu_atomic32_fetch_and_add_explicit(a, v, mo) xu_atomic32_fetch_and_add_explicit_x86(a, v, mo)
#    endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __xu_inline__ xu_int32_t xu_atomic32_fetch_and_set_explicit_x86(xu_atomic32_t* a, xu_int32_t v, xu_int_t mo)
{
    // check
    xu_assert(a);

    __xu_asm__ __xu_volatile__("lock xchgl %0, %1\n" //!< xchgl v, [a]
                               : "+r"(v)
                               : "m"(*a)
                               : "memory");

    return v;
}

static __xu_inline__ xu_bool_t xu_atomic32_compare_and_swap_explicit_x86(xu_atomic32_t* a, xu_int32_t* p, xu_int32_t v,
                                                                         xu_int_t succ, xu_int_t fail)
{
    // check
    xu_assert(a && p);

    /*
     * cmpxchgl v, [a]:
     *
     * if (eax == [a])
     * {
     *      zf = 1;
     *      [a] = v;
     * }
     * else
     * {
     *      zf = 0;
     *      eax = [a];
     * }
     *
     */
    xu_int32_t o;
    xu_int32_t e = *p;
    __xu_asm__ __xu_volatile__("lock cmpxchgl  %3, %1  \n" //!< cmpxchgq v, [a]
                               : "=a"(o)
                               : "m"(*a), "a"(e), "r"(v)
                               : "cc", "memory" //!< "cc" means that flags were changed.
    );
    *p = o;
    return o == e;
}

static __xu_inline__ xu_int32_t xu_atomic32_fetch_and_add_explicit_x86(xu_atomic32_t* a, xu_int32_t v, xu_int_t mo)
{
    // check
    xu_assert(a);

    /*
     * xaddl v, [a]:
     *
     * o = [a]
     * [a] += v;
     * v = o;
     *
     * cf, ef, of, sf, zf, pf... maybe changed
     */
    __xu_asm__ __xu_volatile__("lock xaddl %0, %1 \n" //!< xaddl v, [a]
                               : "+r"(v)
                               : "m"(*a)
                               : "cc", "memory");

    return v;
}

#endif // XU_ASSEMBLER_IS_GAS
