#include "xutil/platform/print.h"

#include <stdio.h>
#include <unistd.h>

xu_void_t xu_print(xu_char_t const* string)
{
    // check
    xu_check_return(string);

    // print to the stdout
    fputs(string, stdout);
}
xu_void_t xu_printl(xu_char_t const* string)
{
    // check
    xu_check_return(string);

    // print string to the stdout
    fputs(string, stdout);

    // print newline to the stdout
    fputs(__xu_newline__, stdout);
}
xu_void_t xu_print_sync()
{
    // flush the stdout
    fflush(stdout);
}
