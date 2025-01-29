#include "xutil/platform/filelock.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"
#include "xutil/platform/file.h"

#include <fcntl.h>
#include <unistd.h>

/* *******************************************************
 * types
 */
// the file lock type
typedef struct __xu_filelock_t
{
    // the file reference
    xu_file_ref_t file;

    // is owner?
    xu_bool_t owner;

} xu_filelock_t;

/* *******************************************************
 * private implementation
 */
static xu_filelock_ref_t xu_filelock_init_impl(xu_file_ref_t file, xu_bool_t owner)
{
    // check
    xu_assert_and_check_return_val(file, xu_null);

    xu_filelock_t* lock = xu_null;
    do
    {
        // init lock
        lock = xu_malloc0_type(xu_filelock_t);
        xu_assert_and_check_break(lock);

        lock->file  = file;
        lock->owner = owner;

    } while (0);
    return (xu_filelock_ref_t)lock;
}

/* *******************************************************
 * implementation
 */
xu_filelock_ref_t xu_filelock_init(xu_file_ref_t file) { return xu_filelock_init_impl(file, xu_false); }
xu_filelock_ref_t xu_filelock_init_from_path(xu_char_t const* path, xu_size_t mode)
{
    xu_assert_and_check_return_val(path, xu_null);
    return xu_filelock_init_impl(xu_file_init(path, mode), xu_true);
}
xu_void_t xu_filelock_exit(xu_filelock_ref_t self)
{
    // check
    xu_filelock_t* lock = (xu_filelock_t*)self;
    xu_assert_and_check_return(lock);

    // exit file
    if (lock->file && lock->owner) xu_file_exit(lock->file);
    lock->file = xu_null;

    // exit lock
    xu_free(lock);
}
xu_bool_t xu_filelock_enter(xu_filelock_ref_t self, xu_size_t mode)
{
    // check
    xu_filelock_t* lock = (xu_filelock_t*)self;
    xu_assert_and_check_return_val(lock && lock->file, xu_false);

    // lock it
    struct flock flk = {0};
    flk.l_type       = (mode == XU_FILELOCK_MODE_EX) ? F_WRLCK : F_RDLCK;
    flk.l_start      = 0;
    flk.l_whence     = SEEK_SET;
    flk.l_len        = 0;
    flk.l_pid        = getpid();
    return fcntl(xu_file2fd(lock->file), F_SETLKW, &flk) == 0;
}
xu_bool_t xu_filelock_enter_try(xu_filelock_ref_t self, xu_size_t mode)
{
    // check
    xu_filelock_t* lock = (xu_filelock_t*)self;
    xu_assert_and_check_return_val(lock && lock->file, xu_false);

    // try to lock it
    struct flock flk = {0};
    flk.l_type       = (mode == XU_FILELOCK_MODE_EX) ? F_WRLCK : F_RDLCK;
    flk.l_start      = 0;
    flk.l_whence     = SEEK_SET;
    flk.l_len        = 0;
    flk.l_pid        = getpid();
    return fcntl(xu_file2fd(lock->file), F_SETLK, &flk) == 0;
}
xu_bool_t xu_filelock_leave(xu_filelock_ref_t self)
{
    // check
    xu_filelock_t* lock = (xu_filelock_t*)self;
    xu_assert_and_check_return_val(lock && lock->file, xu_false);

    // unlock it
    struct flock flk = {0};
    flk.l_type       = F_UNLCK;
    flk.l_start      = 0;
    flk.l_whence     = SEEK_SET;
    flk.l_len        = 0;
    flk.l_pid        = getpid();
    return fcntl(xu_file2fd(lock->file), F_SETLK, &flk) == 0;
}
