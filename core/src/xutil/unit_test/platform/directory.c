#include "xutil/unit_test/ut.h"

/* *******************************************************
 * callback
 */
#if 0
static xu_long_t xu_directory_walk_func(xu_char_t const* path, xu_file_info_t const* info, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(path && info, XU_DIRECTORY_WALK_CODE_END);

    // the modified time
    xu_tm_t mtime = {0};
    xu_localtime(info->mtime, &mtime);

    // trace
    xu_trace_i( "path[%c]: %s, size: %llu, mtime: %04ld-%02ld-%02ld %02ld:%02ld:%02ld"
            , info->type == XU_FILE_TYPE_DIRECTORY? 'd' : 'f'
            , path
            , info->size
            , mtime.year
            , mtime.month
            , mtime.mday
            , mtime.hour
            , mtime.minute
            , mtime.second);
    return XU_DIRECTORY_WALK_CODE_CONTINUE;
}
#endif

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_directory_main(xu_int_t argc, xu_char_t** argv)
{
#if 0
    // home
    xu_char_t home[XU_PATH_MAXN] = {0};
    if (xu_directory_home(home, sizeof(home))) xu_trace_i("home: %s", home);

    // current
    xu_char_t current[XU_PATH_MAXN] = {0};
    if (xu_directory_current(current, sizeof(current))) xu_trace_i("current: %s", current);

    // temporary
    xu_char_t temporary[XU_PATH_MAXN] = {0};
    if (xu_directory_temporary(temporary, sizeof(temporary))) xu_trace_i("temporary: %s", temporary);
#elif 0

    // current
    xu_char_t current[XU_PATH_MAXN] = {0};
    if (xu_directory_current(current, sizeof(current))) xu_trace_i("current: %s", current);

    // current
    xu_directory_walk(argv[1] ? argv[1] : current, -1, xu_true, xu_directory_walk_func, xu_null);
#elif 0
    xu_directory_remove(argv[1]);
#elif 1
    xu_directory_copy(argv[1], argv[2], XU_FILE_COPY_LINK);
#elif 0
    xu_directory_create(argv[1]);
#else
    xu_directory_walk(argv[1], 1, xu_true, xu_directory_walk_func, xu_null);
#endif

    return 0;
}
