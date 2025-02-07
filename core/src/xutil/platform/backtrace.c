#include "xutil/platform/backtrace.h"

#if defined(XU_CONFIG_OS_ANDROID)
#    include "xutil/platform/android/backtrace.c"
#else
#    include "xutil/platform/libc/backtrace.c"
#endif

/* *******************************************************
 * implementation
 */

xu_void_t xu_backtrace_dump(xu_char_t const* prefix, xu_pointer_t* frames, xu_size_t nframe)
{
    // check
    xu_check_return(nframe < 256);

    // the frames
    xu_pointer_t frames_data[256] = {0};
    if (!frames)
    {
        nframe = xu_backtrace_frames(frames_data, nframe, 2);
        frames = frames_data;
    }

    // dump frames
    if (frames && nframe)
    {
        // init symbols
        xu_handle_t symbols = xu_backtrace_symbols_init(frames, nframe);
        if (symbols)
        {
            // walk
            xu_size_t i = 0;
            for (i = 0; i < nframe; i++)
            {
#if XU_CPU_BIT64
                xu_trace_i("%s[%016p]: %s", prefix ? prefix : "", frames[i],
                           xu_backtrace_symbols_name(symbols, frames, nframe, i));
#else
                xu_trace_i("%s[%08p]: %s", prefix ? prefix : "", frames[i],
                           xu_backtrace_symbols_name(symbols, frames, nframe, i));
#endif
            }

            // exit symbols
            xu_backtrace_symbols_exit(symbols);
        }
        else
        {
            // walk
            xu_size_t i = 0;
            for (i = 0; i < nframe; i++)
            {
#if XU_CPU_BIT64
                xu_trace_i("%s[%016p]", prefix ? prefix : "", frames[i]);
#else
                xu_trace_i("%s[%08p]", prefix ? prefix : "", frames[i]);
#endif
            }
        }
    }
}
