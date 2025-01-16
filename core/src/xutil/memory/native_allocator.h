#pragma once

#include "xutil/memory/allocator.h"
#include "xutil/prefix.h"

/*! the global native allocator
 *
 * @return                  the allocator
 */
xu_allocator_ref_t xu_native_allocator(xu_noarg_t);
