#include "xutil/memory/impl/memory.h"
#include "xutil/memory/default_allocator.h"
#include "xutil/memory/native_allocator.h"
#include "xutil/platform/native_memory.h"
#include "xutil/platform/page.h"

// the allocator
extern xu_allocator_ref_t g_allocator;

xu_bool_t xu_memory_init_env(xu_allocator_ref_t allocator)
{
    // done
    xu_bool_t ok = xu_false;
    do
    {
        // init page
        if (!xu_page_init())
        {
            break;
        }

        // init the native memory
        if (!xu_native_memory_init())
        {
            break;
        }

        // init the allocator
#if defined(XU_CONFIG_MICRO_ENABLE) || (defined(__xu_small__) && !defined(__xu_debug__))
        g_allocator = allocator ? allocator : xu_native_allocator();
#else
        g_allocator = allocator ? allocator : xu_default_allocator(xu_null, 0);
#endif
        xu_assert_and_check_break(g_allocator);

        // ok
        ok = xu_true;

    } while (0);

    // failed? exit it
    if (!ok) xu_memory_exit_env();

    // ok?
    return ok;
}
xu_void_t xu_memory_exit_env()
{
    // exit the native memory
    xu_native_memory_exit();

    // exit page
    xu_page_exit();
}
