#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

#define xu_free(data) xu_allocator_free_(xu_allocator(), (xu_pointer_t)data __xu_debug_vals__)
#define xu_malloc(size) xu_allocator_malloc_(xu_allocator(), size __xu_debug_vals__)
#define xu_malloc0(size) xu_allocator_malloc0_(xu_allocator(), size __xu_debug_vals__)
#define xu_nalloc(item, size) xu_allocator_nalloc_(xu_allocator(), item, size __xu_debug_vals__)
#define xu_nalloc0(item, size) xu_allocator_nalloc0_(xu_allocator(), item, size __xu_debug_vals__)
#define xu_ralloc(data, size) xu_allocator_ralloc_(xu_allocator(), (xu_pointer_t)data, size __xu_debug_vals__)

#define xu_malloc_cstr(size) (xu_char_t*)xu_allocator_malloc_(xu_allocator(), size __xu_debug_vals__)
#define xu_malloc0_cstr(size) (xu_char_t*)xu_allocator_malloc0_(xu_allocator(), size __xu_debug_vals__)
#define xu_nalloc_cstr(item, size) (xu_char_t*)xu_allocator_nalloc_(xu_allocator(), item, size __xu_debug_vals__)
#define xu_nalloc0_cstr(item, size) (xu_char_t*)xu_allocator_nalloc0_(xu_allocator(), item, size __xu_debug_vals__)
#define xu_ralloc_cstr(data, size)                                                                                     \
    (xu_char_t*)xu_allocator_ralloc_(xu_allocator(), (xu_pointer_t)data, size __xu_debug_vals__)

#define xu_malloc_bytes(size) (xu_byte_t*)xu_allocator_malloc_(xu_allocator(), size __xu_debug_vals__)
#define xu_malloc0_bytes(size) (xu_byte_t*)xu_allocator_malloc0_(xu_allocator(), size __xu_debug_vals__)
#define xu_nalloc_bytes(item, size) (xu_byte_t*)xu_allocator_nalloc_(xu_allocator(), item, size __xu_debug_vals__)
#define xu_nalloc0_bytes(item, size) (xu_byte_t*)xu_allocator_nalloc0_(xu_allocator(), item, size __xu_debug_vals__)
#define xu_ralloc_bytes(data, size)                                                                                    \
    (xu_byte_t*)xu_allocator_ralloc_(xu_allocator(), (xu_pointer_t)data, size __xu_debug_vals__)

#define xu_malloc_type(type) (type*)xu_allocator_malloc_(xu_allocator(), sizeof(type) __xu_debug_vals__)
#define xu_malloc0_type(type) (type*)xu_allocator_malloc0_(xu_allocator(), sizeof(type) __xu_debug_vals__)
#define xu_nalloc_type(item, type) (type*)xu_allocator_nalloc_(xu_allocator(), item, sizeof(type) __xu_debug_vals__)
#define xu_nalloc0_type(item, type) (type*)xu_allocator_nalloc0_(xu_allocator(), item, sizeof(type) __xu_debug_vals__)
#define xu_ralloc_type(data, item, type)                                                                               \
    (type*)xu_allocator_ralloc_(xu_allocator(), (xu_pointer_t)data, ((item) * sizeof(type)) __xu_debug_vals__)

#define xu_align_free(data) xu_allocator_align_free_(xu_allocator(), (xu_pointer_t)data __xu_debug_vals__)
#define xu_align_malloc(size, align) xu_allocator_align_malloc_(xu_allocator(), size, align __xu_debug_vals__)
#define xu_align_malloc0(size, align) xu_allocator_align_malloc0_(xu_allocator(), size, align __xu_debug_vals__)
#define xu_align_nalloc(item, size, align)                                                                             \
    xu_allocator_align_nalloc_(xu_allocator(), item, size, align __xu_debug_vals__)
#define xu_align_nalloc0(item, size, align)                                                                            \
    xu_allocator_align_nalloc0_(xu_allocator(), item, size, align __xu_debug_vals__)
#define xu_align_ralloc(data, size, align)                                                                             \
    xu_allocator_align_ralloc_(xu_allocator(), (xu_pointer_t)data, size, align __xu_debug_vals__)

#define xu_large_free(data) xu_allocator_large_free_(xu_allocator(), (xu_pointer_t)data __xu_debug_vals__)
#define xu_large_malloc(size, real) xu_allocator_large_malloc_(xu_allocator(), size, real __xu_debug_vals__)
#define xu_large_malloc0(size, real) xu_allocator_large_malloc0_(xu_allocator(), size, real __xu_debug_vals__)
#define xu_large_nalloc(item, size, real) xu_allocator_large_nalloc_(xu_allocator(), item, size, real __xu_debug_vals__)
#define xu_large_nalloc0(item, size, real)                                                                             \
    xu_allocator_large_nalloc0_(xu_allocator(), item, size, real __xu_debug_vals__)
