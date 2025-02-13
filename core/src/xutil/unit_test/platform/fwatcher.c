#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_bool_t         g_stop     = xu_false;
static xu_fwatcher_ref_t g_fwatcher = xu_null;
static xu_int_t          xu_demo_watcher(xu_cpointer_t priv)
{
    xu_char_t const*  path     = (xu_char_t const*)priv;
    xu_fwatcher_ref_t fwatcher = xu_fwatcher_init();
    if (fwatcher)
    {
        g_fwatcher = fwatcher;
        xu_trace_i("watching %s", path);
        if (xu_fwatcher_add(fwatcher, path, xu_true))
        {
            xu_bool_t           eof = xu_false;
            xu_fwatcher_event_t event;
            while (!eof && !g_stop && xu_fwatcher_wait(fwatcher, &event, -1) >= 0)
            {
                xu_char_t const* status = event.event == XU_FWATCHER_EVENT_CREATE
                                              ? "created"
                                              : (event.event == XU_FWATCHER_EVENT_MODIFY ? "modified" : "deleted");
                xu_trace_i("watch: %s %s", event.filepath, status);
                if (xu_strstr(event.filepath, "eof")) eof = xu_true;
            }
        }
        xu_fwatcher_exit(fwatcher);
    }
    g_fwatcher = xu_null;
    return 0;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_fwatcher_main(xu_int_t argc, xu_char_t** argv)
{
    xu_thread_ref_t thread = xu_thread_init(xu_null, xu_demo_watcher, argv[1], 0);
    if (thread)
    {
        xu_getchar();
        g_stop = xu_true;
        if (g_fwatcher) xu_fwatcher_spak(g_fwatcher);
        xu_thread_wait(thread, -1, xu_null);
        xu_thread_exit(thread);
    }
    return 0;
}
