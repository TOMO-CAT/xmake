#pragma once

#include "xutil/prefix.h"

// the virtual memory data size minimum
#define XU_VIRTUAL_MEMORY_DATA_MINN (128 * 1024)

/*! malloc the virtual memory
 *
 * @param size          the size
 *
 * @return              the data address
 */
xu_pointer_t xu_virtual_memory_malloc(xu_size_t size);

/*! realloc the virtual memory
 *
 * @param data          the data address
 * @param size          the size
 *
 * @return              the new data address
 */
xu_pointer_t xu_virtual_memory_ralloc(xu_pointer_t data, xu_size_t size);

/*! free the virtual memory
 *
 * @param data          the data address
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_virtual_memory_free(xu_pointer_t data);
