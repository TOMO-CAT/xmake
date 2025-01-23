#pragma once

#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/type.h"
#include "xutil/platform/spinlock.h"
#include "xutil/prefix.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 *  macros
 */

#define xu_allocator_malloc(allocator, size) xu_allocator_malloc_(allocator, size __xu_debug_vals__)
#define xu_allocator_malloc0(allocator, size) xu_allocator_malloc0_(allocator, size __xu_debug_vals__)

#define xu_allocator_nalloc(allocator, item, size) xu_allocator_nalloc_(allocator, item, size __xu_debug_vals__)
#define xu_allocator_nalloc0(allocator, item, size) xu_allocator_nalloc0_(allocator, item, size __xu_debug_vals__)

#define xu_allocator_ralloc(allocator, data, size)                                                                     \
    xu_allocator_ralloc_(allocator, (xu_pointer_t)(data), size __xu_debug_vals__)
#define xu_allocator_free(allocator, data) xu_allocator_free_(allocator, (xu_pointer_t)(data)__xu_debug_vals__)

#define xu_allocator_large_malloc(allocator, size, real)                                                               \
    xu_allocator_large_malloc_(allocator, size, real __xu_debug_vals__)
#define xu_allocator_large_malloc0(allocator, size, real)                                                              \
    xu_allocator_large_malloc0_(allocator, size, real __xu_debug_vals__)

#define xu_allocator_large_nalloc(allocator, item, size, real)                                                         \
    xu_allocator_large_nalloc_(allocator, item, size, real __xu_debug_vals__)
#define xu_allocator_large_nalloc0(allocator, item, size, real)                                                        \
    xu_allocator_large_nalloc0_(allocator, item, size, real __xu_debug_vals__)

#define xu_allocator_large_ralloc(allocator, data, size, real)                                                         \
    xu_allocator_large_ralloc_(allocator, (xu_pointer_t)(data), size, real __xu_debug_vals__)
#define xu_allocator_large_free(allocator, data)                                                                       \
    xu_allocator_large_free_(allocator, (xu_pointer_t)(data)__xu_debug_vals__)

#define xu_allocator_align_malloc(allocator, size, align)                                                              \
    xu_allocator_align_malloc_(allocator, size, align __xu_debug_vals__)
#define xu_allocator_align_malloc0(allocator, size, align)                                                             \
    xu_allocator_align_malloc0_(allocator, size, align __xu_debug_vals__)

#define xu_allocator_align_nalloc(allocator, item, size, align)                                                        \
    xu_allocator_align_nalloc_(allocator, item, size, align __xu_debug_vals__)
#define xu_allocator_align_nalloc0(allocator, item, size, align)                                                       \
    xu_allocator_align_nalloc0_(allocator, item, size, align __xu_debug_vals__)

#define xu_allocator_align_ralloc(allocator, data, size, align)                                                        \
    xu_allocator_align_ralloc_(allocator, (xu_pointer_t)(data), size, align __xu_debug_vals__)
#define xu_allocator_align_free(allocator, data)                                                                       \
    xu_allocator_align_free_(allocator, (xu_pointer_t)(data)__xu_debug_vals__)

/* *******************************************************
 *  types
 */

// the allocator type enum
typedef enum __xu_allocator_type_e
{
    XU_ALLOCATOR_TYPE_NONE    = 0,
    XU_ALLOCATOR_TYPE_DEFAULT = 1,
    XU_ALLOCATOR_TYPE_NATIVE  = 2,
    XU_ALLOCATOR_TYPE_VIRTUAL = 3,
    XU_ALLOCATOR_TYPE_STATIC  = 4,
    XU_ALLOCATOR_TYPE_LARGE   = 5,
    XU_ALLOCATOR_TYPE_SMALL   = 6

} xu_allocator_type_e;

// the allocator flag enum
typedef enum __xu_allocator_flag_e
{
    XU_ALLOCATOR_FLAG_NONE   = 0,
    XU_ALLOCATOR_FLAG_NOLOCK = 1

} xu_allocator_flag_e;

