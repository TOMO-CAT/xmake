#pragma once

#include "xutil/xutil.h"

/* *******************************************************
 *  main decl
 */
#define XU_UT_MAIN_DECL(name) xu_int_t xu_ut_##name##_main(xu_int_t argc, xu_char_t** argv)

/* *******************************************************
 *  interfaces
 */

// memory
XU_UT_MAIN_DECL(memory_buffer);
XU_UT_MAIN_DECL(memory_check);

// utils
XU_UT_MAIN_DECL(utils_trace);
