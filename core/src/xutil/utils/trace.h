#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  includes
 */
#include "xutil/libc/misc/stdarg.h"
#include "xutil/macros/type.h"

/* *******************************************************
 *  the trace prefix
 */
#ifndef __xu_prefix__
#    define __xu_prefix__ xu_null
#endif

/* *******************************************************
 *  the trace module name
 */
#ifndef XU_TRACE_MODULE_NAME
#    define XU_TRACE_MODULE_NAME xu_null
#endif

/* *******************************************************
 *  the trace module debug
 */
#ifndef XU_TRACE_MODULE_DEBUG
#    define XU_TRACE_MODULE_DEBUG (1)
#endif

/* *******************************************************
 *  trace prefix
 */
#if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#    define xu_trace_p(prefix, fmt, arg...)                                                                            \
        do                                                                                                             \
        {                                                                                                              \
            xu_trace_done(prefix, XU_TRACE_MODULE_NAME, fmt __xu_newline__, ##arg);                                    \
        } while (0)
#    define xu_tracef_p(prefix, fmt, arg...)                                                                           \
        do                                                                                                             \
        {                                                                                                              \
            xu_trace_done(prefix, XU_TRACE_MODULE_NAME, fmt, ##arg);                                                   \
        } while (0)
#    ifdef __xu_debug__
#        define xu_trace_error_p(prefix, fmt, arg...)                                                                  \
            do                                                                                                         \
            {                                                                                                          \
                xu_trace_done(prefix, XU_TRACE_MODULE_NAME, "[error]: " fmt " at %s(): %d, %s" __xu_newline__, ##arg,  \
                              __xu_func__, __xu_line__, __xu_file__);                                                  \
                xu_trace_sync();                                                                                       \
            } while (0)
#        define xu_trace_assert_p(prefix, fmt, arg...)                                                                 \
            do                                                                                                         \
            {                                                                                                          \
                xu_trace_done(prefix, XU_TRACE_MODULE_NAME, "[assert]: " fmt " at %s(): %d, %s" __xu_newline__, ##arg, \
                              __xu_func__, __xu_line__, __xu_file__);                                                  \
                xu_trace_sync();                                                                                       \
            } while (0)
#        define xu_trace_warning_p(prefix, fmt, arg...)                                                                \
            do                                                                                                         \
            {                                                                                                          \
                xu_trace_done(prefix, XU_TRACE_MODULE_NAME, "[warning]: " fmt " at %s(): %d, %s" __xu_newline__,       \
                              ##arg, __xu_func__, __xu_line__, __xu_file__);                                           \
                xu_trace_sync();                                                                                       \
            } while (0)
#        define xu_tracef_error_p(prefix, fmt, arg...)                                                                 \
            do                                                                                                         \
            {                                                                                                          \
                xu_trace_done(prefix, XU_TRACE_MODULE_NAME, "[error]: " fmt " at %s(): %d, %s", ##arg, __xu_func__,    \
                              __xu_line__, __xu_file__);                                                               \
                xu_trace_sync();                                                                                       \
            } while (0)
#        define xu_tracef_assert_p(prefix, fmt, arg...)                                                                \
            do                                                                                                         \
            {                                                                                                          \
                xu_trace_done(prefix, XU_TRACE_MODULE_NAME, "[assert]: " fmt " at %s(): %d, %s", ##arg, __xu_func__,   \
                              __xu_line__, __xu_file__);                                                               \
                xu_trace_sync();                                                                                       \
            } while (0)
#        define xu_tracef_warning_p(prefix, fmt, arg...)                                                               \
            do                                                                                                         \
            {                                                                                                          \
                xu_trace_done(prefix, XU_TRACE_MODULE_NAME, "[warning]: " fmt " at %s(): %d, %s", ##arg, __xu_func__,  \
                              __xu_line__, __xu_file__);                                                               \
                xu_trace_sync();                                                                                       \
            } while (0)
#    else
#        define xu_trace_error_p(prefix, fmt, arg...)
#        define xu_trace_assert_p(prefix, fmt, arg...)
#        define xu_trace_warning_p(prefix, fmt, arg...)
#        define xu_tracef_error_p(prefix, fmt, arg...)
#        define xu_tracef_assert_p(prefix, fmt, arg...)
#        define xu_tracef_warning_p(prefix, fmt, arg...)
#    endif
#else
#    define xu_trace_p
#    define xu_trace_error_p
#    define xu_trace_assert_p
#    define xu_trace_warning_p
#    define xu_tracef_p
#    define xu_tracef_error_p
#    define xu_tracef_assert_p
#    define xu_tracef_warning_p
#endif

/* trace
 *
 * at file xxxx.c:
 *
 * // macros
 * #define XU_TRACE_MODULE_NAME     "module"
 * #define XU_TRACE_MODULE_DEBUG    (1)
 *
 * // includes
 * #include "xutil.h"
 *
 * // trace
 * xu_trace_d("trace debug");
 * xu_trace_i("trace info");
 * xu_trace_e("trace error");
 * xu_trace_w("trace warning");
 *
 * // output for debug
 * "[prefix]: [module]: trace debug"
 * "[prefix]: [module]: trace info"
 * "[prefix]: [module]: [error]: trace error" at func: xxx, line: xxx, file: xxx
 * "[prefix]: [module]: [warning]: trace warning" at func: xxx, line: xxx, file: xxx
 *
 * // output for release or XU_TRACE_MODULE_DEBUG == 0
 * "[prefix]: [module]: trace info"
 * "[prefix]: [module]: [error]: trace error" at func: xxx, line: xxx, file: xxx
 * "[prefix]: [module]: [warning]: trace warning" at func: xxx, line: xxx, file: xxx
 *
 * note: [module]: will be not output if XU_TRACE_MODULE_NAME is not defined
 *
 */
#if XU_TRACE_MODULE_DEBUG && defined(__xu_debug__)
#    if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#        define XU_TRACE_DEBUG
#        define xu_trace_d(fmt, arg...) xu_trace_p(__xu_prefix__, fmt, ##arg)
#        define xu_tracef_d(fmt, arg...) xu_tracef_p(__xu_prefix__, fmt, ##arg)
#        define xu_tracet_d(fmt, arg...) xu_trace_tail(fmt, ##arg)
#    else
#        define xu_trace_d
#        define xu_tracef_d
#        define xu_tracet_d
#    endif
#else
#    if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#        define xu_trace_d(fmt, ...)
#        define xu_tracef_d(fmt, ...)
#        define xu_tracet_d(fmt, ...)
#    else
#        define xu_trace_d
#        define xu_tracef_d
#        define xu_tracet_d
#    endif
#endif

/**
 * 打印 info, debug 和 warning 信息, 其中 debug 只再调试模式下起作用:
 * xu_trace_i("hello world");
 * xu_trace_d("hello world");
 * xu_trace_w("hello world");
 */
#if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#    define xu_trace_i(fmt, arg...) xu_trace_p(__xu_prefix__, fmt, ##arg)
#    define xu_trace_e(fmt, arg...) xu_trace_error_p(__xu_prefix__, fmt, ##arg)
#    define xu_trace_a(fmt, arg...) xu_trace_assert_p(__xu_prefix__, fmt, ##arg)
#    define xu_trace_w(fmt, arg...) xu_trace_warning_p(__xu_prefix__, fmt, ##arg)
#    define xu_tracef_i(fmt, arg...) xu_tracef_p(__xu_prefix__, fmt, ##arg)
#    define xu_tracef_e(fmt, arg...) xu_tracef_error_p(__xu_prefix__, fmt, ##arg)
#    define xu_tracef_a(fmt, arg...) xu_tracef_assert_p(__xu_prefix__, fmt, ##arg)
#    define xu_tracef_w(fmt, arg...) xu_tracef_warning_p(__xu_prefix__, fmt, ##arg)
#    define xu_tracet_i(fmt, arg...) xu_trace_tail(fmt, ##arg)
#    define xu_tracet_e(fmt, arg...) xu_trace_tail(fmt, ##arg)
#    define xu_tracet_a(fmt, arg...) xu_trace_tail(fmt, ##arg)
#    define xu_tracet_w(fmt, arg...) xu_trace_tail(fmt, ##arg)
#else
#    define xu_trace_i
#    define xu_trace_e
#    define xu_trace_a
#    define xu_trace_w
#    define xu_tracef_i
#    define xu_tracef_e
#    define xu_tracef_a
#    define xu_tracef_w
#    define xu_tracet_i
#    define xu_tracet_e
#    define xu_tracet_a
#    define xu_tracet_w
#endif

/* *******************************************************
 *  trace once
 */
#if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#    define xu_trace1_d(fmt, arg...)                                                                                   \
        do                                                                                                             \
        {                                                                                                              \
            static xu_bool_t __trace_once = xu_false;                                                                  \
            if (!__trace_once)                                                                                         \
            {                                                                                                          \
                xu_trace_d(fmt, ##arg);                                                                                \
                __trace_once = xu_true;                                                                                \
            }                                                                                                          \
        } while (0)
#    define xu_trace1_i(fmt, arg...)                                                                                   \
        do                                                                                                             \
        {                                                                                                              \
            static xu_bool_t __trace_once = xu_false;                                                                  \
            if (!__trace_once)                                                                                         \
            {                                                                                                          \
                xu_trace_i(fmt, ##arg);                                                                                \
                __trace_once = xu_true;                                                                                \
            }                                                                                                          \
        } while (0)
#    define xu_trace1_e(fmt, arg...)                                                                                   \
        do                                                                                                             \
        {                                                                                                              \
            static xu_bool_t __trace_once = xu_false;                                                                  \
            if (!__trace_once)                                                                                         \
            {                                                                                                          \
                xu_trace_e(fmt, ##arg);                                                                                \
                __trace_once = xu_true;                                                                                \
            }                                                                                                          \
        } while (0)
#    define xu_trace1_a(fmt, arg...)                                                                                   \
        do                                                                                                             \
        {                                                                                                              \
            static xu_bool_t __trace_once = xu_false;                                                                  \
            if (!__trace_once)                                                                                         \
            {                                                                                                          \
                xu_trace_a(fmt, ##arg);                                                                                \
                __trace_once = xu_true;                                                                                \
            }                                                                                                          \
        } while (0)
#    define xu_trace1_w(fmt, arg...)                                                                                   \
        do                                                                                                             \
        {                                                                                                              \
            static xu_bool_t __trace_once = xu_false;                                                                  \
            if (!__trace_once)                                                                                         \
            {                                                                                                          \
                xu_trace_w(fmt, ##arg);                                                                                \
                __trace_once = xu_true;                                                                                \
            }                                                                                                          \
        } while (0)
#else
#    define xu_trace1_i
#    define xu_trace1_w
#endif

/* *******************************************************
 *  trace more info?
 */
#if !defined(__xu_debug__) && !defined(XU_CONFIG_INFO_TRACE_MORE)
#    undef XU_TRACE_DEBUG
#    undef XU_TRACE_MODULE_DEBUG
#    undef xu_trace_d
#    undef xu_trace_e
#    undef xu_trace_a
#    undef xu_trace_w
#    undef xu_tracef_d
#    undef xu_tracef_e
#    undef xu_tracef_a
#    undef xu_tracef_w
#    undef xu_tracet_d
#    undef xu_tracet_e
#    undef xu_tracet_a
#    undef xu_tracet_w
#    undef xu_trace1_d
#    undef xu_trace1_e
#    undef xu_trace1_a
#    undef xu_trace1_w
#    define XU_TRACE_MODULE_DEBUG (0)
#    if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#        define xu_trace_d(fmt, arg...)
#        define xu_trace_e(fmt, arg...)
#        define xu_trace_a(fmt, arg...)
#        define xu_trace_w(fmt, arg...)
#        define xu_tracef_d(fmt, arg...)
#        define xu_tracef_e(fmt, arg...)
#        define xu_tracef_a(fmt, arg...)
#        define xu_tracef_w(fmt, arg...)
#        define xu_tracet_d(fmt, arg...)
#        define xu_tracet_e(fmt, arg...)
#        define xu_tracet_a(fmt, arg...)
#        define xu_tracet_w(fmt, arg...)
#        define xu_trace1_d(fmt, arg...)
#        define xu_trace1_e(fmt, arg...)
#        define xu_trace1_a(fmt, arg...)
#        define xu_trace1_w(fmt, arg...)
#    else
#        define xu_trace_d
#        define xu_trace_e
#        define xu_trace_a
#        define xu_trace_w
#        define xu_tracef_d
#        define xu_tracef_e
#        define xu_tracef_a
#        define xu_tracef_w
#        define xu_tracet_d
#        define xu_tracet_e
#        define xu_tracet_a
#        define xu_tracet_w
#        define xu_trace1_d
#        define xu_trace1_e
#        define xu_trace1_a
#        define xu_trace1_w
#    endif
#endif

/* *******************************************************
 *  noimpl
 */
#define xu_trace_noimpl() xu_trace1_w("noimpl")

/* *******************************************************
 *  nosafe
 */
#define xu_trace_nosafe() xu_trace1_w("nosafe")

/* *******************************************************
 *  deprecated
 */
#define xu_trace_deprecated() xu_trace1_w("deprecated")

/* *******************************************************
 *  declaration
 */
xu_void_t xu_trace_sync(xu_noarg_t);
xu_void_t xu_trace_done(xu_char_t const* prefix, xu_char_t const* module, xu_char_t const* format, ...);
xu_void_t xu_trace_tail(xu_char_t const* format, ...);

// the trace mode enum
typedef enum __xu_trace_mode_e
{
    XU_TRACE_MODE_NONE  = 0,
    XU_TRACE_MODE_FILE  = 1,
    XU_TRACE_MODE_PRINT = 2
} xu_trace_mode_e;

/*! init trace
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_trace_init(xu_noarg_t);

/*! exit trace
 */
xu_void_t xu_trace_exit(xu_noarg_t);

/*! the trace mode
 *
 * @return          the trace mode
 */
xu_size_t xu_trace_mode(xu_noarg_t);

/*! set the trace mode
 *
 * @param mode      the trace mode
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_trace_mode_set(xu_size_t mode);

/*! the trace file
 *
 * @return          the trace file handle
 */
xu_file_ref_t xu_trace_file(xu_noarg_t);

/*! set the trace file
 *
 * @param file      the trace file handle
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_trace_file_set(xu_file_ref_t file);

/*! set the trace file path
 *
 * @param path      the trace file path
 * @param bappend   is appended?
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_trace_file_set_path(xu_char_t const* path, xu_bool_t bappend);

/*! done trace with arguments
 *
 * @param prefix    the trace prefix
 * @param module    the trace module
 * @param format    the trace format
 * @param args      the trace arguments
 */
xu_void_t xu_trace_done_with_args(xu_char_t const* prefix, xu_char_t const* module, xu_char_t const* format,
                                  xu_va_list_t args);

/*! done trace
 *
 * @param prefix    the trace prefix
 * @param module    the trace module
 * @param format    the trace format
 */
xu_void_t xu_trace_done(xu_char_t const* prefix, xu_char_t const* module, xu_char_t const* format, ...);

/*! done trace tail
 *
 * @param format    the trace format
 */
xu_void_t xu_trace_tail(xu_char_t const* format, ...);

/*! sync trace
 */
xu_void_t xu_trace_sync(xu_noarg_t);
