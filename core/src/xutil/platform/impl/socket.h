#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

// init socket environment
xu_bool_t xu_socket_init_env(xu_noarg_t);

// exit socket environment
xu_void_t xu_socket_exit_env(xu_noarg_t);
