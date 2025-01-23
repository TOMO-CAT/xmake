#include "xutil/platform/atomic64.h"
#include "xutil/platform/spinlock.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 *  macros
 */

// the atomic64 lock mac count
#define XU_ATOMIC64_LOCK_MAXN (16)

/* *******************************************************
 *  types
 */

// the atomic64 lock type
typedef __xu_cacheline_aligned__ struct __xu_atomic64_lock_t
{
    // the lock
    xu_spinlock_t lock;

    // the padding
    xu_byte_t padding[XU_L1_CACHE_BYTES];

} __xu_cacheline_aligned__ xu_atomic64_lock_t;

/* *******************************************************
 *  globals
 */
// the locks
static xu_atomic64_lock_t g_locks[XU_ATOMIC64_LOCK_MAXN] = {{XU_SPINLOCK_INIT, {0}}};

/* *******************************************************
 *  implementation
 */
static __xu_inline_force__ xu_spinlock_ref_t xu_atomic64_lock(xu_atomic64_t* a)
{
    // trace
    xu_trace1_w("using generic atomic64, maybe slower!");

    // the addr
    xu_size_t addr = (xu_size_t)a;

    // compile the hash value
    addr >>= XU_L1_CACHE_SHIFT;
    addr ^= (addr >> 8) ^ (addr >> 16);

    // the lock
    return &g_locks[addr & (XU_ATOMIC64_LOCK_MAXN - 1)].lock;
}

xu_bool_t xu_atomic64_compare_and_swap_explicit_generic(xu_atomic64_t* a, xu_int64_t* p, xu_int64_t v, xu_int_t succ,
                                                        xu_int_t fail)
{
    // check
    xu_assert(a && p);

    // the lock
    xu_spinlock_ref_t lock = xu_atomic64_lock(a);

    // enter
    xu_spinlock_enter(lock);

    // set value
    xu_bool_t     ok = xu_false;
    xu_atomic64_t o  = *a;
    if (o == *p)
    {
        *a = v;
        ok = xu_true;
    }
    else
        *p = o;

    // leave
    xu_spinlock_leave(lock);

    // ok?
    return ok;
}
