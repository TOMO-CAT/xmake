#include "xutil/libc/stdio/stdio.h"

xu_long_t xu_snprintf(xu_char_t* s, xu_size_t n, xu_char_t const* fmt, ...)
{
    xu_long_t ret = 0;
    xu_vsnprintf_format(s, n, fmt, &ret);
    return ret;
}
