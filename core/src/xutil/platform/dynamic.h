#pragma once

#include "xutil/macros/assert.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  types
 * *******************************************************
 */

// the dynamic ref type
typedef __xu_typeref__(dynamic);

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
/*! init dynamic library
 *
 * @param name      the library name
 *
 * @return          the dynamic library
 */
xu_dynamic_ref_t xu_dynamic_init(xu_char_t const* name);

/*! exit dynamic library
 *
 * @param dynamic   the dynamic library
 */
xu_void_t xu_dynamic_exit(xu_dynamic_ref_t dynamic);

/*! the dynamic library function
 *
 * @param dynamic   the dynamic library
 * @param name      the function name
 *
 * @return          the function address
 */
xu_pointer_t xu_dynamic_func(xu_dynamic_ref_t dynamic, xu_char_t const* name);

/*! the dynamic library variable
 *
 * @param dynamic   the dynamic library
 * @param name      the variable name
 *
 * @return          the variable address
 */
xu_pointer_t xu_dynamic_pvar(xu_dynamic_ref_t dynamic, xu_char_t const* name);
