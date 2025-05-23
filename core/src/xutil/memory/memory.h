#pragma once

/* *******************************************************
 * description
 */

#include "xutil/memory/impl/memory.h"
#include "xutil/memory/static_buffer.h"
#include "xutil/prefix.h"

// clang-format off
/*!architecture
 *
 * <pre>
 *
 *  ----------------      -------------------------------------------------------      ----------------------
 * | native memory  | or |                         data                          | <- |    static allocator  |
 *  ----------------      -------------------------------------------------------      ----------------------
 *         |             if data be null             |
 *         `---------------------------------------> |
 *                                                   |
 *  -----------------------------------------------------------------------------      ----------------------      ------      ------
 * |                                large allocator                              | -> |    fixed pool:NB     | -> | slot | -> | slot | -> ...
 *  -----------------------------------------------------------------------------      ----------------------      ------      ------
 *                             |                     |
 *                             |          ---------------------------------------      ----------------------      ------      ------
 *                             |         |            small allocator            | -> |    fixed pool:16B    | -> | slot | -> | slot | -> ...
 *                             |          ---------------------------------------     |----------------------|     ------      ------
 *                             |                              |                       |    fixed pool:32B    | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:64B    | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:96B*   | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:128B   | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:192B*  | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:256B   | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:384B*  | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:512B   | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:1024B  | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:2048B  | -> ...
 *                             |                              |                       |----------------------|
 *                             |                              |                       |    fixed pool:3072B* | -> ...
 *                             |                              |                        ----------------------
 *                             |                              |
 *                             |                              |
 *  ------------------------------------------------------------------------------
 * |                         >3KB        |                 <=3KB                  |
 * |------------------------------------------------------------------------------|
 * |                              default allocator                               |
 *  ------------------------------------------------------------------------------
 *                                                                     |
 *                                                          ----------------------
 *                                                         |     string pool      |
 *                                                          ----------------------
 *
 * </pre>
 */
// clang-format on
