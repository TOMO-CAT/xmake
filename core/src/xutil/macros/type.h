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
 *  wchar
 * *******************************************************
 */
#ifdef XM_CONFIG_TYPE_HAVE_WCHAR
    typedef wchar_t                 xm_wchar_t;
#elif defined(__WCHAR_TYPE__)
typedef __WCHAR_TYPE__              xm_wchar_t;
#elif defined(__WCHAR_WIDTH__) && (__WCHAR_WIDTH__ == 2)
typedef xm_int16_t                  xm_wchar_t;
#elif defined(__WCHAR_WIDTH__) && (__WCHAR_WIDTH__ == 4)
typedef xm_int32_t                  xm_wchar_t;
#else
typedef xm_int32_t                  xm_wchar_t;
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

// the timezone type
typedef struct __xm_timezone_t
{
    xm_int_t                        tz_minuteswest;
    xm_int_t                        tz_dsttime;

}xm_timezone_t;

// the version type
typedef struct __xm_version_t
{
    xm_byte_t                       major;
    xm_byte_t                       minor;
    xm_byte_t                       alter;
    xm_hize_t                       build;

}xm_version_t;

// the value type
typedef union __xm_value_t
{
    xm_uint8_t                      u8;
    xm_sint8_t                      s8;
    xm_char_t                       c;
    xm_wchar_t                      wc;
    xm_bool_t                       b;
    xm_uint16_t                     u16;
    xm_sint16_t                     s16;
    xm_uint32_t                     u32;
    xm_sint32_t                     s32;
    xm_uint64_t                     u64;
    xm_sint64_t                     s64;
    xm_size_t                       ul;
    xm_long_t                       l;
    xm_time_t                       t;
    xm_pointer_t                    ptr;
    xm_cpointer_t                   cptr;
    xm_socket_ref_t                 sock;
    xm_file_ref_t                   file;
    xm_handle_t                     handle;
    xm_char_t*                      str;
    xm_char_t const*                cstr;
    xm_wchar_t*                     wstr;
    xm_wchar_t const*               wcstr;
    xm_atomic_t                     a;
    xm_atomic_t                     a64;
    xm_spinlock_t                   lock;
    xm_stream_ref_t                 stream;
    xm_async_stream_ref_t           astream;

#ifdef xm_CONFIG_TYPE_HAVE_FLOAT
    xm_float_t                      f;
    xm_double_t                     d;
#endif

}xm_value_t, *xm_value_ref_t;
