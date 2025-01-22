#pragma once

#include "xutil/memory/allocator.h"
#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 */
/*! init the large allocator
 *
 * <pre>
 *
 *  -------------------------      ------------------------
 * |       native memory     |    |         data           |
 *  -------------------------      ------------------------
 *              |                             |
 *  -------------------------      ------------------------
 * |  native large allocator |    | static large allocator |
 *  -------------------------      ------------------------
 *              |                             |
 *  ------------------------------------------------------
 * |                     large allocator                  |
 *  ------------------------------------------------------
 *
 * </pre>
 *
 * @param data          the data, uses the native memory if be null
 * @param size          the size
 *
 * @return              the allocator
 */
xu_allocator_ref_t xu_large_allocator_init(xu_byte_t* data, xu_size_t size);
