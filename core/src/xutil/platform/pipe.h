#pragma once

#include "xutil/platform/socket.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 */

// the pipe file ref type
typedef __xu_typeref__(pipe_file);

// the pipe mode type
typedef enum __xu_pipe_mode_t
{
    XU_PIPE_MODE_RO = 1 //!< read only
    ,
    XU_PIPE_MODE_WO = 2 //!< write only
    ,
    XU_PIPE_MODE_BLOCK = 4 //!< block mode

} xu_pipe_mode_t;

// the pipe file event enum
typedef enum __xu_event_event_e
{
    XU_PIPE_EVENT_NONE = XU_SOCKET_EVENT_NONE,
    XU_PIPE_EVENT_CONN = XU_SOCKET_EVENT_CONN,
    XU_PIPE_EVENT_READ = XU_SOCKET_EVENT_RECV,
    XU_PIPE_EVENT_WRIT = XU_SOCKET_EVENT_SEND

} xu_pipe_event_e;

/* *******************************************************
 * interfaces
 */

/*! init the file of named pipe
 *
 * @note the pipe files will be inherited in child process and it will be blocked.
 *
 * @param name          the pipe name
 * @param mode          the pipe mode
 * @param buffer_size   the buffer size of pipe, it will use the default size if pass zero
 *
 * @return              the pipe file
 */
xu_pipe_file_ref_t xu_pipe_file_init(xu_char_t const* name, xu_size_t mode, xu_size_t buffer_size);

/*! init the file pair of anonymous pipe
 *
 * @note the pipe files will be inherited in child process.
 *
 * @param pair          the pipe file pair, read: pair[0], write: pair[1]
 * @param mode          the pipe mode pair, only support XU_PIPE_MODE_BLOCK
 * @param buffer_size   the buffer size of pipe, it will use the default size if pass zero
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_pipe_file_init_pair(xu_pipe_file_ref_t pair[2], xu_size_t mode[2], xu_size_t buffer_size);

/*! exit the pipe file
 *
 * @param file          the pipe file
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_pipe_file_exit(xu_pipe_file_ref_t file);

/*! connect the given named pipe file on server side
 *
 * @param file          the pipe file
 *
 * @return              ok: 1, continue: 0; failed: -1
 */
xu_long_t xu_pipe_file_connect(xu_pipe_file_ref_t file);

/*! read the pipe file data (non-block)
 *
 * @param file          the pipe file
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
xu_long_t xu_pipe_file_read(xu_pipe_file_ref_t file, xu_byte_t* data, xu_size_t size);

/*! writ the pipe file data (non-block)
 *
 * @param file          the file
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
xu_long_t xu_pipe_file_write(xu_pipe_file_ref_t file, xu_byte_t const* data, xu_size_t size);

/*! wait the pipe file events
 *
 * @param file          the file
 * @param events        the pipe file events
 * @param timeout       the timeout, infinity: -1
 *
 * @return              > 0: the events code, 0: timeout, -1: failed
 */
xu_long_t xu_pipe_file_wait(xu_pipe_file_ref_t file, xu_size_t events, xu_long_t timeout);

/*! read the pipe file data for tcp with block mode
 *
 * @param file          the pipe file
 * @param data          the data
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_pipe_file_bread(xu_pipe_file_ref_t file, xu_byte_t* data, xu_size_t size);

/*! writ the pipe file data for tcp with block mode
 *
 * @param file          the pipe file
 * @param data          the data
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_pipe_file_bwrit(xu_pipe_file_ref_t file, xu_byte_t const* data, xu_size_t size);
