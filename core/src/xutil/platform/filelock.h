#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * types
 */

// the file lock ref type
typedef __xu_typeref__(filelock);

// the file lock mode type
typedef enum __xu_filelock_mode_e
{
    XU_FILELOCK_MODE_NONE = 0 //!< none
    ,
    XU_FILELOCK_MODE_EX = 1 //!< exclusive lock mode
    ,
    XU_FILELOCK_MODE_SH = 2 //!< share lock mode

} xu_filelock_mode_t;

/* *******************************************************
 * interfaces
 */

/*! init the file lock
 *
 * @param file      the file reference
 *
 * @return          the file lock
 */
xu_filelock_ref_t xu_filelock_init(xu_file_ref_t file);

/*! init the file lock from the file path
 *
 * @param path      the file path
 * @param mode      the file mode
 *
 * @return          the file lock
 */
xu_filelock_ref_t xu_filelock_init_from_path(xu_char_t const* path, xu_size_t mode);

/*! exit the file lock
 *
 * @param lock      the file lock
 */
xu_void_t xu_filelock_exit(xu_filelock_ref_t lock);

/*! enter the file lock (block)
 *
 * @param lock      the file lock
 * @param mode      the lock mode
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_filelock_enter(xu_filelock_ref_t lock, xu_size_t mode);

/*! try to enter the file lock
 *
 * @param lock      the file lock
 * @param mode      the lock mode
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_filelock_enter_try(xu_filelock_ref_t lock, xu_size_t mode);

/*! leave the file lock
 *
 * @param lock      the file lock
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_filelock_leave(xu_filelock_ref_t lock);
