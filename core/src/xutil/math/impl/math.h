#pragma once

#include "xutil/libc/stdio/printf_object.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/macros/assert.h"
#include "xutil/math/fixed.h"
#include "xutil/prefix.h"
#include "xutil/utils/utils.h"

/* init math environment
 *
 * @return      xu_true or xu_false
 */
xu_bool_t xu_math_init_env(xu_noarg_t);

// exit math environment
xu_void_t xu_math_exit_env(xu_noarg_t);
