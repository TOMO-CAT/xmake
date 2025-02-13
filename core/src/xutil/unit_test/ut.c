#include "xutil/unit_test/ut.h"
#include "xutil/libc/string/string.h"
#include "xutil/utils/trace.h"
#include "xutil/utils/utils.h"
#include "xutil/xutil.h"

/* *******************************************************
 *  macros
 */
#define XU_UT_MAIN_ITEM(name)                                                                                          \
    {                                                                                                                  \
#        name, xu_ut_##name##_main                                                                                     \
    }

/* *******************************************************
 *  types
 */
// the ut type
typedef struct __xu_ut_t
{
    // the ut name
    xu_char_t const* name;

    // the ut main
    xu_int_t (*main)(xu_int_t argc, xu_char_t** argv);

} xu_ut_t;

/* *******************************************************
 *  globals
 */

// the unit-tests
static xu_ut_t g_ut[] = {
    // algorithm
    XU_UT_MAIN_ITEM(algorithm_find),
    XU_UT_MAIN_ITEM(algorithm_sort),

    // container
    XU_UT_MAIN_ITEM(container_bloom_filter),
    XU_UT_MAIN_ITEM(container_circle_queue),
    XU_UT_MAIN_ITEM(container_hash_map),
    XU_UT_MAIN_ITEM(container_hash_set),
    XU_UT_MAIN_ITEM(container_heap),
    XU_UT_MAIN_ITEM(container_list_entry),
    XU_UT_MAIN_ITEM(container_list),
    XU_UT_MAIN_ITEM(container_queue),
    XU_UT_MAIN_ITEM(container_single_list_entry),
    XU_UT_MAIN_ITEM(container_single_list),
    XU_UT_MAIN_ITEM(container_stack),
    XU_UT_MAIN_ITEM(container_vector),

    // hash
    XU_UT_MAIN_ITEM(hash_adler32),
    XU_UT_MAIN_ITEM(hash_benchmark),
    XU_UT_MAIN_ITEM(hash_crc8),
    XU_UT_MAIN_ITEM(hash_crc16),
    XU_UT_MAIN_ITEM(hash_crc32),
    XU_UT_MAIN_ITEM(hash_djb2),
    XU_UT_MAIN_ITEM(hash_fnv32),
    XU_UT_MAIN_ITEM(hash_fnv64),
    XU_UT_MAIN_ITEM(hash_md5),
    XU_UT_MAIN_ITEM(hash_sdbm),
    XU_UT_MAIN_ITEM(hash_sha),
    XU_UT_MAIN_ITEM(hash_uuid),

    // libc
    XU_UT_MAIN_ITEM(libc_stdlib),
    XU_UT_MAIN_ITEM(libc_string),
    XU_UT_MAIN_ITEM(libc_time),

    // libm
    XU_UT_MAIN_ITEM(libm_double),
    XU_UT_MAIN_ITEM(libm_float),
    XU_UT_MAIN_ITEM(libm_integer),

    // math
    XU_UT_MAIN_ITEM(math_fixed),
    XU_UT_MAIN_ITEM(math_random),

    // memory
    XU_UT_MAIN_ITEM(memory_impl_static_fixed_pool),
    XU_UT_MAIN_ITEM(memory_buffer),
    XU_UT_MAIN_ITEM(memory_check),
    XU_UT_MAIN_ITEM(memory_default_allocator),
    XU_UT_MAIN_ITEM(memory_fixed_pool),
    XU_UT_MAIN_ITEM(memory_large_allocator),
    XU_UT_MAIN_ITEM(memory_memops),
    XU_UT_MAIN_ITEM(memory_queue_buffer),
    XU_UT_MAIN_ITEM(memory_small_allocator),
    XU_UT_MAIN_ITEM(memory_static_buffer),
    XU_UT_MAIN_ITEM(memory_string_pool),

    // network
    XU_UT_MAIN_ITEM(network_impl_date),
    XU_UT_MAIN_ITEM(network_cookies),
    XU_UT_MAIN_ITEM(network_dns),
    XU_UT_MAIN_ITEM(network_http),
    XU_UT_MAIN_ITEM(network_hwaddr),
    XU_UT_MAIN_ITEM(network_ipaddr),
    XU_UT_MAIN_ITEM(network_ipv4),
    XU_UT_MAIN_ITEM(network_ipv6),
    XU_UT_MAIN_ITEM(network_ping),
    XU_UT_MAIN_ITEM(network_unix_echo_client),
    XU_UT_MAIN_ITEM(network_unix_echo_server),
    XU_UT_MAIN_ITEM(network_unixaddr),
    XU_UT_MAIN_ITEM(network_url),
    XU_UT_MAIN_ITEM(network_whois),

    // other
    XU_UT_MAIN_ITEM(other_charset),

    // platform
    XU_UT_MAIN_ITEM(platform_addrinfo),
    XU_UT_MAIN_ITEM(platform_atomic),
    XU_UT_MAIN_ITEM(platform_atomic32),
    XU_UT_MAIN_ITEM(platform_atomic64),
    XU_UT_MAIN_ITEM(platform_backtrace),
    XU_UT_MAIN_ITEM(platform_cache_time),
    XU_UT_MAIN_ITEM(platform_directory),
    XU_UT_MAIN_ITEM(platform_environment),
    XU_UT_MAIN_ITEM(platform_file),
    XU_UT_MAIN_ITEM(platform_filelock),
    XU_UT_MAIN_ITEM(platform_fwatcher),
    XU_UT_MAIN_ITEM(platform_hostname),
    XU_UT_MAIN_ITEM(platform_ifaddrs),
    XU_UT_MAIN_ITEM(platform_lock),
    XU_UT_MAIN_ITEM(platform_named_pipe),
    XU_UT_MAIN_ITEM(platform_path),
    XU_UT_MAIN_ITEM(platform_pipe_pair),
    XU_UT_MAIN_ITEM(platform_process),
    XU_UT_MAIN_ITEM(platform_sched),
    XU_UT_MAIN_ITEM(platform_semaphore),
    XU_UT_MAIN_ITEM(platform_stdfile),
    XU_UT_MAIN_ITEM(platform_thread_local),
    XU_UT_MAIN_ITEM(platform_thread),
    XU_UT_MAIN_ITEM(platform_utils),

    // stream
    XU_UT_MAIN_ITEM(stream_cache),
    XU_UT_MAIN_ITEM(stream_charset),
    XU_UT_MAIN_ITEM(stream_null),
    XU_UT_MAIN_ITEM(stream),

    // string
    XU_UT_MAIN_ITEM(string_static_string),
    XU_UT_MAIN_ITEM(string_string),

    // utils
    XU_UT_MAIN_ITEM(utils_base32),
    XU_UT_MAIN_ITEM(utils_base64),
    XU_UT_MAIN_ITEM(utils_bits),
    XU_UT_MAIN_ITEM(utils_dump),
    XU_UT_MAIN_ITEM(utils_trace),
    XU_UT_MAIN_ITEM(utils_url),
};

