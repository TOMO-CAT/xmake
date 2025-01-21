#include "xutil/libc/impl/libc.h"

/* *******************************************************
 *  implementation
 * *******************************************************
 */

xu_bool_t xu_libc_init_env() { return xu_true; }
xu_void_t xu_libc_exit_env()
{
#ifndef XU_CONFIG_MICRO_ENABLE
    // exit the printf format
    xu_printf_object_exit();
#endif
}
