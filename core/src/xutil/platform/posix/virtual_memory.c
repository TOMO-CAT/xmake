#include "xutil/platform/virtual_memory.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/memory/impl/prefix.h"
#include "xutil/prefix.h"
#include <sys/mman.h>

#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#    define MAP_ANONYMOUS MAP_ANON
#endif

xu_pointer_t xu_virtual_memory_malloc(xu_size_t size)
{
    // check
    xu_check_return_val(size, xu_null);
    xu_assert_and_check_return_val(size >= XU_VIRTUAL_MEMORY_DATA_MINN, xu_null);

    /* allocate an anonymous mmap buffer
     *
     * @note we use xu_pool_data_head_t to support xu_pool_data_size() when checking memory in debug mode
     */
    xu_pool_data_head_t* block =
        mmap(xu_null, sizeof(xu_pool_data_head_t) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block)
    {
        block->size = size;
        return (xu_pointer_t)&block[1];
    }
    return xu_null;
}
xu_pointer_t xu_virtual_memory_ralloc(xu_pointer_t data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(size >= XU_VIRTUAL_MEMORY_DATA_MINN, xu_null);

    // no data? malloc it
    if (!data) return xu_virtual_memory_malloc(size);
    // realloc it
    else
    {
        // shrink size? return it directly
        xu_pool_data_head_t* block = &((xu_pool_data_head_t*)data)[-1];
        if (size <= block->size) return data;

        // allocate a new anonymous map buffer
        xu_pointer_t data_new = xu_virtual_memory_malloc(size);
        if (data_new) xu_memcpy(data_new, data, block->size);
        xu_virtual_memory_free(data);
        return data_new;
    }
}
xu_bool_t xu_virtual_memory_free(xu_pointer_t data)
{
    xu_pool_data_head_t* block = (xu_pool_data_head_t*)data;
    if (block)
    {
        block--;
        return munmap((xu_pointer_t)block, sizeof(xu_pool_data_head_t) + block->size) == 0;
    }
    return xu_true;
}
