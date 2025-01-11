#pragma once

#include "xutil/macros/keyword.h"
#include <stdint.h>

/* *******************************************************
 *  basic types
 * *******************************************************
 */
typedef int32_t          xm_int_t;
typedef uint32_t         xm_uint_t;
typedef int16_t          xm_short_t;
typedef uint16_t         xm_ushort_t;
typedef xm_int_t         xm_bool_t;
typedef int8_t           xm_int8_t;
typedef xm_int8_t        xm_sint8_t;
typedef unsigned char    xm_uint8_t;
typedef xm_short_t       xm_int16_t;
typedef xm_int16_t       xm_sint16_t;
typedef xm_ushort_t      xm_uint16_t;
typedef xm_int_t         xm_int32_t;
typedef xm_int32_t       xm_sint32_t;
typedef xm_uint_t        xm_uint32_t;
typedef char             xm_char_t;
typedef unsigned char    xm_uchar_t;
typedef xm_uint8_t       xm_byte_t;
typedef void             xm_void_t;
typedef xm_void_t*       xm_pointer_t;
typedef xm_void_t const* xm_cpointer_t;
typedef xm_pointer_t     xm_handle_t;

/* *******************************************************
 *  bool values
 * *******************************************************
 */
#define xm_true ((xm_bool_t)1)
#define xm_false ((xm_bool_t)0)

/* *******************************************************
 *  null
 * *******************************************************
 */
#ifdef __cplusplus
#    define xm_null (0)
#else
#    define xm_null ((xm_pointer_t)0)
#endif

/* *******************************************************
 *  no argument
 * *******************************************************
 */
#ifdef __cplusplus
#    define xm_noarg_t
#else
#    define xm_noarg_t xm_void_t
#endif

/* *******************************************************
 *  atomic type
 * *******************************************************
 */
#if (__xm_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__))
typedef __xm_volatile__ _Atomic xm_int32_t xm_atomic32_t;
#else
typedef __xm_volatile__ __xm_aligned__(4) xm_int32_t xm_atomic32_t;
#endif

#if XM_CPU_BIT64
typedef xm_atomic64_t xm_atomic_t;
#else
typedef xm_atomic32_t   xm_atomic_t;
#endif

#if (__xm_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__))
typedef __xm_volatile__ atomic_flag xm_atomic_flag_t;
#elif (defined(XM_COMPILER_IS_GCC) && defined(__ATOMIC_SEQ_CST))
typedef __xm_volatile__ struct __xm_atomic_flag_t
{
    unsigned char __val;
} xm_atomic_flag_t;
#else
typedef __xm_volatile__ struct __xm_atomic_flag_t
{
    xm_atomic32_t __val;
} xm_atomic_flag_t;
#endif

// int64_t
typedef int64_t    xm_int64_t;
typedef uint64_t   xm_uint64_t;
typedef xm_int64_t xm_sint64_t;

// long and size
typedef int64_t    xm_long_t;
typedef uint64_t   xm_ulong_t;
typedef xm_ulong_t xm_size_t;

// hong and hize
typedef xm_sint64_t xm_hong_t;
typedef xm_uint64_t xm_hize_t;

// the spinlock type
typedef xm_atomic_flag_t xm_spinlock_t;

// the spinlock ref type
typedef xm_spinlock_t* xm_spinlock_ref_t;

// the pool ref type
typedef __xm_typeref__(pool);

// the file ref type
typedef __xm_typeref__(file);

// the socket ref type
typedef __xm_typeref__(socket);

// the event ref type
typedef __xm_typeref__(event);

// the mutex ref type
typedef __xm_typeref__(mutex);

// the thread ref type
typedef __xm_typeref__(thread);

// the semaphore ref type
typedef __xm_typeref__(semaphore);

// the stream ref type
typedef __xm_typeref__(stream);

// the async stream ref type
typedef __xm_typeref__(async_stream);

// the time type
typedef xm_hong_t xm_time_t;

// the suseconds type
typedef xm_long_t xm_suseconds_t;
