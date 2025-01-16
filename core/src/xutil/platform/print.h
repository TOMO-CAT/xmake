#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/*! print string and without newline
 *
 * @param string    the print string
 */
xu_void_t xu_print(xu_char_t const* string);

/*! print string and newline
 *
 * @param string    the print string
 */
xu_void_t xu_printl(xu_char_t const* string);

/*! sync print
 */
xu_void_t xu_print_sync(xu_noarg_t);
