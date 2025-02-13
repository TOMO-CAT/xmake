#include "stdio.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/platform/stdfile.h"

/* *******************************************************
 * implementation
 */

xu_int_t xu_getchar()
{
    xu_char_t ch;
    return xu_stdfile_getc(xu_stdfile_input(), &ch) ? (xu_int_t)ch : XU_EOF;
}
