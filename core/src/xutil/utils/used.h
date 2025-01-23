#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

#define xu_used(ptr) xu_used_ptr((xu_cpointer_t)(xu_size_t)(ptr))

/* *******************************************************
 * interfaces
 */
/*! this variable have been used
 *
 * @param variable      the variable
 */
xu_void_t xu_used_ptr(xu_cpointer_t variable);
