/* *******************************************************
 * trace
 */

#define XU_TRACE_MODULE_NAME "random_linear"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/math/random/linear.h"
#include "xutil/platform/spinlock.h"

/* *******************************************************
 * globals
 */

// the value
static xu_size_t g_value = 2166136261ul;

// the lock
static xu_spinlock_t g_lock = XU_SPINLOCK_INIT;

/* *******************************************************
 * implementation
 */

xu_void_t xu_random_linear_seed(xu_size_t seed)
{
    // enter
    xu_spinlock_enter(&g_lock);

    // update value
    g_value = seed;

    // leave
    xu_spinlock_leave(&g_lock);
}
xu_long_t xu_random_linear_value()
{
    // enter
    xu_spinlock_enter(&g_lock);

    // generate the next value
    g_value = (g_value * 10807 + 1) & 0xffffffff;

    // leave
    xu_spinlock_leave(&g_lock);

    // ok
    return (xu_long_t)g_value;
}
