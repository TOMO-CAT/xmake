#pragma once

#include "xutil/memory/queue_buffer.h"
#include "xutil/stream/prefix.h"
#include "xutil/stream/stream.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// cast stream
#define xu_stream_cast(stream) ((stream) ? &(((xu_stream_t*)(stream))[-1]) : xu_null)

/* *******************************************************
 * types
 * *******************************************************
 */

// the stream type
typedef struct __xu_stream_t
{
    // the stream type
    xu_uint8_t type;

    // is writted?
    xu_uint8_t bwritted;

    // the url
    xu_url_t url;

    /* the internal state for killing stream in the other thread
     *
     * <pre>
     * XU_STATE_CLOSED
     * XU_STATE_OPENED
     * XU_STATE_KILLED
     * XU_STATE_OPENING
     * XU_STATE_KILLING
     * </pre>
     */
    xu_atomic32_t istate;

    // the timeout
    xu_long_t timeout;

    /* the stream state
     *
     * <pre>
     * XU_STATE_KILLED
     * XU_STATE_WAIT_FAILED
     * </pre>
     */
    xu_size_t state;

    // the offset
    xu_hize_t offset;

    // the cache
    xu_queue_buffer_t cache;

    // wait
    xu_long_t (*wait)(xu_stream_ref_t stream, xu_size_t wait, xu_long_t timeout);

    // open
    xu_bool_t (*open)(xu_stream_ref_t stream);

    // clos
    xu_bool_t (*clos)(xu_stream_ref_t stream);

    // read
    xu_long_t (*read)(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size);

    // writ
    xu_long_t (*writ)(xu_stream_ref_t stream, xu_byte_t const* data, xu_size_t size);

    // seek
    xu_bool_t (*seek)(xu_stream_ref_t stream, xu_hize_t offset);

    // sync
    xu_bool_t (*sync)(xu_stream_ref_t stream, xu_bool_t bclosing);

    // ctrl
    xu_bool_t (*ctrl)(xu_stream_ref_t stream, xu_size_t ctrl, xu_va_list_t args);

    // exit
    xu_void_t (*exit)(xu_stream_ref_t stream);

    // kill
    xu_void_t (*kill)(xu_stream_ref_t stream);

} xu_stream_t;