// the allocator type
typedef struct __xu_allocator_t
{
    /// the type
    xu_uint32_t type : 16;

    /// the flag
    xu_uint32_t flag : 16;

    /// the lock
    xu_spinlock_t lock;

    /*! malloc data
     *
     * @param allocator     the allocator
     * @param size          the size
     *
     * @return              the data address
     */
    xu_pointer_t (*malloc)(struct __xu_allocator_t* allocator, xu_size_t size __xu_debug_decl__);

    /*! realloc data
     *
     * @param allocator     the allocator
     * @param data          the data address
     * @param size          the data size
     *
     * @return              the new data address
     */
    xu_pointer_t (*ralloc)(struct __xu_allocator_t* allocator, xu_pointer_t data, xu_size_t size __xu_debug_decl__);

    /*! free data
     *
     * @param allocator     the allocator
     * @param data          the data address
     *
     * @return              xu_true or xu_false
     */
    xu_bool_t (*free)(struct __xu_allocator_t* allocator, xu_pointer_t data __xu_debug_decl__);

    /*! malloc large data
     *
     * @param allocator     the allocator
     * @param size          the size
     * @param real          the real allocated size >= size, optional
     *
     * @return              the data address
     */
    xu_pointer_t (*large_malloc)(struct __xu_allocator_t* allocator, xu_size_t size, xu_size_t* real __xu_debug_decl__);

    /*! realloc large data
     *
     * @param allocator     the allocator
     * @param data          the data address
     * @param size          the data size
     * @param real          the real allocated size >= size, optional
     *
     * @return              the new data address
     */
    xu_pointer_t (*large_ralloc)(struct __xu_allocator_t* allocator, xu_pointer_t data, xu_size_t size,
                                 xu_size_t* real __xu_debug_decl__);

    /*! free large data
     *
     * @param allocator     the allocator
     * @param data          the data address
     *
     * @return              xu_true or xu_false
     */
    xu_bool_t (*large_free)(struct __xu_allocator_t* allocator, xu_pointer_t data __xu_debug_decl__);

    /*! clear allocator
     *
     * @param allocator     the allocator
     */
    xu_void_t (*clear)(struct __xu_allocator_t* allocator);

    /*! exit allocator
     *
     * @param allocator     the allocator
     */
    xu_void_t (*exit)(struct __xu_allocator_t* allocator);

#ifdef __xu_debug__
    /*! dump allocator
     *
     * @param allocator     the allocator
     */
    xu_void_t (*dump)(struct __xu_allocator_t* allocator);

    /*! have this given data addess?
     *
     * @param allocator     the allocator
     * @param data          the data address
     *
     * @return              xu_true or xu_false
     */
    xu_bool_t (*have)(struct __xu_allocator_t* allocator, xu_cpointer_t data);
#endif

} xu_allocator_t, *xu_allocator_ref_t;

/* *******************************************************
 *  interfaces
 */

/*! the allocator
 *
 * @return              the allocator
 */
xu_allocator_ref_t xu_allocator(xu_noarg_t);

/*! the native allocator
 *
 * uses system memory directly
 *
 * @return              the allocator
 */
xu_allocator_ref_t xu_allocator_native(xu_noarg_t);

/*! the allocator type
 *
 * @param allocator     the allocator
 *
 * @return              the allocator type
 */
xu_size_t xu_allocator_type(xu_allocator_ref_t allocator);

/*! malloc data
 *
 * @param allocator     the allocator
 * @param size          the size
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_malloc_(xu_allocator_ref_t allocator, xu_size_t size __xu_debug_decl__);

/*! malloc data and fill zero
 *
 * @param allocator     the allocator
 * @param size          the size
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_malloc0_(xu_allocator_ref_t allocator, xu_size_t size __xu_debug_decl__);

/*! malloc data with the item count
 *
 * @param allocator     the allocator
 * @param item          the item count
 * @param size          the item size
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_nalloc_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size __xu_debug_decl__);

/*! malloc data with the item count and fill zero
 *
 * @param allocator     the allocator
 * @param item          the item count
 * @param size          the item size
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_nalloc0_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size __xu_debug_decl__);

/*! realloc data
 *
 * @param allocator     the allocator
 * @param data          the data address
 * @param size          the data size
 *
 * @return              the new data address
 */
xu_pointer_t xu_allocator_ralloc_(xu_allocator_ref_t allocator, xu_pointer_t data, xu_size_t size __xu_debug_decl__);

