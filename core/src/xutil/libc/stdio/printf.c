#include "xutil/libc/stdio/stdio.h"
#include "xutil/platform/print.h"

/* *******************************************************
 *  implementation
 */

xu_long_t xu_printf(xu_char_t const* format, ...)
{
    // check
    xu_check_return_val(format, 0);

    // format line
    xu_long_t size       = 0;
    xu_char_t line[8192] = {0};
    xu_vsnprintf_format(line, sizeof(line) - 1, format, &size);

    // print it
    xu_print(line);

    // ok?
    return size;
}
