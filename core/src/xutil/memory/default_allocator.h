#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  includes
 */

#include "xutil/memory/allocator.h"

/* *******************************************************
 *  interfaces
 */

/*! the global default allocator
 *
 * @param data              the buffer data, uses the native buffer if be null
 * @param size              the buffer size
 *
 * @return                  the allocator
 */
xu_allocator_ref_t xu_default_allocator(xu_byte_t* data, xu_size_t size);

/*! init the default allocator
 *
 * <pre>
 *
 *  ----------------      -------------------------------------------------------
 * | native memory  | or |                         data                          |
 *  ----------------      -------------------------------------------------------
 *         |             if data be null             |
 *         `---------------------------------------> |
 *                                                   |
 *  -----------------------------------------------------------------------------
 * |                                  large allocator                            |
 *  -----------------------------------------------------------------------------
 *                             |                     |
 *                             |          ---------------------------------------
 *                             |         |            small allocator            |
 *                             |          ---------------------------------------
 *                             |                              |
 *  -----------------------------------------------------------------------------
 * |                         >3KB        |                 <=3KB                 |
 * |-----------------------------------------------------------------------------|
 * |                              default allocator                              |
 *  -----------------------------------------------------------------------------
 *
 * </pre>
 *
 * @param large_allocator   the large allocator, cannot be null
 *
 * @return                  the allocator
 */
xu_allocator_ref_t xu_default_allocator_init(xu_allocator_ref_t large_allocator);
