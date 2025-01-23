#pragma once

#include "xutil/platform/file.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 */

// the directory walk code
typedef enum __xu_directory_walk_code_e
{
    XU_DIRECTORY_WALK_CODE_END            = 0,
    XU_DIRECTORY_WALK_CODE_CONTINUE       = 1,
    XU_DIRECTORY_WALK_CODE_SKIP_RECURSION = 2 //!< continue and skip sub-directory, only for prefix recursion mode
} xu_directory_walk_code_e;

/*! the directory walk func type
 *
 * @param path          the file path
 * @param info          the file info
 * @param priv          the user private data
 *
 * @return              continue: 1 skip-recursion: 2, break: 0
 */
typedef xu_long_t (*xu_directory_walk_func_t)(xu_char_t const* path, xu_file_info_t const* info, xu_cpointer_t priv);

/* *******************************************************
 * interfaces
 */

/*! create the directory
 *
 * @param path          the directory path
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_directory_create(xu_char_t const* path);

/*! remove the directory
 *
 * @param path          the directory path
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_directory_remove(xu_char_t const* path);

/*! the home directory
 *
 * @param path          the directory path data
 * @param maxn          the directory path maxn
 *
 * @return              the directory path size
 */
xu_size_t xu_directory_home(xu_char_t* path, xu_size_t maxn);

/*! the current directory
 *
 * @param path          the directory path data
 * @param maxn          the directory path maxn
 *
 * @return              the directory path size
 */
xu_size_t xu_directory_current(xu_char_t* path, xu_size_t maxn);

/*! set the current directory
 *
 * @param path          the directory path
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_directory_current_set(xu_char_t const* path);

/*! the temporary directory
 *
 * @param path          the directory path data
 * @param maxn          the directory path maxn
 *
 * @return              the directory path size
 */
xu_size_t xu_directory_temporary(xu_char_t* path, xu_size_t maxn);

/*! the directory walk
 *
 * @param path          the directory path
 * @param recursion     the recursion level, 0, 1, 2, .. or -1 (infinite)
 * @param prefix        is prefix recursion? directory is the first item
 * @param func          the callback func
 * @param priv          the callback priv
 *
 */
xu_void_t xu_directory_walk(xu_char_t const* path, xu_long_t recursion, xu_bool_t prefix, xu_directory_walk_func_t func,
                            xu_cpointer_t priv);

/*! copy directory
 *
 * @param path          the directory path
 * @param dest          the directory dest
 * @param flags         the copy flags, e.g. XU_FILE_COPY_LINK
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_directory_copy(xu_char_t const* path, xu_char_t const* dest, xu_size_t flags);
