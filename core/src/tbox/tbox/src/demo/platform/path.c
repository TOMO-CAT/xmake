/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_compare_path  tb_strcmp

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static tb_void_t tb_demo_path_test_directory(tb_char_t const* path, tb_char_t const* excepted)
{
    tb_char_t data[TB_PATH_MAXN] = {0};
    tb_char_t const* result = tb_path_directory(path, data, sizeof(data));
    if (result && excepted && !tb_compare_path(result, excepted))
        tb_trace_i("directory(%s): %s passed!", path, result);
    else if (!result && !excepted)
        tb_trace_i("directory(%s): null passed!", path);
    else tb_trace_i("directory(%s): %s != %s", path, result, excepted);
}

static tb_void_t tb_demo_path_test_absolute_to(tb_char_t const* root, tb_char_t const* path, tb_char_t const* excepted)
{
    tb_char_t data[TB_PATH_MAXN] = {0};
    tb_char_t const* result = tb_path_absolute_to(root, path, data, sizeof(data));
    if (result && excepted && !tb_compare_path(result, excepted))
        tb_trace_i("absolute_to(%s, %s): %s passed!", root, path, result);
    else if (!result && !excepted)
        tb_trace_i("absolute_to(%s, %s): null passed!", root, path);
    else tb_trace_i("absolute_to(%s, %s): %s != %s", root, path, result, excepted);
}

static tb_void_t tb_demo_path_test_relative_to(tb_char_t const* root, tb_char_t const* path, tb_char_t const* excepted)
{
    tb_char_t data[TB_PATH_MAXN] = {0};
    tb_char_t const* result = tb_path_relative_to(root, path, data, sizeof(data));
    if (result && excepted && !tb_compare_path(result, excepted))
        tb_trace_i("relative_to(%s, %s): %s passed!", root, path, result);
    else if (!result && !excepted)
        tb_trace_i("relative_to(%s, %s): null passed!", root, path);
    else tb_trace_i("relative_to(%s, %s): %s != %s", root, path, result, excepted);
}

static tb_void_t tb_demo_path_test_translate(tb_bool_t reduce_dot2, tb_char_t const* path, tb_char_t const* excepted)
{
    tb_char_t data[TB_PATH_MAXN] = {0};
    tb_strcpy(data, path);
    tb_size_t size = tb_path_translate(data, 0, sizeof(data), reduce_dot2);
    if (excepted && size && !tb_compare_path(data, excepted))
        tb_trace_i("translate(%s): %s passed!", path, data);
    else if (!size && !excepted)
        tb_trace_i("translate(%s): null passed!", path);
    else tb_trace_i("translate(%s): %s != %s", path, data, excepted);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_path_main(tb_int_t argc, tb_char_t** argv)
{
    tb_demo_path_test_directory("", tb_null);
    tb_demo_path_test_directory(".", tb_null);
    tb_demo_path_test_directory("foo", ".");
    tb_demo_path_test_directory("/tmp", "/");
    tb_demo_path_test_directory("/tmp/", "/");
    tb_demo_path_test_directory("/tmp/xxx", "/tmp");
    tb_demo_path_test_directory("/tmp/xxx/", "/tmp");
    tb_demo_path_test_directory("/", tb_null);

    tb_trace_i("");
    tb_demo_path_test_absolute_to("", "", tb_null);
    tb_demo_path_test_absolute_to(".", ".", ".");
    tb_demo_path_test_absolute_to("/", "", tb_null);
    tb_demo_path_test_absolute_to("/", "/", "/");
    tb_demo_path_test_absolute_to("/", ".", "/");
    tb_demo_path_test_absolute_to("/tmp/", "foo", "/tmp/foo");
    tb_demo_path_test_absolute_to("/tmp", "foo", "/tmp/foo");

    tb_trace_i("");
    tb_demo_path_test_relative_to("", "", tb_null);
    tb_demo_path_test_relative_to(".", ".", ".");
    tb_demo_path_test_relative_to("/", "", tb_null);
    tb_demo_path_test_relative_to("/", "/", ".");
    tb_demo_path_test_relative_to("/tmp/", "/tmp/foo", "foo");
    tb_demo_path_test_relative_to("/tmp", "/tmp/foo", "foo");

    tb_trace_i("");
    tb_demo_path_test_translate(tb_false, "", tb_null);
    tb_demo_path_test_translate(tb_false, ".", ".");
    tb_demo_path_test_translate(tb_false, "..", "..");
    tb_demo_path_test_translate(tb_true, "././.", ".");
    tb_demo_path_test_translate(tb_true, "../foo/..", "..");
    tb_demo_path_test_translate(tb_true, "../foo/bar/../..", "..");
    tb_demo_path_test_translate(tb_false, "/", "/");
    tb_demo_path_test_translate(tb_false, "////", "/");
    tb_demo_path_test_translate(tb_false, "/foo//////", "/foo");
    tb_demo_path_test_translate(tb_false, "/foo/../..", "/foo/../..");
    tb_demo_path_test_translate(tb_false, "/foo/../../", "/foo/../..");
    tb_demo_path_test_translate(tb_true, "/foo/././", "/foo");
    tb_demo_path_test_translate(tb_true, "/./././", "/");
    tb_demo_path_test_translate(tb_true, "/foo/bar/.//..//xyz", "/foo/xyz");
    tb_demo_path_test_translate(tb_true, "/foo/../..", "/");
    tb_demo_path_test_translate(tb_true, "/foo/bar../..", "/foo");
    tb_demo_path_test_translate(tb_true, "../..", "../..");
    tb_demo_path_test_translate(tb_true, "../foo/bar/..", "../foo");
    tb_demo_path_test_translate(tb_true, "../foo/bar/../../..", "../..");
    return 0;
}
