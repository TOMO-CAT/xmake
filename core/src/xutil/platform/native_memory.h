#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/*! init native memory
 *
 * @note
 * need support to be called repeatedly
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_native_memory_init(xu_noarg_t);

/// exit native memory
xu_void_t xu_native_memory_exit(xu_noarg_t);

/*! malloc the native memory
 *
 * @param size          the size
 *
 * @return              the data address
 */
xu_pointer_t xu_native_memory_malloc(xu_size_t size);

/*! malloc the native memory and fill zero
 *
 * @param size          the size
 *
 * @return              the data address
 */
xu_pointer_t xu_native_memory_malloc0(xu_size_t size);

/*! malloc the native memory with the item count
 *
 * @param item          the item count
 * @param size          the item size
 *
 * @return              the data address
 */
xu_pointer_t xu_native_memory_nalloc(xu_size_t item, xu_size_t size);

/*! malloc the native memory with the item count and fill zero
 *
 * @param item          the item count
 * @param size          the item size
 *
 * @return              the data address
 */
xu_pointer_t xu_native_memory_nalloc0(xu_size_t item, xu_size_t size);

/*! realloc the native memory
 *
 * @param data          the data address
 * @param size          the size
 *
 * @return              the new data address
 */
xu_pointer_t xu_native_memory_ralloc(xu_pointer_t data, xu_size_t size);

/*! free the native memory
 *
 * @param data          the data address
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_native_memory_free(xu_pointer_t data);
