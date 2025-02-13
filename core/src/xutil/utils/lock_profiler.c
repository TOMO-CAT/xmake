/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "lock_profiler"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xutil/utils/lock_profiler.h"
#include "xutil/platform/native_memory.h"
#include "xutil/utils/singleton.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * macros
 */

// the lock profiler maxn
#ifdef __xu_small__
#    define XU_LOCK_PROFILER_MAXN (256)
#else
#    define XU_LOCK_PROFILER_MAXN (512)
#endif

/* *******************************************************
 * types
 */

// the lock profiler item type
typedef struct __xu_lock_profiler_item_t
{
    // the lock address
    xu_atomic_t lock;

    // the occupied count
    xu_atomic32_t size;

    // the lock name
    xu_atomic_t name;

} xu_lock_profiler_item_t;

// the lock profiler type
typedef struct __xu_lock_profiler_t
{
    // the list
    xu_lock_profiler_item_t list[XU_LOCK_PROFILER_MAXN];

} xu_lock_profiler_t;

/* *******************************************************
 * instance implementation
 */
static xu_handle_t xu_lock_profiler_instance_init(xu_cpointer_t* ppriv) { return (xu_handle_t)xu_lock_profiler_init(); }
static xu_void_t   xu_lock_profiler_instance_exit(xu_handle_t handle, xu_cpointer_t priv)
{
    xu_lock_profiler_dump((xu_lock_profiler_ref_t)handle);
    xu_lock_profiler_exit((xu_lock_profiler_ref_t)handle);
}

/* *******************************************************
 * implementation
 */
xu_lock_profiler_ref_t xu_lock_profiler()
{
    return (xu_lock_profiler_ref_t)xu_singleton_instance(XU_SINGLETON_TYPE_LOCK_PROFILER,
                                                         xu_lock_profiler_instance_init, xu_lock_profiler_instance_exit,
                                                         xu_null, xu_null);
}
xu_lock_profiler_ref_t xu_lock_profiler_init()
{
    return (xu_lock_profiler_ref_t)xu_native_memory_malloc0(sizeof(xu_lock_profiler_t));
}
xu_void_t xu_lock_profiler_exit(xu_lock_profiler_ref_t self)
{
    if (self) xu_native_memory_free((xu_pointer_t)self);
}
xu_void_t xu_lock_profiler_dump(xu_lock_profiler_ref_t self)
{
    // check
    xu_lock_profiler_t* profiler = (xu_lock_profiler_t*)self;
    xu_assert_and_check_return(profiler);

    // trace
    xu_trace_i("");

    // walk
    xu_size_t i = 0;
    xu_size_t n = xu_arrayn(profiler->list);
    for (i = 0; i < n; i++)
    {
        // the item
        xu_lock_profiler_item_t* item = &profiler->list[i];

        // the lock
        xu_pointer_t lock = xu_null;
        if ((lock = (xu_pointer_t)xu_atomic_get(&item->lock)))
        {
            // dump lock
            xu_trace_i("lock: %p, name: %s, occupied: %d", lock, (xu_char_t const*)xu_atomic_get(&item->name),
                       xu_atomic32_get(&item->size));
        }
    }
}
xu_void_t xu_lock_profiler_register(xu_lock_profiler_ref_t self, xu_pointer_t lock, xu_char_t const* name)
{
    // check
    xu_lock_profiler_t* profiler = (xu_lock_profiler_t*)self;
    xu_assert_and_check_return(profiler && lock);

    // trace
    xu_trace_d("register: lock: %p, name: %s: ..", lock, name);

    // the lock address
    xu_size_t addr = (xu_size_t)lock;

    // compile the hash value
    addr ^= (addr >> 8) ^ (addr >> 16);

    // walk
    xu_size_t i = 0;
    for (i = 0; i < 16; i++, addr++)
    {
        // the item
        xu_lock_profiler_item_t* item = &profiler->list[addr & (XU_LOCK_PROFILER_MAXN - 1)];

        // try to register the lock
        xu_long_t zero = 0;
        if (xu_atomic_compare_and_swap(&item->lock, &zero, (xu_long_t)lock))
        {
            // init name
            xu_atomic_set(&item->name, (xu_long_t)name);
            xu_atomic32_init(&item->size, 0);

            // trace
            xu_trace_d("register: lock: %p, name: %s, index: %lu: ok", lock, name, addr & (XU_LOCK_PROFILER_MAXN - 1));

            // ok
            break;
        }
    }

    // full?
    if (i == 16)
    {
        // trace
        xu_trace_w("register: lock: %p, name: %s: no", lock, name);
    }
}
xu_void_t xu_lock_profiler_occupied(xu_lock_profiler_ref_t self, xu_pointer_t lock)
{
    // check
    xu_lock_profiler_t* profiler = (xu_lock_profiler_t*)self;
    xu_check_return(profiler && lock);

    // the lock address
    xu_size_t addr = (xu_size_t)lock;

    // compile the hash value
    addr ^= (addr >> 8) ^ (addr >> 16);

    // walk
    xu_size_t i = 0;
    for (i = 0; i < 16; i++, addr++)
    {
        // the item
        xu_lock_profiler_item_t* item = &profiler->list[addr & (XU_LOCK_PROFILER_MAXN - 1)];

        // is this lock?
        if (lock == (xu_pointer_t)xu_atomic_get(&item->lock))
        {
            // occupied++
            xu_atomic32_fetch_and_add(&item->size, 1);

            // ok
            break;
        }
    }
}
