#pragma once

#include "xutil/memory/allocator.h"
#include "xutil/prefix.h"

/* init the native large allocator and the allocated data will be aligned by the page size
 *
 * @return              the allocator
 */
xu_allocator_ref_t xu_native_large_allocator_init(xu_noarg_t);