/*! free data
 *
 * @param allocator     the allocator
 * @param data          the data address
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_allocator_free_(xu_allocator_ref_t allocator, xu_pointer_t data __xu_debug_decl__);

/*! malloc large data
 *
 * @param allocator     the allocator
 * @param size          the size
 * @param real          the real allocated size >= size, optional
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_large_malloc_(xu_allocator_ref_t allocator, xu_size_t size,
                                        xu_size_t* real __xu_debug_decl__);

/*! malloc large data and fill zero
 *
 * @param allocator     the allocator
 * @param size          the size
 * @param real          the real allocated size >= size, optional
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_large_malloc0_(xu_allocator_ref_t allocator, xu_size_t size,
                                         xu_size_t* real __xu_debug_decl__);

/*! malloc large data with the item count
 *
 * @param allocator     the allocator
 * @param item          the item count
 * @param size          the item size
 * @param real          the real allocated size >= item * size, optional
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_large_nalloc_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size,
                                        xu_size_t* real __xu_debug_decl__);

/*! malloc large data with the item count and fill zero
 *
 * @param allocator     the allocator
 * @param item          the item count
 * @param size          the item size
 * @param real          the real allocated size >= item * size, optional
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_large_nalloc0_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size,
                                         xu_size_t* real __xu_debug_decl__);

/*! realloc large data
 *
 * @param allocator     the allocator
 * @param data          the data address
 * @param size          the data size
 * @param real          the real allocated size >= size, optional
 *
 * @return              the new data address
 */
xu_pointer_t xu_allocator_large_ralloc_(xu_allocator_ref_t allocator, xu_pointer_t data, xu_size_t size,
                                        xu_size_t* real __xu_debug_decl__);

/*! free large data
 *
 * @param allocator     the allocator
 * @param data          the data address
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_allocator_large_free_(xu_allocator_ref_t allocator, xu_pointer_t data __xu_debug_decl__);

/*! align malloc data
 *
 * @param allocator     the allocator
 * @param size          the size
 * @param align         the alignment bytes
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_align_malloc_(xu_allocator_ref_t allocator, xu_size_t size,
                                        xu_size_t align __xu_debug_decl__);

/*! align malloc data and fill zero
 *
 * @param allocator     the allocator
 * @param size          the size
 * @param align         the alignment bytes
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_align_malloc0_(xu_allocator_ref_t allocator, xu_size_t size,
                                         xu_size_t align __xu_debug_decl__);

/*! align malloc data with the item count
 *
 * @param allocator     the allocator
 * @param item          the item count
 * @param size          the item size
 * @param align         the alignment bytes
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_align_nalloc_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size,
                                        xu_size_t align __xu_debug_decl__);

/*! align malloc data with the item count and fill zero
 *
 * @param allocator     the allocator
 * @param item          the item count
 * @param size          the item size
 * @param align         the alignment bytes
 *
 * @return              the data address
 */
xu_pointer_t xu_allocator_align_nalloc0_(xu_allocator_ref_t allocator, xu_size_t item, xu_size_t size,
                                         xu_size_t align __xu_debug_decl__);

/*! align realloc data
 *
 * @param allocator     the allocator
 * @param data          the data address
 * @param size          the data size
 *
 * @return              the new data address
 */
xu_pointer_t xu_allocator_align_ralloc_(xu_allocator_ref_t allocator, xu_pointer_t data, xu_size_t size,
                                        xu_size_t align __xu_debug_decl__);

/*! align free data
 *
 * @param allocator     the allocator
 * @param data          the data address
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_allocator_align_free_(xu_allocator_ref_t allocator, xu_pointer_t data __xu_debug_decl__);

/*! clear it
 *
 * @param allocator     the allocator
 */
xu_void_t xu_allocator_clear(xu_allocator_ref_t allocator);

/*! exit it
 *
 * @param allocator     the allocator
 */
xu_void_t xu_allocator_exit(xu_allocator_ref_t allocator);

#ifdef __xu_debug__
/*! dump it
 *
 * @param allocator     the allocator
 */
xu_void_t xu_allocator_dump(xu_allocator_ref_t allocator);

/*! have this given data addess?
 *
 * @param allocator     the allocator
 * @param data          the data address
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_allocator_have(xu_allocator_ref_t allocator, xu_cpointer_t data);
#endif
