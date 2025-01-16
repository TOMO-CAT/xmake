#pragma once

#include "xutil/prefix.h"

// the string pool ref type
typedef __xu_typeref__(string_pool);

/*! init string pool for small, readonly and repeat strings
 *
 * readonly, strip repeat strings and decrease memory fragmens
 *
 * @param bcase             is case?
 *
 * @return                  the string pool
 */
xu_string_pool_ref_t xu_string_pool_init(xu_bool_t bcase);

/*! exit the string pool
 *
 * @param pool              the string pool
 */
xu_void_t xu_string_pool_exit(xu_string_pool_ref_t pool);

/*! clear the string pool
 *
 * @param pool              the string pool
 */
xu_void_t xu_string_pool_clear(xu_string_pool_ref_t pool);

/*! insert string to the pool and increase the reference count
 *
 * @param pool              the string pool
 * @param data              the string data
 *
 * @return                  the string data
 */
xu_char_t const* xu_string_pool_insert(xu_string_pool_ref_t pool, xu_char_t const* data);

/*! remove string from the pool if the reference count be zero
 *
 * @param pool              the string pool
 * @param data              the string data
 */
xu_void_t xu_string_pool_remove(xu_string_pool_ref_t pool, xu_char_t const* data);

/*! has this string?
 *
 * @param pool              the string pool
 * @param data              the string data
 *
 * @return                  xu_true or xu_false
 */
xu_bool_t xu_string_pool_has(xu_string_pool_ref_t pool, xu_char_t const* data);

#ifdef __xu_debug__
/*! dump the string pool
 *
 * @param pool              the string pool
 */
xu_void_t xu_string_pool_dump(xu_string_pool_ref_t pool);
#endif