#define xu_large_ralloc(data, size, real)                                                                              \
    xu_allocator_large_ralloc_(xu_allocator(), (xu_pointer_t)data, size, real __xu_debug_vals__)

#if XU_CPU_BIT64
#    define xu_align8_free(data) xu_free((xu_pointer_t)data)
#    define xu_align8_malloc(size) xu_malloc(size)
#    define xu_align8_malloc0(size) xu_malloc0(size)
#    define xu_align8_nalloc(item, size) xu_nalloc(item, size)
#    define xu_align8_nalloc0(item, size) xu_nalloc0(item, size)
#    define xu_align8_ralloc(data, size) xu_ralloc((xu_pointer_t)data, size)
#else
#    define xu_align8_free(data) xu_align_free((xu_pointer_t)data)
#    define xu_align8_malloc(size) xu_align_malloc(size, 8)
#    define xu_align8_malloc0(size) xu_align_malloc0(size, 8)
#    define xu_align8_nalloc(item, size) xu_align_nalloc(item, size, 8)
#    define xu_align8_nalloc0(item, size) xu_align_nalloc0(item, size, 8)
#    define xu_align8_ralloc(data, size) xu_align_ralloc((xu_pointer_t)data, size, 8)
#endif

/* *******************************************************
 * declaration
 * *******************************************************
 */

struct __xu_allocator_t;
struct __xu_allocator_t* xu_allocator(xu_noarg_t);
xu_pointer_t             xu_allocator_malloc_(struct __xu_allocator_t* allocator, xu_size_t size __xu_debug_decl__);
xu_pointer_t             xu_allocator_malloc0_(struct __xu_allocator_t* allocator, xu_size_t size __xu_debug_decl__);
xu_pointer_t xu_allocator_nalloc_(struct __xu_allocator_t* allocator, xu_size_t item, xu_size_t size __xu_debug_decl__);
xu_pointer_t xu_allocator_nalloc0_(struct __xu_allocator_t* allocator, xu_size_t item,
                                   xu_size_t size __xu_debug_decl__);
xu_pointer_t xu_allocator_ralloc_(struct __xu_allocator_t* allocator, xu_pointer_t data,
                                  xu_size_t size __xu_debug_decl__);
xu_bool_t    xu_allocator_free_(struct __xu_allocator_t* allocator, xu_pointer_t data __xu_debug_decl__);
xu_pointer_t xu_allocator_align_malloc_(struct __xu_allocator_t* allocator, xu_size_t size,
                                        xu_size_t align __xu_debug_decl__);
xu_pointer_t xu_allocator_align_malloc0_(struct __xu_allocator_t* allocator, xu_size_t size,
                                         xu_size_t align __xu_debug_decl__);
xu_pointer_t xu_allocator_align_nalloc_(struct __xu_allocator_t* allocator, xu_size_t item, xu_size_t size,
                                        xu_size_t align __xu_debug_decl__);
xu_pointer_t xu_allocator_align_nalloc0_(struct __xu_allocator_t* allocator, xu_size_t item, xu_size_t size,
                                         xu_size_t align __xu_debug_decl__);
xu_pointer_t xu_allocator_align_ralloc_(struct __xu_allocator_t* allocator, xu_pointer_t data, xu_size_t size,
                                        xu_size_t align __xu_debug_decl__);
xu_bool_t    xu_allocator_align_free_(struct __xu_allocator_t* allocator, xu_pointer_t data __xu_debug_decl__);
xu_pointer_t xu_allocator_large_malloc_(struct __xu_allocator_t* allocator, xu_size_t size,
                                        xu_size_t* real __xu_debug_decl__);
xu_pointer_t xu_allocator_large_malloc0_(struct __xu_allocator_t* allocator, xu_size_t size,
                                         xu_size_t* real __xu_debug_decl__);
xu_pointer_t xu_allocator_large_nalloc_(struct __xu_allocator_t* allocator, xu_size_t item, xu_size_t size,
                                        xu_size_t* real __xu_debug_decl__);
xu_pointer_t xu_allocator_large_nalloc0_(struct __xu_allocator_t* allocator, xu_size_t item, xu_size_t size,
                                         xu_size_t* real __xu_debug_decl__);
xu_pointer_t xu_allocator_large_ralloc_(struct __xu_allocator_t* allocator, xu_pointer_t data, xu_size_t size,
                                        xu_size_t* real __xu_debug_decl__);
xu_bool_t    xu_allocator_large_free_(struct __xu_allocator_t* allocator, xu_pointer_t data __xu_debug_decl__);
