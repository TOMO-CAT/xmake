#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 */
// the unixaddr string data maxn
#define XU_UNIXADDR_CSTR_MAXN (108)

/* *******************************************************
 *  types
 */

/*! the unix type
 *
 * /path/to.sock
 */
typedef struct __xu_unixaddr_t
{
    // is abstract
    xu_bool_t is_abstract;
    // path
    xu_char_t path[XU_UNIXADDR_CSTR_MAXN];

} xu_unixaddr_t, *xu_unixaddr_ref_t;

/* *******************************************************
 *  interfaces
 */

/*! clear the unixaddr
 *
 * @param unixaddr  the unixaddr
 */
xu_void_t xu_unixaddr_clear(xu_unixaddr_ref_t unixaddr);

/*! is abstract?
 *
 * @param unixaddr  the unixaddr
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_unixaddr_is_abstract(xu_unixaddr_ref_t unixaddr);

/*! is equal?
 *
 * @param unixaddr  the unixaddr
 * @param other     the other unixaddr
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_unixaddr_is_equal(xu_unixaddr_ref_t unixaddr, xu_unixaddr_ref_t other);

/*! get the unixaddr string
 *
 * @param unixaddr  the unixaddr
 * @param data      the unixaddr data
 * @param maxn      the data maxn
 *
 * @return          the unixaddr path
 */
xu_char_t const* xu_unixaddr_cstr(xu_unixaddr_ref_t unixaddr, xu_char_t* data, xu_size_t maxn);

/*! set the unixaddr from string
 *
 * @param unixaddr  the unixaddr
 * @param cstr      the unixaddr path
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_unixaddr_cstr_set(xu_unixaddr_ref_t unixaddr, xu_char_t const* cstr, xu_bool_t is_abstract);
