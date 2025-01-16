#include "xutil/memory/large_allocator.h"
#include "xutil/memory/impl/native_large_allocator.h"
#include "xutil/memory/impl/static_large_allocator.h"
#include "xutil/platform/page.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_allocator_ref_t xu_large_allocator_init(xu_byte_t* data, xu_size_t size)
{
    // init pool
    return (data && size) ? xu_static_large_allocator_init(data, size, xu_page_size())
                          : xu_native_large_allocator_init();
}
