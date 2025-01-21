#include "xutil/platform/thread.h"

#if defined(XU_CONFIG_OS_IOS)
#    include <os/log.h>
#    include <stdio.h>
#    include <unistd.h>

xu_void_t xu_print(xu_char_t const* string)
{
    // check
    xu_check_return(string);

    // print to the ios device log
    os_log(OS_LOG_DEFAULT, "[%08x]: %s", (xu_uint32_t)xu_thread_self(), string);

    // print to the stdout
    fputs(string, stdout);
}
xu_void_t xu_printl(xu_char_t const* string)
{
    // check
    xu_check_return(string);

    // print to the ios device log
    os_log(OS_LOG_DEFAULT, "[%08x]: %s", (xu_uint32_t)xu_thread_self(), string);

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
#endif
