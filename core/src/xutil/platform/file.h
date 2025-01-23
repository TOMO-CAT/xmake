#pragma once

#include "xutil/macros/type.h"
#include "xutil/platform/prefix.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 */

// the aligned size for direct mode
#define XU_FILE_DIRECT_ASIZE (512)

// the cached size for direct mode
#ifdef __xu_small__
#    define XU_FILE_DIRECT_CSIZE (1 << 14)
#else
#    define XU_FILE_DIRECT_CSIZE (1 << 17)
#endif

/* *******************************************************
 *  types
 */
// the file mode type
typedef enum __xu_file_mode_e
{
    XU_FILE_MODE_RO = 1 //!< read only
    ,
    XU_FILE_MODE_WO = 2 //!< write only
    ,
    XU_FILE_MODE_RW = 4 //!< read and write
    ,
    XU_FILE_MODE_CREAT = 8 //!< create
    ,
    XU_FILE_MODE_APPEND = 16 //!< append
    ,
    XU_FILE_MODE_TRUNC = 32 //!< truncate
    ,
    XU_FILE_MODE_DIRECT = 64 //!< direct, no cache, @note data & size must be aligned by XU_FILE_DIRECT_ASIZE
    ,
    XU_FILE_MODE_EXEC = 128 //!< executable, only for xu_file_access, not supported when creating files

} xu_file_mode_e;

// the file seek flag
typedef enum __xu_file_seek_flag_e
{
    XU_FILE_SEEK_BEG = 0,
    XU_FILE_SEEK_CUR = 1,
    XU_FILE_SEEK_END = 2

} xu_file_seek_flag_e;

// the file copy flag
typedef enum __xu_file_copy_flag_e
{
    XU_FILE_COPY_NONE = 0 //!< default: copy symlink as file
    ,
    XU_FILE_COPY_LINK = 1 //!< reserve symlink

} xu_file_copy_flag_e;

// the file type
typedef enum __xu_file_type_e
{
    XU_FILE_TYPE_NONE      = 0,
    XU_FILE_TYPE_DIRECTORY = 1,
    XU_FILE_TYPE_FILE      = 2,
    XU_FILE_TYPE_DOT       = 3,
    XU_FILE_TYPE_DOT2      = 4

} xu_file_type_e;

// the file flag
typedef enum __xu_file_flag_e
{
    XU_FILE_FLAG_NONE = 0,
    XU_FILE_FLAG_LINK = 1 //!< is symlink?

} xu_file_flag_e;

// the file info type
typedef struct __xu_file_info_t
{
    /// the file type
    xu_size_t type : 16;

    /// the file flags
    xu_size_t flags : 16;

    /// the file size
    xu_hize_t size;

    /// the last access time
    xu_time_t atime;

    /// the last modify time
    xu_time_t mtime;

} xu_file_info_t;

/* *******************************************************
 *  interfaces
 */
/*! init the file
 *
 * @param path          the file path
 * @param mode          the file mode
 *
 * @return              the file
 */
xu_file_ref_t xu_file_init(xu_char_t const* path, xu_size_t mode);

/*! exit the file
 *
 * @param file          the file
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_exit(xu_file_ref_t file);

/*! read the file data
 *
 * @param file          the file
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
xu_long_t xu_file_read(xu_file_ref_t file, xu_byte_t* data, xu_size_t size);

/*! writ the file data
 *
 * @param file          the file
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
xu_long_t xu_file_writ(xu_file_ref_t file, xu_byte_t const* data, xu_size_t size);

/*! pread the file data
 *
 * @param file          the file
 * @param data          the data
 * @param size          the size
 * @param offset        the offset, the file offset will not be changed
 *
 * @return              the real size or -1
 */
xu_long_t xu_file_pread(xu_file_ref_t file, xu_byte_t* data, xu_size_t size, xu_hize_t offset);

/*! pwrit the file data
 *
 * @param file          the file
 * @param data          the data
 * @param size          the size
 * @param offset        the offset, the file offset will not be changed
 *
 * @return              the real size or -1
 */
