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
XU_UT_MAIN_DECL(container_list_entry);
XU_UT_MAIN_DECL(container_list);
XU_UT_MAIN_DECL(container_queue);
XU_UT_MAIN_DECL(container_single_list_entry);
XU_UT_MAIN_DECL(container_single_list);
XU_UT_MAIN_DECL(container_stack);
XU_UT_MAIN_DECL(container_vector);

// hash
XU_UT_MAIN_DECL(hash_adler32);
XU_UT_MAIN_DECL(hash_benchmark);
XU_UT_MAIN_DECL(hash_crc8);
XU_UT_MAIN_DECL(hash_crc16);
XU_UT_MAIN_DECL(hash_crc32);
XU_UT_MAIN_DECL(hash_djb2);
XU_UT_MAIN_DECL(hash_fnv32);
XU_UT_MAIN_DECL(hash_fnv64);
XU_UT_MAIN_DECL(hash_md5);
XU_UT_MAIN_DECL(hash_sdbm);
XU_UT_MAIN_DECL(hash_sha);
XU_UT_MAIN_DECL(hash_uuid);

// libc
XU_UT_MAIN_DECL(libc_stdlib);
XU_UT_MAIN_DECL(libc_string);
XU_UT_MAIN_DECL(libc_time);

// libm
XU_UT_MAIN_DECL(libm_double);
XU_UT_MAIN_DECL(libm_float);
XU_UT_MAIN_DECL(libm_integer);

// math
XU_UT_MAIN_DECL(math_fixed);
XU_UT_MAIN_DECL(math_random);

// memory
XU_UT_MAIN_DECL(memory_impl_static_fixed_pool);
XU_UT_MAIN_DECL(memory_buffer);
XU_UT_MAIN_DECL(memory_check);
XU_UT_MAIN_DECL(memory_default_allocator);
XU_UT_MAIN_DECL(memory_fixed_pool);
XU_UT_MAIN_DECL(memory_large_allocator);
XU_UT_MAIN_DECL(memory_memops);
XU_UT_MAIN_DECL(memory_queue_buffer);
XU_UT_MAIN_DECL(memory_small_allocator);
XU_UT_MAIN_DECL(memory_static_buffer);
XU_UT_MAIN_DECL(memory_string_pool);

// utils
XU_UT_MAIN_DECL(utils_trace);