/* *******************************************************
 *  main
 */
xu_int_t main(xu_int_t argc, xu_char_t** argv)
{
#if (defined(__xu_valgrind__) && defined(XU_CONFIG_VALGRIND_HAVE_VALGRIND_STACK_REGISTER)) ||                          \
    defined(__xu_sanitize_address__) || defined(__xu_sanitize_thread__)
    if (!xutil_init(xu_null, xu_native_allocator())) return -1;
#else
    if (!xutil_init(xu_null, xu_null)) return -1;
#endif

    // find the main func from the first argument
    xu_int_t         ok   = 0;
    xu_char_t const* name = xu_null;
    if (argc > 1 && argv[1])
    {
        xu_size_t i = 0;
        xu_size_t n = xu_arrayn(g_ut);
        for (i = 0; i < n; i++)
        {
            // find it?
            if (g_ut[i].name && !xu_stricmp(g_ut[i].name, argv[1]))
            {
                // save name
                name = g_ut[i].name;

                // done main
                ok = g_ut[i].main(argc - 1, argv + 1);
                break;
            }
        }
    }

    // no this ut? help it
    if (!name)
    {
        xu_trace_i("======================================================================");
        xu_trace_i("Usages: xmake run xutil-ut [testname] arguments ...");
        xu_trace_i("");
        xu_trace_i(".e.g");
        xu_trace_i("    xmake run xutil-ut stream http://www.baidu.com /tmp/a");
        xu_trace_i("");

        // walk name
        xu_size_t i = 0;
        xu_size_t n = xu_arrayn(g_ut);
        for (i = 0; i < n; i++)
            xu_trace_i("testname: %s", g_ut[i].name);
    }

    // exit xutil
    xutil_exit();

    // ok?
    return ok;
}
