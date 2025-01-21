#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/*! init page
 *
 * @note
 * need support to be called repeatedly
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_page_init(xu_noarg_t);

/*! exit page
 *
 */
xu_void_t xu_page_exit(xu_noarg_t);

/*! get page size
 *
 * @return              the page size
 */
xu_size_t xu_page_size(xu_noarg_t);
