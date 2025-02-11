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

// container
XU_UT_MAIN_DECL(container_bloom_filter);
XU_UT_MAIN_DECL(container_circle_queue);
XU_UT_MAIN_DECL(container_hash_map);
XU_UT_MAIN_DECL(container_hash_set);
XU_UT_MAIN_DECL(container_heap);

// memory
XU_UT_MAIN_DECL(memory_buffer);
XU_UT_MAIN_DECL(memory_check);

// utils
XU_UT_MAIN_DECL(utils_trace);
