#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

// the pool data magic number
#define XU_POOL_DATA_MAGIC (0xdead)

// the pool data empty magic number
#define XU_POOL_DATA_EMPTY_MAGIC (0xdeaf)

// the pool data patch value
#define XU_POOL_DATA_PATCH (0xcc)

// the pool data size maximum
#define XU_POOL_DATA_SIZE_MAXN (XU_MAXU32)

// the pool data address alignment
#define XU_POOL_DATA_ALIGN XU_CPU_BITBYTE

// the pool data alignment keyword
#define __xu_pool_data_aligned__ __xu_cpu_aligned__

// the pool data head different size for computing the wasted space size
#ifdef __xu_debug__
#    define XU_POOL_DATA_HEAD_DIFF_SIZE (sizeof(xu_pool_data_head_t) - sizeof(xu_uint32_t))
#endif

/* *******************************************************
 * types
 */

#ifdef __xu_debug__
// the pool data debug head type
typedef __xu_pool_data_aligned__ struct __xu_pool_data_debug_head_t
{
    // the file
    xu_char_t const* file;

    // the func
    xu_char_t const* func;

    // the backtrace frames
    xu_pointer_t backtrace[16];

    // the line
    xu_uint16_t line;

    /* the magic
     *
     * @note the address may be not accessed if we place the magic to head.
     */
    xu_uint16_t magic;

} __xu_pool_data_aligned__ xu_pool_data_debug_head_t;
#endif

// the pool data head type
typedef struct __xu_pool_data_head_t
{
#ifdef __xu_debug__
    // the debug head
    xu_pool_data_debug_head_t debug;
#endif

    // the size
    xu_size_t size;

} xu_pool_data_head_t;

// the pool data empty head type
typedef struct __xu_pool_data_empty_head_t
{
#ifdef __xu_debug__
    // the debug head
    xu_pool_data_debug_head_t debug;
#endif

} xu_pool_data_empty_head_t;

/* *******************************************************
 * interfaces
 */
#ifdef __xu_debug__

/* the data size
 *
 * @param data                  the data address
 *
 * @return                      the data size
 */
xu_size_t xu_pool_data_size(xu_cpointer_t data);

/* dump data info
 *
 * @param data                  the data address
 * @param verbose               dump verbose info?
 * @param prefix                the prefix info
 */
xu_void_t xu_pool_data_dump(xu_cpointer_t data, xu_bool_t verbose, xu_char_t const* prefix);

/* save backtrace
 *
 * @param data_head             the data head
 * @param skip_frames           the skiped frame count
 */
xu_void_t xu_pool_data_save_backtrace(xu_pool_data_debug_head_t* debug_head, xu_size_t skip_frames);

#endif
