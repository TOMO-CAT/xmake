#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

/*! init the platform environment
 *
 * @param priv      the platform private data
 *                  pass JavaVM* jvm for android
 *                  pass xu_null for other platform
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_platform_init_env(xu_handle_t priv);

// exit the platform environment
xu_void_t xu_platform_exit_env(xu_noarg_t);
