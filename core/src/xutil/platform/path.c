/* *******************************************************
 * trace
 */

#define XU_TRACE_MODULE_NAME "path"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/platform/path.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/platform/directory.h"
#include "xutil/utils/trace.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * macros
 */

// the path separator
#define XU_PATH_SEPARATOR '/'

// is path separator?
#define xu_path_is_sep(c) ('/' == (c))

/* *******************************************************
 * implementation
 */

#ifndef XU_CONFIG_MICRO_ENABLE
xu_size_t xu_path_translate(xu_char_t* path, xu_size_t size, xu_size_t maxn, xu_bool_t normalize)
{
    return xu_path_translate_to(path, size, path, maxn, normalize);
}

xu_size_t xu_path_translate_to(xu_char_t const* path, xu_size_t size, xu_char_t* data, xu_size_t maxn,
                               xu_bool_t normalize)
{
    // check
    xu_assert_and_check_return_val(path, 0);

    // file://?
    xu_char_t        home[XU_PATH_MAXN];
    xu_char_t const* p = path;
    if (!xu_strnicmp(p, "file:", 5)) p += 5;
    // is user directory?
    else if (path[0] == '~')
    {
        // get the home directory
        xu_size_t home_size = xu_directory_home(home, sizeof(home));
        xu_assert_and_check_return_val(home_size, 0);

        // check the path space
        xu_size_t path_size = size ? size : xu_strlen(path);
        xu_assert_and_check_return_val(home_size + path_size - 1 < maxn, 0);

        /* move the path and ensure the enough space for the home directory
         *
         * @note maybe path and data are same address
         */
        xu_memcpy(home + home_size, path + 1, path_size - 1);
        home[home_size + path_size - 1] = '\0';
        size                            = home_size + path_size - 1;

        // switch home as path source
        path = home;
        p    = home;
    }
    if (!size) size = xu_strlen(path);

    // copy root path
    xu_char_t*       dst      = data;
    xu_char_t const* src      = p;
    xu_char_t const* src_root = src;
    xu_char_t const* dst_root = dst;
    if (xu_path_is_sep(*src))
    {
        ++src;
        *(dst++) = XU_PATH_SEPARATOR;
    }

#    define xu_path_is_end(__p) (__p >= src_end || (*__p) == '\0')
#    define xu_path_is_sep_or_end(__p) (xu_path_is_end(__p) || xu_path_is_sep(*__p))
    xu_char_t const* src_end      = path + size;
    xu_long_t        folder_depth = 0;
    while (!xu_path_is_end(src))
    {
        // reduce repeat separators and "/./" => "/"
        while (xu_path_is_sep(*src) || (normalize && &src[-1] >= src_root && xu_path_is_sep(src[-1]) && src[0] == '.' &&
                                        xu_path_is_sep_or_end(&src[1])))
            ++src;

        if (xu_path_is_end(src)) break;

        // reduce "foo/bar/../" => "foo"
        if (normalize && src[0] == '.' && src[1] == '.' && xu_path_is_sep_or_end(&src[2]))
        {
            if (folder_depth > 0)
            {
                while (--dst != dst_root && !xu_path_is_sep(dst[-1]))
                    ;
                --folder_depth;
            }
            else if (&dst[-1] != dst_root || !xu_path_is_sep(dst[-1]))
            {
                /* "/foo/../.." => "/"
                 * "foo/../.." => "../"
                 */
                *(dst++) = '.';
                *(dst++) = '.';
                *(dst++) = XU_PATH_SEPARATOR;
            }
            src += 3;
        }
        else
        {
            while (!xu_path_is_sep_or_end(src))
                *(dst++) = *(src++);

            if (xu_path_is_sep(*src))
            {
                *(dst++) = XU_PATH_SEPARATOR;
                ++src;
                ++folder_depth;
            }
        }
    }
#    undef xu_path_is_end
#    undef xu_path_is_sep_or_end

    // remove the tail separator and not root: '/'
    while (dst > data + 1 && xu_path_is_sep(dst[-1]))
        --dst;
    *dst = '\0';
    xu_trace_d("translate: %s", data);
    return dst - data;
}
#endif
xu_bool_t xu_path_is_absolute(xu_char_t const* path)
{
    // check
    xu_assert_and_check_return_val(path, xu_false);
    return (path[0] == '/' || path[0] == '\\' || path[0] == '~' || !xu_strnicmp(path, "file:", 5));
}
xu_char_t const* xu_path_absolute(xu_char_t const* path, xu_char_t* data, xu_size_t maxn)
{
    return xu_path_absolute_to(xu_null, path, data, maxn);
}
xu_char_t const* xu_path_absolute_to(xu_char_t const* root, xu_char_t const* path, xu_char_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(path && data && maxn, xu_null);

    // trace
    xu_trace_d("path: %s", path);

    // empty path?
    xu_check_return_val(path[0], xu_null);

#ifdef XU_CONFIG_MICRO_ENABLE

    // the path is absolute?
    if (xu_path_is_absolute(path)) return path;

    // trace
    xu_trace_e("absolute to %s to %s failed!", path, root);
    return xu_null;
#else

    // the path is absolute?
    if (xu_path_is_absolute(path)) return xu_path_translate_to(path, 0, data, maxn, xu_false) ? data : xu_null;

    // get the root directory
    xu_size_t size = 0;
    if (root)
    {
        // copy it
        size = xu_strlcpy(data, root, maxn);
        xu_assert_and_check_return_val(size < maxn, xu_null);
    }
    else
    {
        // get the current directory
        if (!(size = xu_directory_current(data, maxn))) return xu_null;
    }

    // translate the root directory
    size = xu_path_translate(data, size, maxn, xu_false);

    // trace
    xu_trace_d("root: %s, size: %lu", data, size);

    xu_char_t* absolute = data;

    // path => data
    xu_char_t const* p = path;
    xu_char_t const* t = p;
    xu_char_t*       q = absolute + size;
    xu_char_t const* e = absolute + maxn - 1;
    while (1)
    {
        if (xu_path_is_sep(*p) || !*p)
        {
            // the item size
            xu_size_t n = p - t;

            // ..? remove item
            if (n == 2 && t[0] == '.' && t[1] == '.')
            {
                // find the last separator
                for (; q > absolute && *q != XU_PATH_SEPARATOR; q--)
                    ;

                // strip it
                *q = '\0';
            }
            // .? continue it
            else if (n == 1 && t[0] == '.')
                ;
            // append item
            else if (n && q + 1 + n < e)
            {
                // append separator
                *q++ = XU_PATH_SEPARATOR;

                // append item
                xu_strncpy(q, t, n);
                q += n;

                // strip it
                *q = '\0';
            }
            // empty item? remove repeat
            else if (!n)
                ;
            // too small?
            else
            {
                // trace
                xu_trace_e("the data path is too small for %s", path);
                return xu_null;
            }

            // break
            xu_check_break(*p);

            // next
            t = p + 1;
        }

        // next
        p++;
    }

    // end
    if (q > absolute) *q = '\0';
    // root?
    else
    {
        *q++ = XU_PATH_SEPARATOR;
        *q   = '\0';
    }

    // trace
    xu_trace_d("absolute: %s", data);

    // ok?
    return data;
#endif
}
#ifndef XU_CONFIG_MICRO_ENABLE
xu_char_t const* xu_path_relative(xu_char_t const* path, xu_char_t* data, xu_size_t maxn)
{
    return xu_path_relative_to(xu_null, path, data, maxn);
}
xu_char_t const* xu_path_relative_to(xu_char_t const* root, xu_char_t const* path, xu_char_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(path && data && maxn, xu_null);

    // trace
    xu_trace_d("path: %s", path);

    // the root is the current and the path is absolute? return path directly
    if (!root && !xu_path_is_absolute(path))
        return xu_path_translate_to(path, 0, data, maxn, xu_false) ? data : xu_null;

    // get the absolute path
    xu_size_t path_size = 0;
    xu_char_t path_absolute[XU_PATH_MAXN];
    xu_size_t path_maxn = sizeof(path_absolute);
    path                = xu_path_absolute(path, path_absolute, path_maxn);
    xu_check_return_val(path, xu_null);

    path_size = xu_strlen(path);
    xu_assert_and_check_return_val(path && path_size && path_size < path_maxn, xu_null);

    // trace
    xu_trace_d("path_absolute: %s", path);

    // get the absolute root
    xu_size_t root_size = 0;
    xu_char_t root_absolute[XU_PATH_MAXN];
    xu_size_t root_maxn = sizeof(root_absolute);
    if (root)
    {
        // get the absolute root
        root      = xu_path_absolute(root, root_absolute, root_maxn);
        root_size = xu_strlen(root);
    }
    else
    {
        // get the current directory
        if (!(root_size = xu_directory_current(root_absolute, root_maxn))) return xu_null;

        // translate it
        if (!(root_size = xu_path_translate(root_absolute, root_size, root_maxn, xu_false))) return xu_null;
        root = root_absolute;
    }
    xu_assert_and_check_return_val(root && root_size && root_size < root_maxn, xu_null);

    // trace
    xu_trace_d("root_absolute: %s", root);

    // same directory? return "."
    if (path_size == root_size && !xu_strncmp(path, root, root_size))
    {
        // check
        xu_assert_and_check_return_val(maxn >= 2, ".");

        // return "."
        data[0] = '.';
        data[1] = '\0';
        return data;
    }

    // append separator
    if (path_size + 1 < path_maxn)
    {
        path_absolute[path_size++] = XU_PATH_SEPARATOR;
        path_absolute[path_size]   = '\0';
    }
    if (root_size + 1 < root_maxn)
    {
        root_absolute[root_size++] = XU_PATH_SEPARATOR;
        root_absolute[root_size]   = '\0';
    }

    // trace
    xu_trace_d("path: %s, root: %s", path_absolute, root_absolute);

    // find the common leading directory
    xu_char_t const* p    = path_absolute;
    xu_char_t const* q    = root_absolute;
    xu_long_t        last = -1;
    for (; *p && *q && *p == *q; q++, p++)
    {
        // save the last separator
        if (*p == XU_PATH_SEPARATOR) last = q - root_absolute;
    }

    // is different directory? using the absolute path
    if (last <= 0)
    {
        // trace
        xu_trace_d("no common root: %d", last);

        // the path size
        xu_size_t size = xu_min(path_size - 1, maxn);

        // copy it
        xu_strncpy(data, path, size);
        data[size] = '\0';
    }
    // exists same root?
    else
    {
        // count the remaining levels in root
        xu_size_t        count = 0;
        xu_char_t const* l     = root_absolute + last + 1;
        for (; *l; l++)
        {
            if (*l == XU_PATH_SEPARATOR) count++;
        }

        // append "../" or "..\\"
        xu_char_t* d = data;
        xu_char_t* e = data + maxn;
        while (count--)
        {
            if (d + 3 < e)
            {
                d[0] = '.';
                d[1] = '.';
                d[2] = XU_PATH_SEPARATOR;
                d += 3;
            }
        }

        // append the left path
        l = path_absolute + last + 1;
        while (*l && d < e)
            *d++ = *l++;

        // remove the last separator
        if (d > data) d--;

        // end
        *d = '\0';
    }

    // trace
    xu_trace_d("relative: %s", data);

    // ok?
    return data;
}
#endif
xu_char_t const* xu_path_directory(xu_char_t const* path, xu_char_t* data, xu_size_t maxn)
{
    xu_assert_and_check_return_val(path && data && maxn, xu_null);

    xu_size_t        n = xu_strlen(path);
    xu_char_t const* e = path + n;
    xu_char_t const* p = e - 1;
    while (p >= path && *p && xu_path_is_sep(*p))
        p--;
    while (p >= path && *p && !xu_path_is_sep(*p))
        p--;
    if (p >= path)
    {
        if ((p == path || !xu_path_is_sep(*p)) && p < e) p++;
        n = p - path;
        if (n && n < maxn)
        {
            xu_strncpy(data, path, n);
            data[n] = '\0';
            return data;
        }
        else
            return xu_null;
    }
    return (n && path[0] != '.' && !xu_path_is_absolute(path)) ? "." : xu_null;
}
