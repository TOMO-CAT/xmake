#pragma once

#include "xutil/prefix.h"

#include <stdint.h>

/* *******************************************************
 *  includes
 */
#include "xutil/libc/misc/stdarg.h"
#include "xutil/macros/keyword.h"

/* *******************************************************
 *  basic types
 */
typedef signed int       xu_int_t;
typedef unsigned int     xu_uint_t;
typedef signed short     xu_short_t;
typedef unsigned short   xu_ushort_t;
typedef xu_int_t         xu_bool_t;
typedef signed char      xu_int8_t;
typedef xu_int8_t        xu_sint8_t;
typedef unsigned char    xu_uint8_t;
typedef xu_short_t       xu_int16_t;
typedef xu_int16_t       xu_sint16_t;
typedef xu_ushort_t      xu_uint16_t;
typedef xu_int_t         xu_int32_t;
typedef xu_int32_t       xu_sint32_t;
typedef xu_uint_t        xu_uint32_t;
typedef char             xu_char_t;
typedef unsigned char    xu_uchar_t;
typedef xu_uint8_t       xu_byte_t;
typedef void             xu_void_t;
typedef xu_void_t*       xu_pointer_t;
typedef xu_void_t const* xu_cpointer_t;
typedef xu_pointer_t     xu_handle_t;

/* *******************************************************
 *  bool values
 */
#define xu_true ((xu_bool_t)1)
#define xu_false ((xu_bool_t)0)

/* *******************************************************
 *  null
 */
#ifdef __cplusplus
#    define xu_null (0)
#else
#    define xu_null ((xu_pointer_t)0)
#endif

/* *******************************************************
 *  no argument
 */
#ifdef __cplusplus
#    define xu_noarg_t
#else
#    define xu_noarg_t xu_void_t
#endif

/* *******************************************************
 *  wchar
 */
#ifdef XU_CONFIG_TYPE_HAVE_WCHAR
typedef wchar_t xu_wchar_t;
#elif defined(__WCHAR_TYPE__)
typedef __WCHAR_TYPE__     xu_wchar_t;
#elif defined(__WCHAR_WIDTH__) && (__WCHAR_WIDTH__ == 2)
typedef xu_int16_t xu_wchar_t;
#elif defined(__WCHAR_WIDTH__) && (__WCHAR_WIDTH__ == 4)
typedef xu_int32_t xu_wchar_t;
#else
typedef xu_int32_t xu_wchar_t;
#endif

// int64_t
#if (defined(__LONG_WIDTH__) && __LONG_WIDTH__ == 8)
|| (defined(__SIZEOF_LONG__) && __SIZEOF_LONG__ == 8) typedef signed long xu_int64_t;
typedef unsigned long xu_uint64_t;
#else
typedef signed long long   xu_int64_t;
typedef unsigned long long xu_uint64_t;
#endif
typedef xu_int64_t xu_sint64_t;

// hong and hize
typedef xu_sint64_t xu_hong_t;
typedef xu_uint64_t xu_hize_t;

// long and size
typedef signed long   xu_long_t;
typedef unsigned long xu_ulong_t;
typedef xu_ulong_t    xu_size_t;

// integer pointer
typedef xu_long_t  xu_ptrdiff_t;
typedef xu_ulong_t xu_uintptr_t;

// double
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
typedef float  xu_float_t;
typedef double xu_double_t;
#endif

// fixed
typedef xu_int32_t   xu_fixed6_t;
typedef xu_int32_t   xu_fixed16_t;
typedef xu_int32_t   xu_fixed30_t;
typedef xu_fixed16_t xu_fixed_t;

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT

// the ieee float type
typedef union __xu_ieee_float_t
{
    xu_float_t  f;
    xu_uint32_t i;

} xu_ieee_float_t;

// the ieee double type
#    ifdef XU_FLOAT_BIGENDIAN
typedef union __xu_ieee_double_t
{
    xu_double_t d;
    struct
    {
        xu_uint32_t h;
        xu_uint32_t l;

    } i;

} xu_ieee_double_t;
#    else
typedef union __xu_ieee_double_t
{
    xu_double_t d;
    struct
    {
        xu_uint32_t l;
        xu_uint32_t h;
    } i;

} xu_ieee_double_t;
#    endif
#endif

