#pragma once

#include "xutil/algorithm/walk.h"
#include "xutil/prefix.h"

/* init the thread local environment
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_thread_local_init_env(xu_noarg_t);

// exit the thread local environment
xu_void_t xu_thread_local_exit_env(xu_noarg_t);

/* walk all thread locals
 *
 * @param func      the walk function
 * @param priv      the user private data
 */
xu_void_t xu_thread_local_walk(xu_walk_func_t func, xu_cpointer_t priv);
