#pragma once

#include "xutil/xutil.h"

/* *******************************************************
 *  main decl
 */
#define XU_UT_MAIN_DECL(name) xu_int_t xu_ut_##name##_main(xu_int_t argc, xu_char_t** argv)

/* *******************************************************
 *  interfaces
 */

// algorithm
XU_UT_MAIN_DECL(algorithm_find);
XU_UT_MAIN_DECL(algorithm_sort);
XU_UT_MAIN_DECL(container_circle_queue);

// container
XU_UT_MAIN_DECL(container_bloom_filter);

// memory
XU_UT_MAIN_DECL(memory_buffer);
XU_UT_MAIN_DECL(memory_check);

// utils
XU_UT_MAIN_DECL(utils_trace);
