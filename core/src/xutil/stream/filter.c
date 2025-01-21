/* *******************************************************
 * trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "filter"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/stream/filter.h"
#include "xutil/stream/impl/filter.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_bool_t xu_filter_open(xu_filter_ref_t self)
{
    // check
    xu_filter_t* filter = (xu_filter_t*)self;
    xu_assert_and_check_return_val(filter, xu_false);

    // opened?
    xu_check_return_val(!filter->bopened, xu_true);

    // open it
    filter->bopened = filter->open ? filter->open(filter) : xu_true;

    // ok?
    return filter->bopened;
}
xu_void_t xu_filter_clos(xu_filter_ref_t self)
{
    // check
    xu_filter_t* filter = (xu_filter_t*)self;
    xu_assert_and_check_return(filter);

    // opened?
    xu_check_return(filter->bopened);

    // clos it
    if (filter->clos) filter->clos(filter);

    // clear eof
    filter->beof = xu_false;

    // clear limit
    filter->limit = -1;

    // clear offset
    filter->offset = 0;

    // exit idata
    xu_buffer_clear(&filter->idata);

    // exit odata
    xu_queue_buffer_clear(&filter->odata);

    // closed
    filter->bopened = xu_false;
}
xu_void_t xu_filter_exit(xu_filter_ref_t self)
{
    // check
    xu_filter_t* filter = (xu_filter_t*)self;
    xu_assert_and_check_return(filter);

    // exit it
    if (filter->exit) filter->exit(filter);

    // exit idata
    xu_buffer_exit(&filter->idata);

    // exit odata
    xu_queue_buffer_exit(&filter->odata);

    // free it
    xu_free(filter);
}
xu_bool_t xu_filter_ctrl(xu_filter_ref_t self, xu_size_t ctrl, ...)
{
    // check
    xu_filter_t* filter = (xu_filter_t*)self;
    xu_assert_and_check_return_val(filter && filter->ctrl && ctrl, xu_false);

    // init args
    xu_va_list_t args;
    xu_va_start(args, ctrl);

    // ctrl it
    xu_bool_t ok = filter->ctrl(filter, ctrl, args);

    // exit args
    xu_va_end(args);

    // ok?
    return ok;
}
xu_long_t xu_filter_spak(xu_filter_ref_t self, xu_byte_t const* data, xu_size_t size, xu_byte_t const** pdata,
                         xu_size_t need, xu_long_t sync)
{
    // check
    xu_filter_t* filter = (xu_filter_t*)self;
    xu_assert_and_check_return_val(filter && filter->spak && pdata, -1);

    // init odata
    *pdata = xu_null;

    // save the input offset
    filter->offset += size;

    // eof?
    if (filter->limit >= 0 && filter->offset == filter->limit) filter->beof = xu_true;

    // eof? sync it
    if (filter->beof) sync = -1;

    // the idata
    xu_byte_t const* idata = xu_buffer_data(&filter->idata);
    xu_size_t        isize = xu_buffer_size(&filter->idata);
    if (data && size)
    {
        // append data to cache if have the cache data
        if (idata && isize)
        {
            // trace
            xu_trace_d("[%p]: append idata: %lu", self, size);

            // append data
            idata = xu_buffer_memncat(&filter->idata, data, size);
            isize = xu_buffer_size(&filter->idata);
        }
        // using the data directly if no cache data
        else
        {
            // trace
            xu_trace_d("[%p]: using idata directly: %lu", self, size);

            // using it directly
            idata = data;
            isize = size;
        }
    }
    // sync data if null
    else
    {
        // check sync
        xu_assert_and_check_return_val(sync, 0);
    }

    // the need
    if (!need) need = xu_max(size, xu_queue_buffer_maxn(&filter->odata));
    xu_assert_and_check_return_val(need, -1);

    // init pull
    xu_size_t  omaxn = 0;
    xu_byte_t* odata = xu_queue_buffer_pull_init(&filter->odata, &omaxn);
    if (odata)
    {
        // the osize
        xu_long_t osize = omaxn >= need ? need : 0;

        // exit pull
        if (odata) xu_queue_buffer_pull_exit(&filter->odata, osize > 0 ? osize : 0);

        // enough?
        if (osize > 0)
        {
            // append to the cache if idata is not belong to the cache
            if (size && idata == data) xu_buffer_memncat(&filter->idata, data, size);

            // return it directly
            *pdata = odata;
            return osize;
        }
    }

    // grow odata maxn if not enough
    if (need > xu_queue_buffer_maxn(&filter->odata)) xu_queue_buffer_resize(&filter->odata, need);

    // the odata
    omaxn = 0;
    odata = xu_queue_buffer_push_init(&filter->odata, &omaxn);
    xu_assert_and_check_return_val(odata && omaxn, -1);

    // init stream
    xu_static_stream_t istream = {0};
    xu_static_stream_t ostream = {0};
    if (idata && isize)
    {
        // @note istream maybe null for sync the end data
        if (!xu_static_stream_init(&istream, (xu_byte_t*)idata, isize)) return -1;
    }
    if (!xu_static_stream_init(&ostream, (xu_byte_t*)odata, omaxn)) return -1;

    // trace
    xu_trace_d("[%p]: spak: ileft: %lu, oleft: %lu, offset: %llu, limit: %lld, beof: %d: ..", self,
               xu_buffer_size(&filter->idata), xu_queue_buffer_size(&filter->odata), filter->offset, filter->limit,
               filter->beof);

    // spak data
    xu_long_t osize = filter->spak(filter, &istream, &ostream, sync);

    // eof?
    if (osize < 0) filter->beof = xu_true;

    // no data and eof?
    if (!osize && !xu_static_stream_left(&istream) && filter->beof) osize = -1;

    // eof? sync it
    if (filter->beof) sync = -1;

    // exit odata
    xu_queue_buffer_push_exit(&filter->odata, osize > 0 ? osize : 0);

    // have the left idata?
    xu_size_t left = xu_static_stream_left(&istream);
    if (left)
    {
        // move to the cache head if idata is belong to the cache
        if (idata != data)
        {
            // trace
            xu_trace_d("[%p]: move to the cache head: %lu", self, left);

            xu_buffer_memnmov(&filter->idata, xu_static_stream_offset(&istream), left);
        }
        // append to the cache if idata is not belong to the cache
        else
        {
            // trace
            xu_trace_d("[%p]: append to the cache: %lu", self, left);

            xu_buffer_memncat(&filter->idata, xu_static_stream_pos(&istream), left);
        }
    }
    // clear the cache
    else
        xu_buffer_clear(&filter->idata);

    // init pull
    omaxn = 0;
    odata = xu_queue_buffer_pull_init(&filter->odata, &omaxn);

    // no sync? cache the output data
    if (!sync) osize = omaxn >= need ? need : 0;
    // sync and has data? return it directly
    else if (omaxn)
        osize = xu_min(omaxn, need);
    // sync, no data or end?
    //  else osize = osize;

    // exit pull
    if (odata) xu_queue_buffer_pull_exit(&filter->odata, osize > 0 ? osize : 0);

    // return it if have the odata
    if (osize > 0) *pdata = odata;

    // trace
    xu_trace_d("[%p]: spak: ileft: %lu, oleft: %lu, offset: %llu, limit: %lld, beof: %d: %ld", self,
               xu_buffer_size(&filter->idata), xu_queue_buffer_size(&filter->odata), filter->offset, filter->limit,
               filter->beof, osize);

    // ok?
    return osize;
}
xu_bool_t xu_filter_push(xu_filter_ref_t self, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_filter_t* filter = (xu_filter_t*)self;
    xu_assert_and_check_return_val(filter && data && size, xu_false);

    // push data
    xu_bool_t ok = xu_buffer_memncat(&filter->idata, data, size) ? xu_true : xu_false;

    // save the input offset
    if (ok) filter->offset += size;

    // ok?
    return ok;
}
xu_bool_t xu_filter_beof(xu_filter_ref_t self)
{
    // check
    xu_filter_t* filter = (xu_filter_t*)self;
    xu_assert_and_check_return_val(filter, xu_false);

    // is eof?
    return filter->beof;
}
xu_void_t xu_filter_limit(xu_filter_ref_t self, xu_hong_t limit)
{
    // check
    xu_filter_t* filter = (xu_filter_t*)self;
    xu_assert_and_check_return(filter);

    // limit the input size
    filter->limit = limit;
}
