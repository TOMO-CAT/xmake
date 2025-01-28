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
    XU_UT_MAIN_ITEM(algorithm_sort),
    XU_UT_MAIN_ITEM(memory_buffer),
    XU_UT_MAIN_ITEM(memory_check),
    XU_UT_MAIN_ITEM(utils_trace),
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
        xu_trace_i("    xmake run xutil-ut stream http://www.xxxxx.com /tmp/a");
        xu_trace_i("");

        // walk name
        xu_size_t i = 0;
        xu_size_t n = xu_arrayn(g_ut);
        for (i = 0; i < n; i++)
            xu_trace_i("testname: %s", g_ut[i].name);
    }

    // exit tbox
    xutil_exit();

    // ok?
    return ok;
}
