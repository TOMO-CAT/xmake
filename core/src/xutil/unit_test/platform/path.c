#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */
#ifdef XU_CONFIG_OS_WINDOWS
#    define xu_compare_path xu_stricmp
#else
#    define xu_compare_path xu_strcmp
#endif

/* *******************************************************
 * private implementation
 */

static xu_void_t xu_demo_path_test_directory(xu_char_t const* path, xu_char_t const* excepted)
{
    xu_char_t        data[XU_PATH_MAXN] = {0};
    xu_char_t const* result             = xu_path_directory(path, data, sizeof(data));
    if (result && excepted && !xu_compare_path(result, excepted))
        xu_trace_i("directory(%s): %s passed!", path, result);
    else if (!result && !excepted)
        xu_trace_i("directory(%s): null passed!", path);
    else
        xu_trace_i("directory(%s): %s != %s", path, result, excepted);
}

static xu_void_t xu_demo_path_test_absolute_to(xu_char_t const* root, xu_char_t const* path, xu_char_t const* excepted)
{
    xu_char_t        data[XU_PATH_MAXN] = {0};
    xu_char_t const* result             = xu_path_absolute_to(root, path, data, sizeof(data));
    if (result && excepted && !xu_compare_path(result, excepted))
        xu_trace_i("absolute_to(%s, %s): %s passed!", root, path, result);
    else if (!result && !excepted)
        xu_trace_i("absolute_to(%s, %s): null passed!", root, path);
    else
        xu_trace_i("absolute_to(%s, %s): %s != %s", root, path, result, excepted);
}

static xu_void_t xu_demo_path_test_relative_to(xu_char_t const* root, xu_char_t const* path, xu_char_t const* excepted)
{
    xu_char_t        data[XU_PATH_MAXN] = {0};
    xu_char_t const* result             = xu_path_relative_to(root, path, data, sizeof(data));
    if (result && excepted && !xu_compare_path(result, excepted))
        xu_trace_i("relative_to(%s, %s): %s passed!", root, path, result);
    else if (!result && !excepted)
        xu_trace_i("relative_to(%s, %s): null passed!", root, path);
    else
        xu_trace_i("relative_to(%s, %s): %s != %s", root, path, result, excepted);
}

