#pragma once

#include "xutil/libc/stdio/printf_object.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  interfaces
 */

/* init libc environment
 *
 * @return  xu_true or xu_false
 */
xu_bool_t xu_libc_init_env(xu_noarg_t);

/* exit libc environment
 */
xu_void_t xu_libc_exit_env(xu_noarg_t);
