#pragma once

#include "xutil/xutil.h"

/* *******************************************************
 *  main decl
 * *******************************************************
 */
#define XU_DEMO_MAIN_DECL(name) xu_int_t xu_ut_##name##_main(xu_int_t argc, xu_char_t** argv)

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

// utils
XU_DEMO_MAIN_DECL(utils_trace);