static xu_void_t xu_demo_path_test_translate(xu_bool_t reduce_dot2, xu_char_t const* path, xu_char_t const* excepted)
{
    xu_char_t data[XU_PATH_MAXN] = {0};
    xu_strcpy(data, path);
    xu_size_t size = xu_path_translate(data, 0, sizeof(data), reduce_dot2);
    if (excepted && size && !xu_compare_path(data, excepted))
        xu_trace_i("translate(%s): %s passed!", path, data);
    else if (!size && !excepted)
        xu_trace_i("translate(%s): null passed!", path);
    else
        xu_trace_i("translate(%s): %s != %s", path, data, excepted);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_path_main(xu_int_t argc, xu_char_t** argv)
{
    xu_demo_path_test_directory("", xu_null);
    xu_demo_path_test_directory(".", xu_null);
    xu_demo_path_test_directory("foo", ".");
#ifdef XU_CONFIG_OS_WINDOWS
    xu_demo_path_test_directory("c:", xu_null);
    xu_demo_path_test_directory("c:\\", xu_null);
    xu_demo_path_test_directory("c:\\xxx", "c:");
    xu_demo_path_test_directory("c:\\xxx\\yyy", "c:\\xxx");
#else
    xu_demo_path_test_directory("/tmp", "/");
    xu_demo_path_test_directory("/tmp/", "/");
    xu_demo_path_test_directory("/tmp/xxx", "/tmp");
    xu_demo_path_test_directory("/tmp/xxx/", "/tmp");
    xu_demo_path_test_directory("/", xu_null);
#endif

    xu_trace_i("");
    xu_demo_path_test_absolute_to("", "", xu_null);
    xu_demo_path_test_absolute_to(".", ".", ".");
#ifdef XU_CONFIG_OS_WINDOWS
    xu_demo_path_test_absolute_to("c:", "foo", "c:\\foo");
    xu_demo_path_test_absolute_to("c:\\", "foo", "c:\\foo");
    xu_demo_path_test_absolute_to("c:\\tmp", "foo", "c:\\tmp\\foo");
    xu_demo_path_test_absolute_to("c:\\tmp\\", "foo", "c:\\tmp\\foo");
    xu_demo_path_test_absolute_to("\\\\.\\tmp\\", "foo", "\\\\.\\tmp\\foo");
    xu_demo_path_test_absolute_to("\\\\wsl.localhost\\tmp\\", "foo", "\\\\wsl.localhost\\tmp\\foo");
    xu_demo_path_test_absolute_to("\\tmp\\", "foo", "\\tmp\\foo");
#else
    xu_demo_path_test_absolute_to("/", "", xu_null);
    xu_demo_path_test_absolute_to("/", "/", "/");
    xu_demo_path_test_absolute_to("/", ".", "/");
    xu_demo_path_test_absolute_to("/tmp/", "foo", "/tmp/foo");
    xu_demo_path_test_absolute_to("/tmp", "foo", "/tmp/foo");
#endif

    xu_trace_i("");
    xu_demo_path_test_relative_to("", "", xu_null);
    xu_demo_path_test_relative_to(".", ".", ".");
#ifdef XU_CONFIG_OS_WINDOWS
    xu_demo_path_test_relative_to("c:\\", "c:", ".");
    xu_demo_path_test_relative_to("c:\\foo", "c:\\foo", ".");
    xu_demo_path_test_relative_to("c:\\", "c:\\foo", "foo");
    xu_demo_path_test_relative_to("c:\\tmp", "c:\\tmp\\foo", "foo");
    xu_demo_path_test_relative_to("c:\\tmp\\", "c:\\tmp\\foo", "foo");
#else
    xu_demo_path_test_relative_to("/", "", xu_null);
    xu_demo_path_test_relative_to("/", "/", ".");
    xu_demo_path_test_relative_to("/tmp/", "/tmp/foo", "foo");
    xu_demo_path_test_relative_to("/tmp", "/tmp/foo", "foo");
#endif

    xu_trace_i("");
    xu_demo_path_test_translate(xu_false, "", xu_null);
    xu_demo_path_test_translate(xu_false, ".", ".");
    xu_demo_path_test_translate(xu_false, "..", "..");
    xu_demo_path_test_translate(xu_true, "././.", ".");
    xu_demo_path_test_translate(xu_true, "../foo/..", "..");
    xu_demo_path_test_translate(xu_true, "../foo/bar/../..", "..");
#ifdef XU_CONFIG_OS_WINDOWS
    xu_demo_path_test_translate(xu_false, "c:", "c:");
    xu_demo_path_test_translate(xu_false, "c:\\", "c:");
    xu_demo_path_test_translate(xu_false, "c:\\foo\\\\\\", "c:\\foo");
    xu_demo_path_test_translate(xu_false, "c:\\foo\\..\\..", "c:\\foo\\..\\..");
    xu_demo_path_test_translate(xu_true, "c:\\foo\\.\\.\\", "c:\\foo");
    xu_demo_path_test_translate(xu_true, "c:\\foo\\bar\\.\\..\\xyz", "c:\\foo\\xyz");
    xu_demo_path_test_translate(xu_true, "c:\\foo\\..\\..", "c:");
    xu_demo_path_test_translate(xu_true, "../..", "..\\..");
    xu_demo_path_test_translate(xu_true, "../foo/bar/..", "..\\foo");
    xu_demo_path_test_translate(xu_true, "../foo/bar/../../..", "..\\..");
    xu_demo_path_test_translate(xu_false, "c:\\temp\\test-file.txt", "c:\\temp\\test-file.txt");
    xu_demo_path_test_translate(xu_false, "\\\\127.0.0.1\\c$\\temp\\test-file.txt",
                                "\\\\127.0.0.1\\c$\\temp\\test-file.txt");
    xu_demo_path_test_translate(xu_false, "\\\\.\\c:\\temp\\test-file.txt", "\\\\.\\c:\\temp\\test-file.txt");
    xu_demo_path_test_translate(xu_false, "\\\\?\\c:\\temp\\test-file.txt", "\\\\?\\c:\\temp\\test-file.txt");
    xu_demo_path_test_translate(xu_false, "\\\\.\\UNC\\LOCALHOST\\c$\\temp\\test-file.txt",
                                "\\\\.\\UNC\\LOCALHOST\\c$\\temp\\test-file.txt");
    xu_demo_path_test_translate(xu_false, "\\\\127.0.0.1\\c$\\temp\\test-file.txt",
                                "\\\\127.0.0.1\\c$\\temp\\test-file.txt");
    xu_demo_path_test_translate(xu_false, "\\temp\\test-file.txt", "\\temp\\test-file.txt");
#else
    xu_demo_path_test_translate(xu_false, "/", "/");
    xu_demo_path_test_translate(xu_false, "////", "/");
    xu_demo_path_test_translate(xu_false, "/foo//////", "/foo");
    xu_demo_path_test_translate(xu_false, "/foo/../..", "/foo/../..");
    xu_demo_path_test_translate(xu_false, "/foo/../../", "/foo/../..");
    xu_demo_path_test_translate(xu_true, "/foo/././", "/foo");
    xu_demo_path_test_translate(xu_true, "/./././", "/");
    xu_demo_path_test_translate(xu_true, "/foo/bar/.//..//xyz", "/foo/xyz");
    xu_demo_path_test_translate(xu_true, "/foo/../..", "/");
    xu_demo_path_test_translate(xu_true, "/foo/bar../..", "/foo");
    xu_demo_path_test_translate(xu_true, "../..", "../..");
    xu_demo_path_test_translate(xu_true, "../foo/bar/..", "../foo");
    xu_demo_path_test_translate(xu_true, "../foo/bar/../../..", "../..");
#endif
    return 0;
}
