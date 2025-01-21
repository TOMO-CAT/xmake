#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/*! init network environment
 *
 * @return      xu_true or xu_false
 */
xu_bool_t xu_network_init_env(xu_noarg_t);

/// exit network environment
xu_void_t xu_network_exit_env(xu_noarg_t);
