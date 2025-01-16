#pragma once

#include "xutil/memory/allocator.h"
#include "xutil/prefix.h"

/* init the large allocator and the allocated data will be aligned by the page size
 *
 * @param data          the allocator data
 * @param size          the allocator size
 * @param pagesize      the pagesize
 *
 * @return              the allocator
 */
xu_allocator_ref_t xu_static_large_allocator_init(xu_byte_t* data, xu_size_t size, xu_size_t pagesize);
