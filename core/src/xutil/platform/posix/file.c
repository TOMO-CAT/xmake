#include "xutil/platform/file.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"
#include "xutil/platform/directory.h"
#include "xutil/platform/path.h"
#include "xutil/utils/trace.h"
#include "xutil/utils/utils.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#ifdef XU_CONFIG_POSIX_HAVE_COPYFILE
#    include <copyfile.h>
#endif
#ifdef XU_CONFIG_POSIX_HAVE_SENDFILE
#    include <sys/sendfile.h>
#endif

xu_file_ref_t xu_file_init(xu_char_t const* path, xu_size_t mode)
{
    // check
    xu_assert_and_check_return_val(path, xu_null);

    // the full path
    xu_char_t full[XU_PATH_MAXN];
    path = xu_path_absolute(path, full, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_null);

    // flags
    xu_size_t flags = 0;
    if (mode & XU_FILE_MODE_RO)
        flags |= O_RDONLY;
    else if (mode & XU_FILE_MODE_WO)
        flags |= O_WRONLY;
    else if (mode & XU_FILE_MODE_RW)
        flags |= O_RDWR;

    if (mode & XU_FILE_MODE_CREAT) flags |= O_CREAT;
    if (mode & XU_FILE_MODE_APPEND) flags |= O_APPEND;
    if (mode & XU_FILE_MODE_TRUNC) flags |= O_TRUNC;

        // dma mode, no cache
#if defined(XU_CONFIG_OS_LINUX) && defined(O_DIRECT)
    if (mode & XU_FILE_MODE_DIRECT) flags |= O_DIRECT;
#endif

    // noblock
    flags |= O_NONBLOCK;

    // modes
    xu_size_t modes = 0;
    if (mode & XU_FILE_MODE_CREAT)
    {
        // 0644: -rw-r--r--
        modes = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    }

    // open it, @note need absolute path
    xu_long_t fd = open(path, flags, modes);
    if (fd < 0 && (mode & XU_FILE_MODE_CREAT) && (errno != EPERM && errno != EACCES))
    {
#ifndef XU_CONFIG_MICRO_ENABLE
        // open it again after creating the file directory
        xu_int_t  errno_bak = errno;
        xu_char_t dir[XU_PATH_MAXN];
        if (xu_directory_create(xu_path_directory(path, dir, sizeof(dir))))
            fd = open(path, flags, modes);
        else
            errno = errno_bak;
#endif
    }
    xu_check_return_val(fd >= 0, xu_null);

    // trace
    xu_trace_d("open: %p", xu_fd2file(fd));

    // ok?
    return xu_fd2file(fd);
}
xu_bool_t xu_file_exit(xu_file_ref_t file)
{
    // check
    xu_assert_and_check_return_val(file, xu_false);

    // trace
    xu_trace_d("close: %p", file);

    // close it
    xu_bool_t ok = !close(xu_file2fd(file)) ? xu_true : xu_false;

    // failed?
    if (!ok)
    {
        // trace
        xu_trace_e("close: %p failed, errno: %d", file, errno);
    }

    // ok?
    return ok;
}
xu_long_t xu_file_read(xu_file_ref_t file, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(file && data, -1);

    // read it
    return read(xu_file2fd(file), data, size);
}
xu_long_t xu_file_writ(xu_file_ref_t file, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(file && data, -1);

    // writ it
    return write(xu_file2fd(file), data, size);
}
xu_bool_t xu_file_sync(xu_file_ref_t file)
{
    // check
    xu_assert_and_check_return_val(file, xu_false);

    // sync
#ifdef XU_CONFIG_POSIX_HAVE_FDATASYNC
    return !fdatasync(xu_file2fd(file)) ? xu_true : xu_false;
#else
    return !fsync(xu_file2fd(file)) ? xu_true : xu_false;
#endif
}
xu_hong_t xu_file_seek(xu_file_ref_t file, xu_hong_t offset, xu_size_t mode)
{
    // check
    xu_assert_and_check_return_val(file, -1);

    // seek
    return lseek(xu_file2fd(file), offset, mode);
}
xu_hong_t xu_file_offset(xu_file_ref_t file)
{
    // check
    xu_assert_and_check_return_val(file, -1);

    // the offset
    return xu_file_seek(file, (xu_hong_t)0, XU_FILE_SEEK_CUR);
}
xu_hize_t xu_file_size(xu_file_ref_t file)
{
    // check
    xu_assert_and_check_return_val(file, 0);

    // the file size
    xu_hize_t   size = 0;
    struct stat st   = {0};
    if (!fstat(xu_file2fd(file), &st)) size = st.st_size;

    // ok?
    return size;
}
xu_bool_t xu_file_info(xu_char_t const* path, xu_file_info_t* info)
{
    // check
    xu_assert_and_check_return_val(path, xu_false);

    // the full path (need translate "~/")
    xu_char_t full[XU_PATH_MAXN];
    path = xu_path_absolute(path, full, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // get info
    if (info)
    {
        // init info
        xu_memset(info, 0, sizeof(xu_file_info_t));

        // get stat, even if the file does not exist, it may be a dead symbolic link
#if defined(XU_CONFIG_POSIX_HAVE_LSTAT64)
        struct stat64 st = {0};
        if (!lstat64(path, &st))
#else
        struct stat st = {0};
        if (!lstat(path, &st))
#endif
        {
            // get file type
            if (S_ISDIR(st.st_mode))
                info->type = XU_FILE_TYPE_DIRECTORY;
            else
                info->type = XU_FILE_TYPE_FILE;

            // is symlink?
            info->flags = XU_FILE_FLAG_NONE;
            if (S_ISLNK(st.st_mode))
            {
                // we need get more file info about symlink, does it point to directory?
                xu_memset(&st, 0, sizeof(st));
#if defined(XU_CONFIG_POSIX_HAVE_STAT64)
                if (!stat64(path, &st))
#else
                if (!stat(path, &st))
#endif
                {
                    if (S_ISDIR(st.st_mode))
                        info->type = XU_FILE_TYPE_DIRECTORY;
                    else
                        info->type = XU_FILE_TYPE_FILE;
                }
                info->flags |= XU_FILE_FLAG_LINK;
            }

            // file size
            info->size = st.st_size >= 0 ? (xu_hize_t)st.st_size : 0;

            // the last access time
            info->atime = (xu_time_t)st.st_atime;

            // the last modify time
            info->mtime = (xu_time_t)st.st_mtime;
            return xu_true;
        }
    }
    else if (!access(path, F_OK))
    {
        return xu_true;
    }
    return xu_false;
}
#ifndef XU_CONFIG_MICRO_ENABLE
xu_long_t xu_file_pread(xu_file_ref_t file, xu_byte_t* data, xu_size_t size, xu_hize_t offset)
{
    // check
    xu_assert_and_check_return_val(file, -1);

    // read it
#    ifdef XU_CONFIG_POSIX_HAVE_PREAD64
    return pread64(xu_file2fd(file), data, (size_t)size, offset);
#    else
    return pread(xu_file2fd(file), data, (size_t)size, offset);
#    endif
}
xu_long_t xu_file_pwrit(xu_file_ref_t file, xu_byte_t const* data, xu_size_t size, xu_hize_t offset)
{
    // check
    xu_assert_and_check_return_val(file, -1);

    // writ it
#    ifdef XU_CONFIG_POSIX_HAVE_PWRITE64
    return pwrite64(xu_file2fd(file), data, (size_t)size, offset);
#    else
    return pwrite(xu_file2fd(file), data, (size_t)size, offset);
#    endif
}
xu_long_t xu_file_readv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(file && list && size, -1);

    // check iovec
    xu_assert_static(sizeof(xu_iovec_t) == sizeof(struct iovec));
    xu_assert(xu_memberof_eq(xu_iovec_t, data, struct iovec, iov_base));
    xu_assert(xu_memberof_eq(xu_iovec_t, size, struct iovec, iov_len));

    // read it
    return readv(xu_file2fd(file), (struct iovec const*)list, size);
}
xu_long_t xu_file_writv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(file && list && size, -1);

    // check iovec
    xu_assert_static(sizeof(xu_iovec_t) == sizeof(struct iovec));
    xu_assert(xu_memberof_eq(xu_iovec_t, data, struct iovec, iov_base));
    xu_assert(xu_memberof_eq(xu_iovec_t, size, struct iovec, iov_len));

    // writ it
    return writev(xu_file2fd(file), (struct iovec const*)list, size);
}
xu_hong_t xu_file_writf(xu_file_ref_t file, xu_file_ref_t ifile, xu_hize_t offset, xu_hize_t size)
{
    // check
    xu_assert_and_check_return_val(file && ifile && size, -1);

#    ifdef XU_CONFIG_POSIX_HAVE_SENDFILE

    // writ it
    off_t     seek = offset;
    xu_hong_t real = sendfile(xu_file2fd(file), xu_file2fd(ifile), &seek, (size_t)size);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;

#    else

    // read data
    xu_byte_t data[8192];
    xu_long_t read = xu_file_pread(ifile, data, sizeof(data), offset);
    xu_check_return_val(read > 0, read);

    // writ data
    xu_size_t writ = 0;
    while (writ < read)
    {
        xu_long_t real = xu_file_writ(file, data + writ, read - writ);
        if (real > 0)
            writ += real;
        else
            break;
    }

    // ok?
    return writ == read ? writ : -1;
#    endif
}
xu_long_t xu_file_preadv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size, xu_hize_t offset)
{
    // check
    xu_assert_and_check_return_val(file && list && size, -1);

    // check iovec
    xu_assert_static(sizeof(xu_iovec_t) == sizeof(struct iovec));
    xu_assert(xu_memberof_eq(xu_iovec_t, data, struct iovec, iov_base));
    xu_assert(xu_memberof_eq(xu_iovec_t, size, struct iovec, iov_len));

    // read it
#    ifdef XU_CONFIG_POSIX_HAVE_PREADV
    return preadv(xu_file2fd(file), (struct iovec const*)list, size, offset);
#    else

    // FIXME: lock it

    // save offset
    xu_hong_t current = xu_file_offset(file);
    xu_assert_and_check_return_val(current >= 0, -1);

    // seek it
    if (current != offset && xu_file_seek(file, offset, XU_FILE_SEEK_BEG) != offset) return -1;

    // read it
    xu_long_t real = xu_file_readv(file, list, size);

    // restore offset
    if (current != offset && xu_file_seek(file, current, XU_FILE_SEEK_BEG) != current) return -1;

    // ok
    return real;
#    endif
}
xu_long_t xu_file_pwritv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size, xu_hize_t offset)
{
    // check
    xu_assert_and_check_return_val(file && list && size, -1);

    // check iovec
    xu_assert_static(sizeof(xu_iovec_t) == sizeof(struct iovec));
    xu_assert(xu_memberof_eq(xu_iovec_t, data, struct iovec, iov_base));
    xu_assert(xu_memberof_eq(xu_iovec_t, size, struct iovec, iov_len));

    // writ it
#    ifdef XU_CONFIG_POSIX_HAVE_PWRITEV
    return pwritev(xu_file2fd(file), (struct iovec const*)list, size, offset);
#    else

    // FIXME: lock it

    // save offset
    xu_hong_t current = xu_file_offset(file);
    xu_assert_and_check_return_val(current >= 0, -1);

    // seek it
    if (current != offset && xu_file_seek(file, offset, XU_FILE_SEEK_BEG) != offset) return -1;

    // writ it
    xu_long_t real = xu_file_writv(file, list, size);

    // restore offset
    if (current != offset && xu_file_seek(file, current, XU_FILE_SEEK_BEG) != current) return -1;

    // ok
    return real;
#    endif
}
xu_bool_t xu_file_copy(xu_char_t const* path, xu_char_t const* dest, xu_size_t flags)
{
    // check
    xu_assert_and_check_return_val(path && dest, xu_false);

    // the full path
    xu_char_t data[XU_PATH_MAXN];
    path = xu_path_absolute(path, data, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // copy link
    xu_file_info_t info = {0};
    if (flags & XU_FILE_COPY_LINK && xu_file_info(path, &info) && info.flags & XU_FILE_FLAG_LINK)
    {
        // read link first
        xu_char_t        srcpath[XU_PATH_MAXN];
        xu_long_t        size     = readlink(path, srcpath, XU_PATH_MAXN);
        xu_char_t const* linkpath = srcpath;
        if (size == XU_PATH_MAXN)
        {
            xu_size_t  maxn = XU_PATH_MAXN * 2;
            xu_char_t* buff = (xu_char_t*)xu_malloc(maxn);
            if (buff)
            {
                xu_long_t size = readlink(path, buff, maxn);
                if (size > 0 && size < maxn)
                {
                    buff[size] = '\0';
                    linkpath   = buff;
                }
            }
        }
        else if (size >= 0 && size < XU_PATH_MAXN)
            srcpath[size] = '\0';

        // do link
        xu_bool_t ok = xu_file_link(linkpath, dest);

        // free link path
        if (linkpath && linkpath != srcpath)
        {
            xu_free((xu_pointer_t)linkpath);
            linkpath = xu_null;
        }
        return ok;
    }

#    ifdef XU_CONFIG_POSIX_HAVE_COPYFILE

    // the dest path
    xu_char_t full1[XU_PATH_MAXN];
    dest = xu_path_absolute(dest, full1, XU_PATH_MAXN);
    xu_assert_and_check_return_val(dest, xu_false);

    // attempt to copy it directly
    if (!copyfile(path, dest, 0, COPYFILE_ALL))
        return xu_true;
    else if (errno != EPERM && errno != EACCES)
    {
        // attempt to copy it again after creating directory
        xu_char_t dir[XU_PATH_MAXN];
        xu_int_t  errno_bak = errno;
        if (xu_directory_create(xu_path_directory(dest, dir, sizeof(dir))))
            return !copyfile(path, dest, 0, COPYFILE_ALL);
        else
            errno = errno_bak;
    }

    // failed
    return xu_false;
#    else
    xu_int_t  ifd = -1;
    xu_int_t  ofd = -1;
    xu_bool_t ok  = xu_false;
    do
    {
        // get stat.st_mode first
#        ifdef XU_CONFIG_POSIX_HAVE_STAT64
        struct stat64 st = {0};
        if (stat64(path, &st)) break;
#        else
        struct stat st = {0};
        if (stat(path, &st)) break;
#        endif

        // open source file
        ifd = open(path, O_RDONLY);
        xu_check_break(ifd >= 0);

        // get the absolute source path
        dest = xu_path_absolute(dest, data, sizeof(data));
        xu_assert_and_check_break(dest);

        // open destinate file and copy file mode
        ofd = open(dest, O_RDWR | O_CREAT | O_TRUNC, st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        if (ofd < 0 && (errno != EPERM && errno != EACCES))
        {
            // attempt to open it again after creating directory
            xu_int_t  errno_bak = errno;
            xu_char_t dir[XU_PATH_MAXN];
            if (xu_directory_create(xu_path_directory(dest, dir, sizeof(dir))))
                ofd = open(dest, O_RDWR | O_CREAT | O_TRUNC, st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
            else
                errno = errno_bak;
        }
        xu_check_break(ofd >= 0);

        // get file size
        xu_hize_t size = xu_file_size(xu_fd2file(ifd));

        // init write size
        xu_hize_t writ = 0;

        // attempt to copy file using `sendfile`
#        ifdef XU_CONFIG_POSIX_HAVE_SENDFILE
        while (writ < size)
        {
            off_t     seek = writ;
            xu_hong_t real = sendfile(ofd, ifd, &seek, (size_t)(size - writ));
            if (real > 0)
                writ += real;
            else
                break;
        }

        /* attempt to copy file directly if sendfile failed
         *
         * sendfile() supports regular file only after "since Linux 2.6.33".
         */
        if (writ != size)
        {
            lseek(ifd, 0, SEEK_SET);
            lseek(ofd, 0, SEEK_SET);
        }
        else
        {
            ok = xu_true;
            break;
        }
#        endif

        // copy file using `read` and `write`
        writ = 0;
        while (writ < size)
        {
            // read some data
            xu_int_t real = read(ifd, data, (size_t)xu_min(size - writ, sizeof(data)));
            if (real > 0)
            {
                real = write(ofd, data, real);
                if (real > 0)
                    writ += real;
                else
                    break;
            }
            else
                break;
        }

        // ok?
        ok = (writ == size);

    } while (0);

    // close source file
    if (ifd >= 0) close(ifd);
    ifd = -1;

    // close destinate file
    if (ofd >= 0) close(ofd);
    ofd = -1;

    // ok?
    return ok;
#    endif
}
xu_bool_t xu_file_create(xu_char_t const* path)
{
    // check
    xu_assert_and_check_return_val(path, xu_false);

    // make it
    xu_file_ref_t file = xu_file_init(path, XU_FILE_MODE_CREAT | XU_FILE_MODE_WO | XU_FILE_MODE_TRUNC);
    if (file) xu_file_exit(file);

    // ok?
    return file ? xu_true : xu_false;
}
xu_bool_t xu_file_remove(xu_char_t const* path)
{
    // check
    xu_assert_and_check_return_val(path, xu_false);

    // the full path
    xu_char_t full[XU_PATH_MAXN];
    path = xu_path_absolute(path, full, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // remove it
    return !remove(path) ? xu_true : xu_false;
}
xu_bool_t xu_file_rename(xu_char_t const* path, xu_char_t const* dest)
{
    // check
    xu_assert_and_check_return_val(path && dest, xu_false);

    // the full path
    xu_char_t full0[XU_PATH_MAXN];
    path = xu_path_absolute(path, full0, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // the dest path
    xu_char_t full1[XU_PATH_MAXN];
    dest = xu_path_absolute(dest, full1, XU_PATH_MAXN);
    xu_assert_and_check_return_val(dest, xu_false);

    // attempt to rename it directly
    if (!rename(path, dest))
        return xu_true;
    else if (errno != EPERM && errno != EACCES)
    {
        // attempt to rename it again after creating directory
        xu_int_t  errno_bak = errno;
        xu_char_t dir[XU_PATH_MAXN];
        if (xu_directory_create(xu_path_directory(dest, dir, sizeof(dir))))
            return !rename(path, dest);
        else
            errno = errno_bak;
    }
    return xu_false;
}
xu_bool_t xu_file_link(xu_char_t const* path, xu_char_t const* dest)
{
    // check
    xu_assert_and_check_return_val(path && dest, xu_false);

    // the dest path
    xu_char_t full1[XU_PATH_MAXN];
    dest = xu_path_absolute(dest, full1, XU_PATH_MAXN);
    xu_assert_and_check_return_val(dest, xu_false);

    // attempt to link it directly
    // @note we should not use absolute path, dest -> path (may be relative path)
    if (!symlink(path, dest))
        return xu_true;
    else if (errno != EPERM && errno != EACCES)
    {
        // attempt to link it again after creating directory
        xu_int_t  errno_bak = errno;
        xu_char_t dir[XU_PATH_MAXN];
        if (xu_directory_create(xu_path_directory(dest, dir, sizeof(dir))))
            return !symlink(path, dest);
        else
            errno = errno_bak;
    }
    return xu_false;
}
xu_bool_t xu_file_access(xu_char_t const* path, xu_size_t mode)
{
    // check
    xu_assert_and_check_return_val(path, xu_false);

    // the full path
    xu_char_t full[XU_PATH_MAXN];
    path = xu_path_absolute(path, full, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // flags
    xu_size_t flags = 0;
    if (mode & XU_FILE_MODE_RW)
        flags = R_OK | W_OK;
    else
    {
        if (mode & XU_FILE_MODE_RO) flags |= R_OK;
        if (mode & XU_FILE_MODE_WO) flags |= W_OK;
    }
    if (mode & XU_FILE_MODE_EXEC) flags |= X_OK;

    return !access(full, flags);
}
xu_bool_t xu_file_touch(xu_char_t const* path, xu_time_t atime, xu_time_t mtime)
{
    // check
    xu_assert_and_check_return_val(path, xu_false);

    // the full path
    xu_char_t full[XU_PATH_MAXN];
    path = xu_path_absolute(path, full, XU_PATH_MAXN);
    xu_assert_and_check_return_val(path, xu_false);

    // file exists?
    xu_bool_t       ok = xu_false;
    struct timespec ts[2];
    xu_memset(ts, 0, sizeof(ts));
    if (!access(path, F_OK))
    {
        if (atime > 0 || mtime > 0)
        {
#    ifdef XU_CONFIG_POSIX_HAVE_UTIMENSAT
            if (atime > 0)
                ts[0].tv_sec = atime;
            else
                ts[0].tv_nsec = UTIME_OMIT;
            if (mtime > 0)
                ts[1].tv_sec = mtime;
            else
                ts[1].tv_nsec = UTIME_OMIT;
            ok = !utimensat(AT_FDCWD, path, ts, 0);
#    endif
        }
        else
            ok = xu_true;
    }
    else
    {
        // create a new file if not exists
        xu_file_ref_t file = xu_file_init(path, XU_FILE_MODE_RW | XU_FILE_MODE_CREAT);
        if (file)
        {
            if (atime > 0 || mtime > 0)
            {
#    ifdef XU_CONFIG_POSIX_HAVE_FUTIMENS
                if (atime > 0)
                    ts[0].tv_sec = atime;
                else
                    ts[0].tv_nsec = UTIME_OMIT;
                if (mtime > 0)
                    ts[1].tv_sec = mtime;
                else
                    ts[1].tv_nsec = UTIME_OMIT;
                ok = !futimens(xu_file2fd(file), ts);
#    endif
            }
            else
                ok = xu_true;
            xu_file_exit(file);
        }
    }
    return ok;
}
#endif