/* *******************************************************
 *  atomic type
 */
#if (__xu_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__))
typedef __xu_volatile__ _Atomic xu_int32_t xu_atomic32_t;
#else
typedef __xu_volatile__    __xu_aligned__(4) xu_int32_t xu_atomic32_t;
#endif

// the atomic64 type, need be aligned for arm, ..
#if (__xu_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__))
typedef __xu_volatile__ _Atomic xu_int64_t xu_atomic64_t;
#else
typedef __xu_volatile__    __xu_aligned__(8) xu_int64_t xu_atomic64_t;
#endif

#if XU_CPU_BIT64
typedef xu_atomic64_t xu_atomic_t;
#else
typedef xu_atomic32_t      xu_atomic_t;
#endif

#if (__xu_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__))
typedef __xu_volatile__ atomic_flag xu_atomic_flag_t;
#elif (defined(XU_COMPILER_IS_GCC) && defined(__ATOMIC_SEQ_CST))
typedef __xu_volatile__ struct __xu_atomic_flag_t
{
    unsigned char __val;
} xu_atomic_flag_t;
#else
typedef __xu_volatile__ struct __xu_atomic_flag_t
{
    xu_atomic32_t __val;
} xu_atomic_flag_t;
#endif

// the spinlock type
typedef xu_atomic_flag_t xu_spinlock_t;

// the spinlock ref type
typedef xu_spinlock_t* xu_spinlock_ref_t;

// the pool ref type
typedef __xu_typeref__(pool);

// the file ref type
typedef __xu_typeref__(file);

// the socket ref type
typedef __xu_typeref__(socket);

// the event ref type
typedef __xu_typeref__(event);

// the mutex ref type
typedef __xu_typeref__(mutex);

// the thread ref type
typedef __xu_typeref__(thread);

// the semaphore ref type
typedef __xu_typeref__(semaphore);

// the stream ref type
typedef __xu_typeref__(stream);

// the async stream ref type
typedef __xu_typeref__(async_stream);

// the time type
typedef xu_hong_t xu_time_t;

// the suseconds type
typedef xu_long_t xu_suseconds_t;

// the timeval type
typedef struct __xu_timeval_t
{
    xu_time_t      tv_sec;
    xu_suseconds_t tv_usec;

} xu_timeval_t;

// the timezone type
typedef struct __xu_timezone_t
{
    xu_int_t tz_minuteswest;
    xu_int_t tz_dsttime;

} xu_timezone_t;

// the version type
typedef struct __xu_version_t
{
    xu_byte_t major;
    xu_byte_t minor;
    xu_byte_t alter;
    xu_hize_t build;

} xu_version_t;

// the value type
typedef union __xu_value_t
{
    xu_uint8_t            u8;
    xu_sint8_t            s8;
    xu_char_t             c;
    xu_wchar_t            wc;
    xu_bool_t             b;
    xu_uint16_t           u16;
    xu_sint16_t           s16;
    xu_uint32_t           u32;
    xu_sint32_t           s32;
    xu_uint64_t           u64;
    xu_sint64_t           s64;
    xu_size_t             ul;
    xu_long_t             l;
    xu_time_t             t;
    xu_pointer_t          ptr;
    xu_cpointer_t         cptr;
    xu_socket_ref_t       sock;
    xu_file_ref_t         file;
    xu_handle_t           handle;
    xu_char_t*            str;
    xu_char_t const*      cstr;
    xu_wchar_t*           wstr;
    xu_wchar_t const*     wcstr;
    xu_atomic_t           a;
    xu_atomic_t           a64;
    xu_spinlock_t         lock;
    xu_stream_ref_t       stream;
    xu_async_stream_ref_t astream;

#ifdef xu_CONFIG_TYPE_HAVE_FLOAT
    xu_float_t  f;
    xu_double_t d;
#endif

} xu_value_t, *xu_value_ref_t;
