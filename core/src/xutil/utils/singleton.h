#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  includes
 */
// #include "xutil/macros/type.h"
#include "xutil/platform/atomic.h"

/* *******************************************************
 *  types
 */
// the singleton type enum
typedef enum __xu_singleton_type_e
{
    /// the default allocator
    XU_SINGLETON_TYPE_DEFAULT_ALLOCATOR = 0

    /// the static allocator
    ,
    XU_SINGLETON_TYPE_STATIC_ALLOCATOR = 1

#ifdef XU_CONFIG_MICRO_ENABLE

    /// the user defined type
    ,
    XU_SINGLETON_TYPE_USER = 2

#else

    /// the lock profiler type
    ,
    XU_SINGLETON_TYPE_LOCK_PROFILER = 2

    /// the ifaddrs type
    ,
    XU_SINGLETON_TYPE_IFADDRS = 3

    /// the timer type
    ,
    XU_SINGLETON_TYPE_TIMER = 4

    /// the ltimer type
    ,
    XU_SINGLETON_TYPE_LTIMER = 5

    /// the aicp type, TODO deprecated
    ,
    XU_SINGLETON_TYPE_AICP = 6

    /// the openssl library type
    ,
    XU_SINGLETON_TYPE_LIBRARY_OPENSSL = 7

    /// the mysql library type
    ,
    XU_SINGLETON_TYPE_LIBRARY_MYSQL = 8

    /// the sqlite3 library type
    ,
    XU_SINGLETON_TYPE_LIBRARY_SQLITE3 = 9

    /// the thread pool type
    ,
    XU_SINGLETON_TYPE_THREAD_POOL = 10

    /// the transfer pool type
    ,
    XU_SINGLETON_TYPE_TRANSFER_POOL = 11

    /// the cookies type
    ,
    XU_SINGLETON_TYPE_COOKIES = 12

    /// the stdfile(stdin) type
    ,
    XU_SINGLETON_TYPE_STDFILE_STDIN = 13

    /// the stdfile(stdout) type
    ,
    XU_SINGLETON_TYPE_STDFILE_STDOUT = 14

    /// the stdfile(stderr) type
    ,
    XU_SINGLETON_TYPE_STDFILE_STDERR = 15

    /// the user defined type
    ,
    XU_SINGLETON_TYPE_USER = 16

#endif

/// the max count of the singleton type
#if defined(XU_CONFIG_MICRO_ENABLE)
    ,
    XU_SINGLETON_TYPE_MAXN = XU_SINGLETON_TYPE_USER + 2
#elif defined(__xu_small__)
    ,
    XU_SINGLETON_TYPE_MAXN = XU_SINGLETON_TYPE_USER + 8
#else
    ,
    XU_SINGLETON_TYPE_MAXN = XU_SINGLETON_TYPE_USER + 64
#endif

} xu_singleton_type_e;

// the singleton init func type
typedef xu_handle_t (*xu_singleton_init_func_t)(xu_cpointer_t* ppriv);

// the singleton exit func type
typedef xu_void_t (*xu_singleton_exit_func_t)(xu_handle_t instance, xu_cpointer_t priv);

// the singleton kill func type
typedef xu_void_t (*xu_singleton_kill_func_t)(xu_handle_t instance, xu_cpointer_t priv);

// the singleton static init func type
typedef xu_bool_t (*xu_singleton_static_init_func_t)(xu_handle_t instance, xu_cpointer_t priv);

/* *******************************************************
 *  interfaces
 */

/*! init singleton
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_singleton_init(xu_noarg_t);

/*! kill singleton
 */
xu_void_t xu_singleton_kill(xu_noarg_t);

/*! exit singleton
 */
xu_void_t xu_singleton_exit(xu_noarg_t);

/*! the singleton instance
 *
 * @param type      the singleton type
 * @param init      the singleton init func
 * @param exit      the singleton exit func
 * @param kill      the singleton kill func
 * @param priv      the private data
 *
 * @return          the singleton instance handle
 */
xu_handle_t xu_singleton_instance(xu_size_t type, xu_singleton_init_func_t init, xu_singleton_exit_func_t exit,
                                  xu_singleton_kill_func_t kill, xu_cpointer_t priv);

/*! the singleton static instance
 *
 * @code
    static xu_bool_t xu_xxxx_instance_init(xu_handle_t instance, xu_cpointer_t priv)
    {
        // init
        // ...

        // ok
        return xu_true;
    }
    xu_xxxx_t* xu_xxxx()
    {
        // init
        static xu_atomic32_t    s_binited = 0;
        static xu_xxxx_t        s_xxxx = {0};

        // init the static instance
        xu_bool_t ok = xu_singleton_static_init(&s_binited, &s_xxxx, xu_xxxx_instance_init, xu_null);
        xu_assert(ok);

        // ok
        return &s_xxxx;
    }
 * @endcode
 *
 * @param binited   the singleton static instance is inited?
 * @param instance  the singleton static instance
 * @param init      the singleton static init func
 * @param priv      the private data
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_singleton_static_init(xu_atomic32_t* binited, xu_handle_t instance, xu_singleton_static_init_func_t init,
                                   xu_cpointer_t priv);
