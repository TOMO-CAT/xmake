#pragma once

#include "xutil/memory/allocator.h"
#include "xutil/memory/fixed_pool.h"
#include "xutil/memory/impl/prefix.h"
#include "xutil/prefix.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the data size maximum
#define XU_SMALL_ALLOCATOR_DATA_MAXN (3072)

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! init the small allocator only for size <=3KB
 *
 * <pre>
 *
 *  --------------------------------------
 * |    fixed pool: 16B    |  1-16B       |
 * |--------------------------------------|
 * |    fixed pool: 32B    |  17-32B      |
 * |--------------------------------------|
 * |    fixed pool: 64B    |  33-64B      |
 * |--------------------------------------|
 * |    fixed pool: 96B*   |  65-96B*     |
 * |--------------------------------------|
 * |    fixed pool: 128B   |  97-128B     |
 * |--------------------------------------|
 * |    fixed pool: 192B*  |  129-192B*   |
 * |--------------------------------------|
 * |    fixed pool: 256B   |  193-256B    |
 * |--------------------------------------|
 * |    fixed pool: 384B*  |  257-384B*   |
 * |--------------------------------------|
 * |    fixed pool: 512B   |  385-512B    |
 * |--------------------------------------|
 * |    fixed pool: 1024B  |  513-1024B   |
 * |--------------------------------------|
 * |    fixed pool: 2048B  |  1025-2048B  |
 * |--------------------------------------|
 * |    fixed pool: 3072B* |  2049-3072B* |
 *  --------------------------------------
 *
 * </pre>
 *
 * @param large_allocator   the large allocator, uses the global allocator if be null
 *
 * @return                  the pool
 */
xu_allocator_ref_t xu_small_allocator_init(xu_allocator_ref_t large_allocator);
