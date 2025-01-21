#include "xutil/platform/impl/platform.h"
#include "xutil/platform/cpu.h"
#include "xutil/platform/impl/dns.h"
#include "xutil/platform/impl/socket.h"
#include "xutil/platform/impl/thread_local.h"

/* *******************************************************
 *  declaration
 * *******************************************************
 */
xu_bool_t xu_process_group_init();
xu_void_t xu_process_group_exit();

/* *******************************************************
 *  implementation
 * *******************************************************
 */
xu_bool_t xu_platform_init_env(xu_handle_t priv)
{
    // init the current platform environment
#if defined(XU_CONFIG_OS_ANDROID)
    if (!xu_android_init_env(priv)) return xu_false;
#endif

    // init socket environment
    if (!xu_socket_init_env()) return xu_false;

        // init dns environment
#ifndef XU_CONFIG_MICRO_ENABLE
    if (!xu_dns_init_env()) return xu_false;
#endif

        // init thread local environment
#ifndef XU_CONFIG_MICRO_ENABLE
    if (!xu_thread_local_init_env()) return xu_false;
#endif

        // init exception environment
#ifdef XU_CONFIG_EXCEPTION_ENABLE
    if (!xu_exception_init_env()) return xu_false;
#endif

        // init cpu count/cache
#ifndef XU_CONFIG_MICRO_ENABLE
    (xu_void_t) xu_cpu_count();
#endif

    // init the global process group
#ifndef XU_CONFIG_MICRO_ENABLE
    if (!xu_process_group_init()) return xu_false;
#endif

    // ok
    return xu_true;
}
xu_void_t xu_platform_exit_env()
{
    // exit all process in the process group
#ifndef XU_CONFIG_MICRO_ENABLE
    xu_process_group_exit();
#endif

    // exit exception environment
#ifdef XU_CONFIG_EXCEPTION_ENABLE
    xu_exception_exit_env();
#endif

    // exit thread local environment
#ifndef XU_CONFIG_MICRO_ENABLE
    xu_thread_local_exit_env();
#endif

    // exit dns environment
#ifndef XU_CONFIG_MICRO_ENABLE
    xu_dns_exit_env();
#endif

    // exit socket environment
    xu_socket_exit_env();

    // exit the current platform environment
#if defined(XU_CONFIG_OS_ANDROID)
    xu_android_exit_env();
#endif
}
