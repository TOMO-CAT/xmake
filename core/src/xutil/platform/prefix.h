#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */

// fd to file
#define xu_fd2file(fd) ((fd) >= 0 ? (xu_file_ref_t)((xu_long_t)(fd) + 1) : xu_null)

// file to fd
#define xu_file2fd(file) (xu_int_t)((file) ? (((xu_long_t)(file)) - 1) : -1)

// fd to sock
#define xu_fd2sock(fd) ((fd) >= 0 ? (xu_socket_ref_t)((xu_long_t)(fd) + 1) : xu_null)

// sock to fd
#define xu_sock2fd(sock) (xu_int_t)((sock) ? (((xu_long_t)(sock)) - 1) : -1)

/* *******************************************************
 *  types
 * *******************************************************
 */
/*! the iovec size type
 *
 */
typedef xu_size_t xu_iovec_size_t;

/// the iovec type for readv, preadv, writv, pwritv, recvv, sendv
typedef struct __xu_iovec_t
{
    /// the data
    xu_byte_t* data;

    /// the size
    xu_iovec_size_t size;

} xu_iovec_t;