xu_long_t xu_file_pwrit(xu_file_ref_t file, xu_byte_t const* data, xu_size_t size, xu_hize_t offset);

/*! readv the file data
 *
 * @param file          the file
 * @param list          the iovec list
 * @param size          the iovec size
 *
 * @return              the real size or -1
 */
xu_long_t xu_file_readv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size);

/*! writv the file data
 *
 * @param file          the file
 * @param list          the iovec list
 * @param size          the iovec size
 *
 * @return              the real size or -1
 */
xu_long_t xu_file_writv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size);

/*! writf the file data
 *
 * @param file          the file
 * @param ifile         the input file
 * @param offset        the input file offset, the file offset will not be changed
 * @param size          the writted size
 *
 * @return              the real size or -1
 */
xu_hong_t xu_file_writf(xu_file_ref_t file, xu_file_ref_t ifile, xu_hize_t offset, xu_hize_t size);

/*! preadv the file data
 *
 * @param file          the file
 * @param list          the iovec list
 * @param size          the iovec size
 * @param offset        the offset, the file offset will not be changed
 *
 * @return              the real size or -1
 */
xu_long_t xu_file_preadv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size, xu_hize_t offset);

/*! pwritv the file data
 *
 * @param file          the file
 * @param list          the iovec list
 * @param size          the iovec size
 * @param offset        the offset, the file offset will not be changed
 *
 * @return              the real size or -1
 */
xu_long_t xu_file_pwritv(xu_file_ref_t file, xu_iovec_t const* list, xu_size_t size, xu_hize_t offset);

/*! seek the file offset
 *
 * @param file          the file
 * @param offset        the file offset
 * @param mode          the seek mode
 *
 * @return              the real offset or -1
 */
xu_hong_t xu_file_seek(xu_file_ref_t file, xu_hong_t offset, xu_size_t mode);

/*! fsync the file
 *
 * @param file          the file
 */
xu_bool_t xu_file_sync(xu_file_ref_t file);

/*! the file size
 *
 * @param file          the file
 *
 * @return              the file size
 */
xu_hize_t xu_file_size(xu_file_ref_t file);

/*! the file offset
 *
 * @param file          the file
 *
 * @return              the file offset or -1
 */
xu_hong_t xu_file_offset(xu_file_ref_t file);

/*! the file info for file or directory
 *
 * @param path          the file path
 * @param info          the file info
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_info(xu_char_t const* path, xu_file_info_t* info);

/*! is this file path is case sensitive?
 *
 * @param path          the file path, @note We must pass a path to the file that exists
 *
 * @return              1: case sensitive, 0: case insensitive, -1: failed
 */
xu_long_t xu_file_fscase(xu_char_t const* path);

/*! copy the file
 *
 * @param path          the file path
 * @param dest          the dest path
 * @param flags         the copy flags, e.g. XU_FILE_COPY_LINK
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_copy(xu_char_t const* path, xu_char_t const* dest, xu_size_t flags);

/*! create the file
 *
 * @param path          the file path
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_create(xu_char_t const* path);

/*! remove the file
 *
 * @param path          the file path
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_remove(xu_char_t const* path);

/*! rename the file
 *
 * @param path          the source file path
 * @param dest          the destination file path
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_rename(xu_char_t const* path, xu_char_t const* dest);

/*! link the file
 *
 * @param path          the source file path
 * @param dest          the destination file path
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_link(xu_char_t const* path, xu_char_t const* dest);

/*! check whether the file or directory can be accessed
 *
 * @param path          the path of the file or directory
 * @param mode          the required accessing mode
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_access(xu_char_t const* path, xu_size_t mode);

/*! update the file time, it will create a new if file not found
 *
 * @param path          the file path
 * @param atime         the last access time, it will not modify this time if it's zero
 * @param mtime         the last modify time, it will not modify this time if it's zero
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_file_touch(xu_char_t const* path, xu_time_t atime, xu_time_t mtime);
