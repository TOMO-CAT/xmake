/* *******************************************************
 * trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "platform_file"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/platform/file.h"
#include "xutil/libc/misc/ctype.h"
#include "xutil/platform/path.h"

#if defined(XU_CONFIG_POSIX_HAVE_OPEN)
#    include "xutil/platform/posix/file.c"
#else
xu_file_ref_t xu_file_init(xu_char_t const* path, xu_size_t mode)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_bool_t xu_file_exit(xu_file_ref_t file)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_long_t xu_file_read(xu_file_ref_t file, xu_byte_t* data, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_file_writ(xu_file_ref_t file, xu_byte_t const* data, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_file_pread(xu_file_ref_t file, xu_byte_t* data, xu_size_t size, xu_hize_t offset)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_file_pwrit(xu_file_ref_t file, xu_byte_t const* data, xu_size_t size, xu_hize_t offset)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_file_readv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_file_writv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_hong_t xu_file_writf(xu_file_ref_t file, xu_file_ref_t ifile, xu_hize_t offset, xu_hize_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_file_preadv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size, xu_hize_t offset)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_file_pwritv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size, xu_hize_t offset)
{
    xu_trace_noimpl();
    return -1;
}
xu_bool_t xu_file_sync(xu_file_ref_t file)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_hong_t xu_file_seek(xu_file_ref_t file, xu_hong_t offset, xu_size_t mode)
{
    xu_trace_noimpl();
    return -1;
}
xu_hong_t xu_file_offset(xu_file_ref_t file)
{
    xu_trace_noimpl();
    return -1;
}
xu_hize_t xu_file_size(xu_file_ref_t file)
{
    xu_trace_noimpl();
    return 0;
}
xu_bool_t xu_file_info(xu_char_t const* path, xu_file_info_t* info)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_file_copy(xu_char_t const* path, xu_char_t const* dest, xu_size_t flags)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_file_create(xu_char_t const* path)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_file_remove(xu_char_t const* path)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_file_rename(xu_char_t const* path, xu_char_t const* dest)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_file_link(xu_char_t const* path, xu_char_t const* dest)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_file_access(xu_char_t const* path, xu_size_t mode)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_file_touch(xu_char_t const* path, xu_time_t atime, xu_time_t mtime)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
xu_long_t xu_file_fscase(xu_char_t const* path)
{
    // check
    xu_assert_and_check_return_val(path, -1);

    // flip path case
    xu_char_t        path_flipcase[XU_PATH_MAXN];
    xu_char_t const* p = path;
    xu_size_t        i = 0;
    xu_char_t        ch;
    while (*p && i < (XU_PATH_MAXN - 1))
    {
        ch                 = *p++;
        ch                 = xu_islower(ch) ? xu_toupper(ch) : xu_tolower(ch);
        path_flipcase[i++] = ch;
    }
    path_flipcase[i] = '\0';

    xu_file_info_t info, info_flipcase;
    if (xu_file_info(path, &info))
    {
        if (xu_file_info(path_flipcase, &info_flipcase) && info.size == info_flipcase.size &&
            info.mtime == info_flipcase.mtime && info.type == info_flipcase.type && info.flags == info_flipcase.flags)
        {
            return 0;
        }
        return 1;
    }
    else
        return -1;
}
