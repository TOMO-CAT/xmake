#include "xutil/utils/trace.h"
#include "xutil/libc/misc/time/time.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/platform/file.h"
#include "xutil/platform/impl/mutex.h"
#include "xutil/platform/mutex.h"
#include "xutil/platform/print.h"
#include "xutil/platform/thread.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */

// the trace line maxn
#ifdef XU_CONFIG_MICRO_ENABLE
#    define XU_TRACE_LINE_MAXN (256)
#else
#    ifdef __xu_small__
#        define XU_TRACE_LINE_MAXN (8192)
#    else
#        define XU_TRACE_LINE_MAXN (8192 << 1)
#    endif
#endif

/* *******************************************************
 *  globals
 * *******************************************************
 */

// the mode
static xu_size_t g_mode = XU_TRACE_MODE_PRINT;

#ifndef XU_CONFIG_MICRO_ENABLE
// the file
static xu_file_ref_t g_file = xu_null;

// the file is referenced?
static xu_bool_t g_bref = xu_false;
#endif

// the line
static xu_char_t g_line[XU_TRACE_LINE_MAXN];

// the lock
static xu_mutex_t     g_lock_mutex;
static xu_mutex_ref_t g_lock = xu_null;

/* *******************************************************
 *  implementation
 * *******************************************************
 */
