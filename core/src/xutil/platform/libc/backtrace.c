#include "xutil/platform/backtrace.h"
#include "xutil/libc/string/string.h"
#include "xutil/utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(XU_CONFIG_LIBC_HAVE_BACKTRACE)
#    include <execinfo.h>
#endif

/* *******************************************************
 * backtrace implementation
 */

#if defined(XU_CONFIG_LIBC_HAVE_BACKTRACE)
xu_size_t xu_backtrace_frames(xu_pointer_t* frames, xu_size_t nframe, xu_size_t nskip)
{
    // note: cannot use assert
    xu_check_return_val(frames && nframe, 0);

    // skip some frames?
    if (nskip)
    {
        // init temp frames
        xu_pointer_t temp[256] = {0};
        xu_check_return_val(nframe + nskip < 256, 0);

        // done backtrace
        xu_size_t size = backtrace(temp, nframe + nskip);
        xu_check_return_val(nskip < size, 0);

        // update nframe
        nframe = xu_min(nframe, size - nskip);

        // save to frames
        xu_memcpy_(frames, temp + nskip, nframe * sizeof(xu_pointer_t));
    }
    // backtrace
    else
        nframe = backtrace(frames, nframe);

    // ok?
    return nframe;
}
#else
xu_size_t xu_backtrace_frames(xu_pointer_t* frames, xu_size_t nframe, xu_size_t nskip)
{
    xu_trace_noimpl();
    return 0;
}
#endif

/* *******************************************************
 * symbols implementation
 */

#if defined(XU_CONFIG_LIBC_HAVE_BACKTRACE)
xu_handle_t xu_backtrace_symbols_init(xu_pointer_t* frames, xu_size_t nframe)
{
    xu_check_return_val(frames && nframe, xu_null);
    return (xu_handle_t)backtrace_symbols(frames, nframe);
}
xu_char_t const* xu_backtrace_symbols_name(xu_handle_t symbols, xu_pointer_t* frames, xu_size_t nframe,
                                           xu_size_t iframe)
{
    xu_check_return_val(symbols && frames && nframe && iframe < nframe, xu_null);
    return ((xu_char_t const**)symbols)[iframe];
}
xu_void_t xu_backtrace_symbols_exit(xu_handle_t symbols)
{
    if (symbols) free(symbols);
}
#elif 0
xu_handle_t xu_backtrace_symbols_init(xu_pointer_t* frames, xu_size_t nframe)
{
    // check
    xu_check_return_val(frames && nframe, xu_null);

    // init symbols
    return malloc(8192);
}
xu_char_t const* xu_backtrace_symbols_name(xu_handle_t symbols, xu_pointer_t* frames, xu_size_t nframe,
                                           xu_size_t iframe)
{
    // check
    xu_check_return_val(symbols && frames && nframe && iframe < nframe, xu_null);

    // the frame address
    xu_pointer_t frame = frames[iframe];
    xu_check_return_val(frame, xu_null);

    // the frame dlinfo
    Dl_info dlinfo = {0};
    if (!dladdr(frame, &dlinfo)) return xu_null;

    // format
    xu_long_t size = 0;
    xu_size_t maxn = 8192;
    if (dlinfo.dli_fname) size = xu_snprintf((xu_char_t*)symbols, maxn, "%s(", dlinfo.dli_fname);
    if (dlinfo.dli_sname && size >= 0)
        size += xu_snprintf((xu_char_t*)symbols + size, maxn - size, "%s", dlinfo.dli_sname);
    if (dlinfo.dli_sname && frame >= dlinfo.dli_saddr && size >= 0)
        size += xu_snprintf((xu_char_t*)symbols + size, maxn - size, "+%#lx", (xu_size_t)(frame - dlinfo.dli_saddr));
    if (size >= 0) size += xu_snprintf((xu_char_t*)symbols + size, maxn - size, ") [%p]", frame);
    if (size >= 0) ((xu_char_t*)symbols)[size] = '\0';

    // ok
    return symbols;
}
xu_void_t xu_backtrace_symbols_exit(xu_handle_t symbols)
{
    // exit symbols
    if (symbols) free(symbols);
}
#else
xu_handle_t xu_backtrace_symbols_init(xu_pointer_t* frames, xu_size_t nframe)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_char_t const* xu_backtrace_symbols_name(xu_handle_t symbols, xu_pointer_t* frames, xu_size_t nframe,
                                           xu_size_t iframe)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_void_t xu_backtrace_symbols_exit(xu_handle_t symbols) { xu_trace_noimpl(); }
#endif
