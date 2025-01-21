/* *******************************************************
 *  trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "chunked"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/libc/stdlib/stdlib.h"
#include "xutil/stream/filter.h"
#include "xutil/string/string.h"

/* *******************************************************
 *  types
 * *******************************************************
 */
// the chunked filter type
typedef struct __xu_filter_chunked_t
{
    // the filter base
    xu_filter_t base;

    // the chunked size
    xu_size_t size;

    // the chunked read
    xu_size_t read;

    // the cache line
    xu_string_t line;

} xu_filter_chunked_t;

/* *******************************************************
 *  implementation
 * *******************************************************
 */
static __xu_inline__ xu_filter_chunked_t* xu_filter_chunked_cast(xu_filter_t* filter)
{
    // check
    xu_assert_and_check_return_val(filter && filter->type == XU_FILTER_TYPE_CHUNKED, xu_null);
    return (xu_filter_chunked_t*)filter;
}
/* chunked_data
 *
 *   head     data   tail
 * ea5\r\n ..........\r\n e65\r\n..............\r\n 0\r\n\r\n
 * ---------------------- ------------------------- ---------
 *        chunk0                  chunk1               end
 */
static xu_long_t xu_filter_chunked_spak(xu_filter_t* filter, xu_static_stream_ref_t istream,
                                        xu_static_stream_ref_t ostream, xu_long_t sync)
{
    // check
    xu_filter_chunked_t* cfilter = xu_filter_chunked_cast(filter);
    xu_assert_and_check_return_val(cfilter && istream && ostream, -1);
    xu_assert_and_check_return_val(xu_static_stream_valid(istream) && xu_static_stream_valid(ostream), -1);

    // the idata
    xu_byte_t const* ip = xu_static_stream_pos(istream);
    xu_byte_t const* ie = xu_static_stream_end(istream);

    // trace
    xu_trace_d("[%p]: isize: %lu, beof: %d", cfilter, xu_static_stream_size(istream), filter->beof);

    // find the eof: '\r\n 0\r\n\r\n'
    if (!filter->beof && ip + 6 < ie && ie[-7] == '\r' && ie[-6] == '\n' && ie[-5] == '0' && ie[-4] == '\r' &&
        ie[-3] == '\n' && ie[-2] == '\r' && ie[-1] == '\n')
    {
        // is eof
        filter->beof = xu_true;
    }

    // the odata
    xu_byte_t* op = (xu_byte_t*)xu_static_stream_pos(ostream);
    xu_byte_t* oe = (xu_byte_t*)xu_static_stream_end(ostream);
    xu_byte_t* ob = op;

    // parse chunked head and chunked tail
    if (!cfilter->size || cfilter->read >= cfilter->size)
    {
        // walk
        while (ip < ie)
        {
            // the charactor
            xu_char_t ch = *ip++;

            // trace
            xu_trace_d("[%p]: character: %x", cfilter, ch);

            // check
            xu_assert_and_check_return_val(ch, -1);

            // append char to line
            if (ch != '\n') xu_string_chrcat(&cfilter->line, ch);
            // is line end?
            else
            {
                // check
                xu_char_t const* pb = xu_string_cstr(&cfilter->line);
                xu_size_t        pn = xu_string_size(&cfilter->line);
                xu_assert_and_check_return_val(pb, -1);

                // trace
                xu_trace_d("[%p]: line: %s", cfilter, xu_string_cstr(&cfilter->line));

                // strip '\r' if exists
                if (pb[pn - 1] == '\r') xu_string_strip(&cfilter->line, pn - 1);

                // is chunked tail? only "\r\n"
                if (!xu_string_size(&cfilter->line))
                {
                    // reset size
                    cfilter->read = 0;
                    cfilter->size = 0;

                    // trace
                    xu_trace_d("[%p]: tail", cfilter);

                    // continue
                    continue;
                }
                // is chunked head? parse size
                else
                {
                    // parse size
                    cfilter->size = xu_s16tou32(pb);

                    // trace
                    xu_trace_d("[%p]: size: %lu", cfilter, cfilter->size);

                    // clear data
                    xu_string_clear(&cfilter->line);

                    // is eof? "0\r\n\r\n"
                    if (!cfilter->size)
                    {
                        // trace
                        xu_trace_d("[%p]: eof", cfilter);

                        // is eof
                        filter->beof = xu_true;

                        // continue to spak the end data
                        continue;
                    }

                    // ok
                    break;
                }
            }
        }
    }

    // check
    xu_assert_and_check_return_val(cfilter->read <= cfilter->size, -1);

    // read chunked data
    xu_size_t size = xu_min3(ie - ip, oe - op, cfilter->size - cfilter->read);
    if (size)
    {
        // copy data
        xu_memcpy((xu_byte_t*)op, ip, size);
        ip += size;
        op += size;

        // update read
        cfilter->read += size;
    }

    // update stream
    xu_static_stream_goto(istream, (xu_byte_t*)ip);
    xu_static_stream_goto(ostream, (xu_byte_t*)op);

    // trace
    xu_trace_d("[%p]: read: %lu, size: %lu, beof: %u, ileft: %lu", cfilter, cfilter->read, cfilter->size, filter->beof,
               xu_static_stream_left(istream));

    // ok
    return (op - ob);
}
static xu_void_t xu_filter_chunked_clos(xu_filter_t* filter)
{
    // check
    xu_filter_chunked_t* cfilter = xu_filter_chunked_cast(filter);
    xu_assert_and_check_return(cfilter);

    // clear size
    cfilter->size = 0;

    // clear read
    cfilter->read = 0;

    // clear line
    xu_string_clear(&cfilter->line);
}
static xu_void_t xu_filter_chunked_exit(xu_filter_t* filter)
{
    // check
    xu_filter_chunked_t* cfilter = xu_filter_chunked_cast(filter);
    xu_assert_and_check_return(cfilter);

    // exit line
    xu_string_exit(&cfilter->line);
}

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
xu_filter_ref_t xu_filter_init_from_chunked(xu_bool_t dechunked)
{
    // done
    xu_bool_t            ok     = xu_false;
    xu_filter_chunked_t* filter = xu_null;
    do
    {
        // noimpl for encoding chunked
        if (!dechunked)
        {
            xu_trace_noimpl();
            break;
        }

        // make filter
        filter = xu_malloc0_type(xu_filter_chunked_t);
        xu_assert_and_check_break(filter);

        // init filter
        if (!xu_filter_init((xu_filter_t*)filter, XU_FILTER_TYPE_CHUNKED)) break;
        filter->base.spak = xu_filter_chunked_spak;
        filter->base.clos = xu_filter_chunked_clos;
        filter->base.exit = xu_filter_chunked_exit;

        // init line
        if (!xu_string_init(&filter->line)) break;

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
