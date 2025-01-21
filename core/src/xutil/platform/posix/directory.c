#include "xutil/platform/directory.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/platform/environment.h"
#include "xutil/platform/path.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* *******************************************************
 * private implementation
 * *******************************************************
 */

static xu_long_t xu_directory_walk_remove(xu_char_t const* path, xu_file_info_t const* info, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(path && info, XU_DIRECTORY_WALK_CODE_END);

    // remove file, directory and dead symbol link (info->type is none, file not exists)
    remove(path);
    return XU_DIRECTORY_WALK_CODE_CONTINUE;
}
static xu_long_t xu_directory_walk_copy(xu_char_t const* path, xu_file_info_t const* info, xu_cpointer_t priv)
{
    // check
    xu_value_t* tuple = (xu_value_t*)priv;
    xu_assert_and_check_return_val(path && info && priv, XU_DIRECTORY_WALK_CODE_END);

    // the dest directory
    xu_char_t const* dest = tuple[0].cstr;
    xu_assert_and_check_return_val(dest, XU_DIRECTORY_WALK_CODE_END);

    // the file name
    xu_size_t        size = tuple[1].ul;
    xu_char_t const* name = path + size;

    // the copy flags
    xu_size_t flags = tuple[2].ul;

    // the dest file path
    xu_char_t dpath[8192] = {0};
    xu_snprintf(dpath, 8192, "%s/%s", dest, name[0] == '/' ? name + 1 : name);

    // remove the dest file first
    xu_file_info_t dinfo = {0};
    if (xu_file_info(dpath, &dinfo))
    {
        if (dinfo.type == XU_FILE_TYPE_FILE) xu_file_remove(dpath);
        if (dinfo.type == XU_FILE_TYPE_DIRECTORY) xu_directory_remove(dpath);
    }

    // do copy
    xu_bool_t ok             = xu_true;
    xu_bool_t skip_recursion = xu_false;
    switch (info->type)
    {
    case XU_FILE_TYPE_FILE: ok = xu_file_copy(path, dpath, flags); break;
    case XU_FILE_TYPE_DIRECTORY:
    {
        // reserve symlink?
        if ((flags & XU_FILE_COPY_LINK) && (info->flags & XU_FILE_FLAG_LINK))
        {
            // just copy link and skip recursion
            ok             = xu_file_copy(path, dpath, XU_FILE_COPY_LINK);
            skip_recursion = xu_true;
        }
        else
            ok = xu_directory_create(dpath);
    }
    break;
    default: break;
    }
    tuple[3].b        = ok;
    xu_size_t retcode = XU_DIRECTORY_WALK_CODE_CONTINUE;
    if (skip_recursion) retcode |= XU_DIRECTORY_WALK_CODE_SKIP_RECURSION;
    return retcode;
}
static xu_long_t xu_directory_walk_impl(xu_char_t const* path, xu_long_t recursion, xu_bool_t prefix,
                                        xu_directory_walk_func_t func, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(path && func, XU_DIRECTORY_WALK_CODE_END);

    // last
    xu_long_t last = xu_strlen(path) - 1;
    xu_assert_and_check_return_val(last >= 0, XU_DIRECTORY_WALK_CODE_END);

    // done
    xu_long_t ok         = XU_DIRECTORY_WALK_CODE_CONTINUE;
    xu_char_t temp[4096] = {0};
    DIR*      directory  = xu_null;
    if ((directory = opendir(path)))
    {
        // walk
        xu_char_t      name[1024];
        struct dirent* item = xu_null;
        while ((item = readdir(directory)))
        {
            // get the item name
            if (sizeof(name) == xu_strlcpy(name, item->d_name, sizeof(name))) continue;

            if (xu_strcmp(name, ".") && xu_strcmp(name, ".."))
            {
                // the temp path
                xu_long_t n = xu_snprintf(temp, 4095, "%s%s%s", path, path[last] == '/' ? "" : "/", name);
                if (n >= 0) temp[n] = '\0';

                // get the file info (file maybe not exists, dead symbol link)
                xu_file_info_t info = {0};
                xu_file_info(temp, &info);

                // do callback
                if (prefix) ok = func(temp, &info, priv);
                xu_check_break(ok);

                // walk to the next directory
                if (info.type == XU_FILE_TYPE_DIRECTORY && recursion && ok != XU_DIRECTORY_WALK_CODE_SKIP_RECURSION)
                    ok = xu_directory_walk_impl(temp, recursion > 0 ? recursion - 1 : recursion, prefix, func, priv);
                xu_check_break(ok);

                // do callback
                if (!prefix) ok = func(temp, &info, priv);
                xu_check_break(ok);
            }
        }

        // exit directory
        closedir(directory);
    }

    // continue ?
    return ok;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_bool_t xu_directory_create(xu_char_t const* path)
{
    // check
    xu_assert_and_check_return_val(path, xu_false);

    // the full path
    xu_char_t full[XU_PATH_MAXN];
    path = xu_path_absolute(path, full, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // make it (0755: drwxr-xr-x)
    xu_bool_t ok = !mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
    if (!ok && (errno != EPERM && errno != EACCES))
    {
        // make directory
        xu_char_t        temp[XU_PATH_MAXN] = {0};
        xu_char_t const* p                  = full;
        xu_char_t*       t                  = temp;
        xu_char_t const* e                  = temp + XU_PATH_MAXN - 1;
        for (; t < e && *p; t++)
        {
            *t = *p;
            if (*p == '/')
            {
                // make directory if not exists
                if (!xu_file_info(temp, xu_null))
                {
                    if (mkdir(temp, S_IRWXU | S_IRWXG | S_IRWXO) != 0) return xu_false;
                }

                // skip repeat '/'
                while (*p && *p == '/')
                    p++;
            }
            else
                p++;
        }

        // make it again
        ok = !mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
    }
    return ok;
}
xu_bool_t xu_directory_remove(xu_char_t const* path)
{
    // the full path
    xu_char_t full[XU_PATH_MAXN];
    path = xu_path_absolute(path, full, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // walk remove
    xu_directory_walk_impl(path, -1, xu_false, xu_directory_walk_remove, xu_null);

    // remove it
    return !remove(path) ? xu_true : xu_false;
}
xu_size_t xu_directory_current(xu_char_t* path, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(path && maxn, 0);

    // the current directory
    xu_size_t size = 0;
    if (getcwd(path, maxn - 1)) size = xu_strlen(path);

    // ok?
    return size;
}
xu_bool_t xu_directory_current_set(xu_char_t const* path)
{
    // the absolute path
    xu_char_t full[XU_PATH_MAXN];
    path = xu_path_absolute(path, full, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // change to the directory
    return !chdir(path);
}
#if !defined(XU_CONFIG_OS_IOS) && !defined(XU_CONFIG_OS_ANDROID)
xu_size_t xu_directory_home(xu_char_t* path, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(path && maxn, 0);

    // get the home directory
    xu_size_t size = 0;
    if (!(size = xu_environment_first("HOME", path, maxn))) size = xu_environment_first("XDG_CONFIG_HOME", path, maxn);
    return size;
}
xu_size_t xu_directory_temporary(xu_char_t* path, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(path && maxn > 4, 0);

    // get the temporary directory
    xu_size_t size = 0;
    if (!(size = xu_environment_first("TMPDIR", path, maxn))) size = xu_strlcpy(path, "/tmp", maxn);
    return size;
}
#endif
xu_void_t xu_directory_walk(xu_char_t const* path, xu_long_t recursion, xu_bool_t prefix, xu_directory_walk_func_t func,
                            xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return(path && func);

    // walk it directly if rootdir is relative path
    xu_file_info_t info = {0};
    if (!xu_path_is_absolute(path) && xu_file_info(path, &info) && info.type == XU_FILE_TYPE_DIRECTORY)
        xu_directory_walk_impl(path, recursion, prefix, func, priv);
    else
    {
        // the absolute path (translate "~/")
        xu_char_t full[XU_PATH_MAXN];
        path = xu_path_absolute(path, full, XU_PATH_MAXN);
        xu_assert_and_check_return(path);

        // walk
        xu_directory_walk_impl(path, recursion, prefix, func, priv);
    }
}
xu_bool_t xu_directory_copy(xu_char_t const* path, xu_char_t const* dest, xu_size_t flags)
{
    // the absolute path
    xu_char_t full0[XU_PATH_MAXN];
    path = xu_path_absolute(path, full0, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // the dest path
    xu_char_t full1[XU_PATH_MAXN];
    dest = xu_path_absolute(dest, full1, XU_PATH_MAXN);
    xu_assert_and_check_return_val(dest, xu_false);

    // walk copy
    xu_value_t tuple[4];
    tuple[0].cstr = dest;
    tuple[1].ul   = xu_strlen(path);
    tuple[2].ul   = flags;
    tuple[3].b    = xu_true;
    xu_directory_walk_impl(path, -1, xu_true, xu_directory_walk_copy, tuple);

    // copy empty directory?
    xu_bool_t ok = tuple[3].b;
    if (ok && !xu_file_info(dest, xu_null)) return xu_directory_create(dest);
    return ok;
}
