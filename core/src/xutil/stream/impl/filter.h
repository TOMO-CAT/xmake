#pragma once

#include "xutil/memory/buffer.h"
#include "xutil/memory/queue_buffer.h"
#include "xutil/prefix.h"
#include "xutil/stream/static_stream.h"

/* *******************************************************
 * types
 */
// the filter type
typedef struct __xu_filter_t
{
    // the type
    xu_size_t type;

    // the input is eof?
    xu_bool_t beof;

    // is opened?
    xu_bool_t bopened;

    // the input limit size
    xu_hong_t limit;

    // the input offset
    xu_hize_t offset;

    // the input data
    xu_buffer_t idata;

    // the output data
    xu_queue_buffer_t odata;

    // the open
    xu_bool_t (*open)(struct __xu_filter_t* filter);

    // the clos
    xu_void_t (*clos)(struct __xu_filter_t* filter);

    // the spak
    xu_long_t (*spak)(struct __xu_filter_t* filter, xu_static_stream_ref_t istream, xu_static_stream_ref_t ostream,
                      xu_long_t sync);

    // the ctrl
    xu_bool_t (*ctrl)(struct __xu_filter_t* filter, xu_size_t ctrl, xu_va_list_t args);

    // the exit
    xu_void_t (*exit)(struct __xu_filter_t* filter);

} xu_filter_t;

/* *******************************************************
 * interfaces
 */
static __xu_inline__ xu_bool_t xu_filter_init(xu_filter_t* filter, xu_size_t type)
{
    // check
    xu_assert_and_check_return_val(filter, xu_false);

    // init type
    filter->type = type;

    // init the input eof
    filter->beof = xu_false;

    // init input limit size
    filter->limit = -1;

    // init the input offset
    filter->offset = 0;

    // init idata
    if (!xu_buffer_init(&filter->idata)) return xu_false;

    // init odata
    if (!xu_queue_buffer_init(&filter->odata, 8192)) return xu_false;

    // ok
    return xu_true;
}
