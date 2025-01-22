/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "cache"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/stream/filter.h"

/* *******************************************************
 * types
 */
// the cache filter type
typedef struct __xu_filter_cache_t
{
    // the filter base
    xu_filter_t base;

} xu_filter_cache_t;

/* *******************************************************
 * implementation
 */
static __xu_inline__ xu_filter_cache_t* xu_filter_cache_cast(xu_filter_t* filter)
{
    // check
    xu_assert_and_check_return_val(filter && filter->type == XU_FILTER_TYPE_CACHE, xu_null);
    return (xu_filter_cache_t*)filter;
}
static xu_long_t xu_filter_cache_spak(xu_filter_t* filter, xu_static_stream_ref_t istream,
                                      xu_static_stream_ref_t ostream, xu_long_t sync)
{
    // check
    xu_filter_cache_t* cfilter = xu_filter_cache_cast(filter);
    xu_assert_and_check_return_val(cfilter && istream && ostream, -1);
    xu_assert_and_check_return_val(xu_static_stream_valid(istream) && xu_static_stream_valid(ostream), -1);

    // the idata
    xu_byte_t const* ip = xu_static_stream_pos(istream);
    xu_byte_t const* ie = xu_static_stream_end(istream);

    // the odata
    xu_byte_t* op = (xu_byte_t*)xu_static_stream_pos(ostream);
    xu_byte_t* oe = (xu_byte_t*)xu_static_stream_end(ostream);
    xu_byte_t* ob = op;

    // the need
    xu_size_t need = xu_min(ie - ip, oe - op);

    // copy data
    if (need) xu_memcpy(op, ip, need);
    ip += need;
    op += need;

    // update stream
    xu_static_stream_goto(istream, (xu_byte_t*)ip);
    xu_static_stream_goto(ostream, (xu_byte_t*)op);

    // no data and sync end? end
    if (sync < 0 && op == ob && !xu_static_stream_left(istream)) return -1;

    // ok
    return (op - ob);
}

/* *******************************************************
 * interfaces
 */
xu_filter_ref_t xu_filter_init_from_cache(xu_size_t size)
{
    // done
    xu_bool_t          ok     = xu_false;
    xu_filter_cache_t* filter = xu_null;
    do
    {
        // make filter
        filter = xu_malloc0_type(xu_filter_cache_t);
        xu_assert_and_check_break(filter);

        // init filter
        if (!xu_filter_init((xu_filter_t*)filter, XU_FILTER_TYPE_CACHE)) break;
        filter->base.spak = xu_filter_cache_spak;

        // init the cache size
        if (size) xu_queue_buffer_resize(&filter->base.odata, size);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit filter
        xu_filter_exit((xu_filter_ref_t)filter);
        filter = xu_null;
    }

    // ok?
    return (xu_filter_ref_t)filter;
}
