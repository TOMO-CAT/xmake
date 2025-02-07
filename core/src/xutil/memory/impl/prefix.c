#include "xutil/memory/impl/prefix.h"
#include "xutil/platform/backtrace.h"
#include "xutil/utils/dump.h"
#include "xutil/xutil.h"

/* *******************************************************
 * implementation
 */

#ifdef __xu_debug__
__xu_no_sanitize_address__ xu_size_t xu_pool_data_size(xu_cpointer_t data)
{
    // check
    xu_check_return_val(data, 0);

    // done
    xu_size_t            size      = 0;
    xu_pool_data_head_t* data_head = xu_null;
    do
    {
        // tbox must be running normally
        xu_check_break(xutil_state() == XU_STATE_OK);

        // get global allocator
        xu_allocator_ref_t allocator = xu_allocator();
        xu_check_break(allocator);

        // have this data address?
        xu_check_break(xu_allocator_have(allocator, data));

        // the data head
        data_head = &(((xu_pool_data_head_t*)data)[-1]);
        xu_check_break(data_head->debug.magic == XU_POOL_DATA_MAGIC);

        // ok
        size = data_head->size;

    } while (0);

    // ok?
    return size;
}
xu_void_t xu_pool_data_dump(xu_cpointer_t data, xu_bool_t verbose, xu_char_t const* prefix)
{
    // done
    xu_pool_data_head_t* data_head = xu_null;
    do
    {
        // no data?
        xu_assert_and_check_break(data);

        // the data head
        data_head = &(((xu_pool_data_head_t*)data)[-1]);

        // dump the head info
        xu_size_t data_limit = 256;
        if (data_head->debug.magic == XU_POOL_DATA_MAGIC)
        {
            // the data size
            xu_size_t data_size = (xu_size_t)data_head->size;

            // format the backtrace prefix
            xu_char_t backtrace_prefix[256] = {0};
            xu_snprintf(backtrace_prefix, sizeof(backtrace_prefix), "%s    ", prefix ? prefix : "");

            // dump backtrace
            xu_size_t nframe = 0;
            while (nframe < xu_arrayn(data_head->debug.backtrace) && data_head->debug.backtrace[nframe])
                nframe++;
            xu_trace_i("%sdata: from: %s(): %u, %s", prefix ? prefix : "", data_head->debug.func, data_head->debug.line,
                       data_head->debug.file);
            xu_backtrace_dump(backtrace_prefix, data_head->debug.backtrace, nframe);

            // dump the data info
            xu_trace_i("%sdata: %p, size: %lu, patch: %x", prefix ? prefix : "", data, data_size,
                       ((xu_byte_t const*)data)[data_size]);

            // dump the first 256-bytes data
            if (data_size && verbose)
            {
                // the dump size
                xu_size_t dump_size = xu_min(data_size, data_limit);

                // dump it
                xu_trace_i("%sdata: first %lu-bytes:", prefix ? prefix : "", dump_size);
                xu_dump_data((xu_byte_t const*)data, dump_size);

                // dump the last 256-bytes data
                if (data_size > dump_size)
                {
                    // the last data
                    xu_byte_t const* data_last =
                        xu_max((xu_byte_t const*)data + data_size - data_limit, (xu_byte_t const*)data + dump_size);

                    // update the dump size
                    dump_size = (xu_byte_t const*)data + data_size - data_last;

                    // dump it
                    xu_trace_i("%sdata: last %lu-bytes:", prefix ? prefix : "", dump_size);
                    xu_dump_data(data_last, dump_size);
                }
            }
        }
        // for the public fixed_pool
        else if (data_head->debug.magic == XU_POOL_DATA_EMPTY_MAGIC)
        {
            // format the backtrace prefix
            xu_char_t backtrace_prefix[256] = {0};
            xu_snprintf(backtrace_prefix, sizeof(backtrace_prefix), "%s    ", prefix ? prefix : "");

            // dump backtrace
            xu_size_t nframe = 0;
            while (nframe < xu_arrayn(data_head->debug.backtrace) && data_head->debug.backtrace[nframe])
                nframe++;
            xu_trace_i("%sdata: from: %s(): %u, %s", prefix ? prefix : "", data_head->debug.func, data_head->debug.line,
                       data_head->debug.file);
            xu_backtrace_dump(backtrace_prefix, data_head->debug.backtrace, nframe);

            // dump the data info
            xu_trace_i("%sdata: %p, size: fixed", prefix ? prefix : "", data);
        }
        else
        {
            // dump the data head
            xu_trace_i("%sdata: invalid head:", prefix ? prefix : "");
            xu_dump_data((xu_byte_t const*)data_head, sizeof(xu_pool_data_head_t));

            // dump the first 256-bytes data
            xu_trace_i("%sdata: first %lu-bytes:", prefix ? prefix : "", data_limit);
            xu_dump_data((xu_byte_t const*)data, data_limit);
        }

    } while (0);
}
xu_void_t xu_pool_data_save_backtrace(xu_pool_data_debug_head_t* debug_head, xu_size_t skip_frames)
{
    xu_size_t nframe = xu_backtrace_frames(debug_head->backtrace, xu_arrayn(debug_head->backtrace), skip_frames + 2);
    if (nframe < xu_arrayn(debug_head->backtrace))
        xu_memset_(debug_head->backtrace + nframe, 0,
                   (xu_arrayn(debug_head->backtrace) - nframe) * sizeof(xu_cpointer_t));
}
#endif
