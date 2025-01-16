#pragma once

#include "xutil/prefix.h"

/* init libm environment
 *
 * @return  xu_true or xu_false
 */
xu_bool_t xu_libm_init_env(xu_noarg_t);

/* exit libm environment
 */
xu_void_t xu_libm_exit_env(xu_noarg_t);
