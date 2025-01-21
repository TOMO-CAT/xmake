#pragma once

#include "xutil/prefix.h"

// the queue buffer type
typedef struct __xu_queue_buffer_t
{
    // the buffer data
    xu_byte_t* data;

    // the buffer head
    xu_byte_t* head;

    // the buffer size
    xu_size_t size;

    // the buffer maxn
    xu_size_t maxn;

} xu_queue_buffer_t, *xu_queue_buffer_ref_t;

/*! init buffer
 *
 * @param buffer    the buffer
 * @param maxn      the buffer maxn
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_queue_buffer_init(xu_queue_buffer_ref_t buffer, xu_size_t maxn);

/*! exit buffer
 *
 * @param buffer    the buffer
 */
xu_void_t xu_queue_buffer_exit(xu_queue_buffer_ref_t buffer);

/*! the buffer data
 *
 * @param buffer    the buffer
 *
 * @return          the buffer data
 */
xu_byte_t* xu_queue_buffer_data(xu_queue_buffer_ref_t buffer);

/*! the buffer head
 *
 * @param buffer    the buffer
 *
 * @return          the buffer head
 */
xu_byte_t* xu_queue_buffer_head(xu_queue_buffer_ref_t buffer);

/*! the buffer tail
 *
 * @param buffer    the buffer
 *
 * @return          the buffer tail
 */
xu_byte_t* xu_queue_buffer_tail(xu_queue_buffer_ref_t buffer);

/*! the buffer maxn
 *
 * @param buffer    the buffer
 *
 * @return          the buffer maxn
 */
xu_size_t xu_queue_buffer_maxn(xu_queue_buffer_ref_t buffer);

/*! the buffer size
 *
 * @param buffer    the buffer
 *
 * @return          the buffer size
 */
xu_size_t xu_queue_buffer_size(xu_queue_buffer_ref_t buffer);

/*! the buffer left
 *
 * @param buffer    the buffer
 *
 * @return          the buffer left
 */
xu_size_t xu_queue_buffer_left(xu_queue_buffer_ref_t buffer);

/*! the buffer full?
 *
 * @param buffer    the buffer
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_queue_buffer_full(xu_queue_buffer_ref_t buffer);

/*! the buffer null?
 *
 * @param buffer    the buffer
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_queue_buffer_null(xu_queue_buffer_ref_t buffer);

/*! clear buffer
 *
 * @param buffer    the buffer
 */
xu_void_t xu_queue_buffer_clear(xu_queue_buffer_ref_t buffer);

/*! resize buffer size
 *
 * @param buffer    the buffer
 * @param maxn      the buffer maxn
 *
 * @return          the buffer data
 */
xu_byte_t* xu_queue_buffer_resize(xu_queue_buffer_ref_t buffer, xu_size_t maxn);

/*! skip buffer
 *
 * @param buffer    the buffer
 * @param size      the skiped size
 *
 * @return          the real size
 */
xu_long_t xu_queue_buffer_skip(xu_queue_buffer_ref_t buffer, xu_size_t size);

/*! read buffer
 *
 * @param buffer    the buffer
 * @param data      the data
 * @param size      the size
 *
 * @return          the real size
 */
xu_long_t xu_queue_buffer_read(xu_queue_buffer_ref_t buffer, xu_byte_t* data, xu_size_t size);

/*! writ buffer
 *
 * @param buffer    the buffer
 * @param data      the data
 * @param size      the size
 *
 * @return          the real size
 */
xu_long_t xu_queue_buffer_writ(xu_queue_buffer_ref_t buffer, xu_byte_t const* data, xu_size_t size);

/*! init pull buffer for reading
 *
 * @param buffer    the buffer
 * @param size      the size
 *
 * @return          the data
 */
xu_byte_t* xu_queue_buffer_pull_init(xu_queue_buffer_ref_t buffer, xu_size_t* size);

/*! exit pull buffer for reading
 *
 * @param buffer    the buffer
 * @param size      the size
 */
xu_void_t xu_queue_buffer_pull_exit(xu_queue_buffer_ref_t buffer, xu_size_t size);

/*! init push buffer for writing
 *
 * @param buffer    the buffer
 * @param size      the size
 *
 * @return          the data
 */
xu_byte_t* xu_queue_buffer_push_init(xu_queue_buffer_ref_t buffer, xu_size_t* size);

/*! exit push buffer for writing
 *
 * @param buffer    the buffer
 * @param size      the size
 */
xu_void_t xu_queue_buffer_push_exit(xu_queue_buffer_ref_t buffer, xu_size_t size);