xu_bool_t xu_trace_init()
{
    // init lock
    g_lock = xu_mutex_init_impl(&g_lock_mutex);
    return g_lock ? xu_true : xu_false;
}
xu_void_t xu_trace_exit()
{
    // sync trace
    xu_trace_sync();

    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // clear mode
    g_mode = XU_TRACE_MODE_PRINT;

    // clear file
#ifndef XU_CONFIG_MICRO_ENABLE
    if (g_file && !g_bref) xu_file_exit(g_file);
    g_file = xu_null;
    g_bref = xu_false;
#endif

    // leave
    if (g_lock) xu_mutex_leave(g_lock);

    // exit lock
    xu_mutex_exit_impl(&g_lock_mutex);
    g_lock = xu_null;
}
xu_size_t xu_trace_mode()
{
    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // the mode
    xu_size_t mode = g_mode;

    // leave
    if (g_lock) xu_mutex_leave(g_lock);

    // ok?
    return mode;
}
xu_bool_t xu_trace_mode_set(xu_size_t mode)
{
    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // set the mode
    g_mode = mode;

    // leave
    if (g_lock) xu_mutex_leave(g_lock);

    // ok
    return xu_true;
}
#ifndef XU_CONFIG_MICRO_ENABLE
xu_file_ref_t xu_trace_file()
{
    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // the file
    xu_file_ref_t file = g_file;

    // leave
    if (g_lock) xu_mutex_leave(g_lock);

    // ok?
    return file;
}
xu_bool_t xu_trace_file_set(xu_file_ref_t file)
{
    // check
    xu_check_return_val(file, xu_false);

    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // exit the previous file
    if (g_file && !g_bref) xu_file_exit(g_file);

    // set the file
    g_file = file;
    g_bref = xu_true;

    // leave
    if (g_lock) xu_mutex_leave(g_lock);

    // ok
    return xu_true;
}
xu_bool_t xu_trace_file_set_path(xu_char_t const* path, xu_bool_t bappend)
{
    // check
    xu_check_return_val(path, xu_false);

    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // exit the previous file
    if (g_file && !g_bref) xu_file_exit(g_file);

    // set the file
    g_file =
        xu_file_init(path, XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | (bappend ? XU_FILE_MODE_APPEND : XU_FILE_MODE_TRUNC));
    g_bref = xu_false;

    // ok?
    xu_bool_t ok = g_file ? xu_true : xu_false;

    // leave
    if (g_lock) xu_mutex_leave(g_lock);

    // ok?
    return ok;
}
#endif
xu_void_t xu_trace_done_with_args(xu_char_t const* prefix, xu_char_t const* module, xu_char_t const* format,
                                  xu_va_list_t args)
{
    // check
    xu_check_return(format);

    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // done
    do
    {
        // check
        xu_check_break(g_mode);

        // init
        xu_char_t* p = g_line;
        xu_char_t* e = g_line + sizeof(g_line);

        // print prefix to file
#ifndef XU_CONFIG_MICRO_ENABLE
        if ((g_mode & XU_TRACE_MODE_FILE) && g_file)
        {
            // print time to file
            xu_tm_t lt = {0};
            if (p < e && xu_localtime(xu_time(), &lt))
                p += xu_snprintf(p, e - p, "[%04ld-%02ld-%02ld %02ld:%02ld:%02ld]: ", lt.year, lt.month, lt.mday,
                                 lt.hour, lt.minute, lt.second);

            // print self to file
            if (p < e) p += xu_snprintf(p, e - p, "[%lx]: ", xu_thread_self());
        }
#endif

        // append prefix
        xu_char_t* b = p;
        if (prefix && p < e) p += xu_snprintf(p, e - p, "[%s]: ", prefix);

        // append module
        if (module && p < e) p += xu_snprintf(p, e - p, "[%s]: ", module);

        // append format
        if (p < e) p += xu_vsnprintf(p, e - p, format, args);

        // append end
        if (p < e) *p = '\0';
        e[-1] = '\0';

        // print it
        if (g_mode & XU_TRACE_MODE_PRINT) xu_print(b);

            // print it to file
#ifndef XU_CONFIG_MICRO_ENABLE
        if ((g_mode & XU_TRACE_MODE_FILE) && g_file)
        {
            // done
            xu_size_t size = p - g_line;
            xu_size_t writ = 0;
            while (writ < size)
            {
                // writ it
                xu_long_t real = xu_file_writ(g_file, (xu_byte_t const*)g_line + writ, size - writ);
                xu_check_break(real > 0);

                // save size
                writ += real;
            }
        }
#endif

    } while (0);

    // leave
    if (g_lock) xu_mutex_leave(g_lock);
}
xu_void_t xu_trace_done(xu_char_t const* prefix, xu_char_t const* module, xu_char_t const* format, ...)
{
    // check
    xu_check_return(format);

    // init args
    xu_va_list_t args;
    xu_va_start(args, format);

    // done trace
    xu_trace_done_with_args(prefix, module, format, args);

    // exit args
    xu_va_end(args);
}
xu_void_t xu_trace_tail(xu_char_t const* format, ...)
{
    // check
    xu_check_return(format);

    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // done
    do
    {
        // check
        xu_check_break(g_mode);

        // init
        xu_va_list_t l;
        xu_char_t*   p = g_line;
        xu_char_t*   e = g_line + sizeof(g_line);
        xu_va_start(l, format);

        // append format
        if (p < e) p += xu_vsnprintf(p, e - p, format, l);

        // append end
        if (p < e) *p = '\0';
        e[-1] = '\0';

        // print it
        if (g_mode & XU_TRACE_MODE_PRINT) xu_print(g_line);

            // print it to file
#ifndef XU_CONFIG_MICRO_ENABLE
        if ((g_mode & XU_TRACE_MODE_FILE) && g_file)
        {
            // done
            xu_size_t size = p - g_line;
            xu_size_t writ = 0;
            while (writ < size)
            {
                // writ it
                xu_long_t real = xu_file_writ(g_file, (xu_byte_t const*)g_line + writ, size - writ);
                xu_check_break(real > 0);

                // save size
                writ += real;
            }
        }
#endif

        // exit
        xu_va_end(l);

    } while (0);

    // leave
    if (g_lock) xu_mutex_leave(g_lock);
}
xu_void_t xu_trace_sync()
{
    // enter
    if (g_lock) xu_mutex_enter_without_profiler(g_lock);

    // sync it
    if (g_mode & XU_TRACE_MODE_PRINT) xu_print_sync();

        // sync it to file
#ifndef XU_CONFIG_MICRO_ENABLE
    if ((g_mode & XU_TRACE_MODE_FILE) && g_file) xu_file_sync(g_file);
#endif

    // leave
    if (g_lock) xu_mutex_leave(g_lock);
}
