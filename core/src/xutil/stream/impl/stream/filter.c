#include "xutil/stream/filter.h"
#include "xutil/stream/stream.h"

// the stream filter type
typedef struct __xu_stream_filter_t
{
    // the filter
    xu_filter_ref_t filter;

    // the filter is referenced? need not exit it
    xu_bool_t bref;

    // is eof?
    xu_bool_t beof;

    // is wait?
    xu_bool_t wait;

    // the last
    xu_long_t last;

    // the mode, none: 0, read: 1, writ: -1
    xu_long_t mode;

    // the stream
    xu_stream_ref_t stream;

    // the left write data
    xu_byte_t const* wdata;
    xu_size_t        wsize;

} xu_stream_filter_t;

static __xu_inline__ xu_stream_filter_t* xu_stream_filter_cast(xu_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && xu_stream_type(stream) == XU_STREAM_TYPE_FLTR, xu_null);

    // ok?
    return (xu_stream_filter_t*)stream;
}
static xu_bool_t xu_stream_filter_open(xu_stream_ref_t stream)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return_val(stream_filter && stream_filter->stream, xu_false);

    // clear mode
    stream_filter->mode = 0;

    // clear last
    stream_filter->last = 0;

    // clear wait
    stream_filter->wait = xu_false;

    // clear eof
    stream_filter->beof = xu_false;

    // clear write data
    stream_filter->wdata = xu_null;
    stream_filter->wsize = 0;

    // open filter
    if (stream_filter->filter && !xu_filter_open(stream_filter->filter)) return xu_false;

    // open stream
    return !xu_stream_is_opened(stream_filter->stream) ? xu_stream_open(stream_filter->stream) : xu_true;
}
static xu_bool_t xu_stream_filter_clos(xu_stream_ref_t stream)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return_val(stream_filter && stream_filter->stream, xu_false);

    // sync the end filter data
    if (stream_filter->filter && stream_filter->mode == -1)
    {
        // flush the left data first
        if (stream_filter->wdata && stream_filter->wsize)
        {
            if (!xu_stream_bwrit(stream_filter->stream, stream_filter->wdata, stream_filter->wsize)) return xu_false;
            stream_filter->wdata = xu_null;
            stream_filter->wsize = 0;
        }

        // spak data
        xu_byte_t const* data = xu_null;
        xu_long_t        size = xu_filter_spak(stream_filter->filter, xu_null, 0, &data, 0, -1);
        if (size > 0 && data)
        {
            // writ data
            if (!xu_stream_bwrit(stream_filter->stream, data, size)) return xu_false;
        }
    }

    // done
    xu_bool_t ok = xu_stream_clos(stream_filter->stream);

    // ok?
    if (ok)
    {
        // clear mode
        stream_filter->mode = 0;

        // clear last
        stream_filter->last = 0;

        // clear wait
        stream_filter->wait = xu_false;

        // clear eof
        stream_filter->beof = xu_false;

        // clear write data
        stream_filter->wdata = xu_null;
        stream_filter->wsize = 0;

        // close the filter
        if (stream_filter->filter) xu_filter_clos(stream_filter->filter);
    }

    // ok?
    return ok;
}
static xu_void_t xu_stream_filter_exit(xu_stream_ref_t stream)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return(stream_filter);

    // exit it
    if (!stream_filter->bref && stream_filter->filter) xu_filter_exit(stream_filter->filter);
    stream_filter->filter = xu_null;
    stream_filter->bref   = xu_false;
}
static xu_void_t xu_stream_filter_kill(xu_stream_ref_t stream)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return(stream_filter);

    // kill it
    if (stream_filter->stream) xu_stream_kill(stream_filter->stream);
}
static xu_long_t xu_stream_filter_read(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return_val(stream_filter && stream_filter->stream && data, -1);
    xu_check_return_val(size, 0);

    // read
    xu_long_t real = xu_stream_read(stream_filter->stream, data, size);

    // done filter
    if (stream_filter->filter)
    {
        // save mode: read
        if (!stream_filter->mode) stream_filter->mode = 1;

        // check mode
        xu_assert_and_check_return_val(stream_filter->mode == 1, -1);

        // save last
        stream_filter->last = real;

        // eof?
        if (real < 0 || (!real && stream_filter->wait) || xu_filter_beof(stream_filter->filter))
            stream_filter->beof = xu_true;
        // clear wait
        else if (real > 0)
            stream_filter->wait = xu_false;

        // spak data
        xu_byte_t const* odata = xu_null;
        if (real)
            real = xu_filter_spak(stream_filter->filter, data, real < 0 ? 0 : real, &odata, size,
                                  stream_filter->beof ? -1 : 0);
        // no data? try to sync it
        if (!real) real = xu_filter_spak(stream_filter->filter, xu_null, 0, &odata, size, stream_filter->beof ? -1 : 1);

        // has data? save it
        if (real > 0 && odata) xu_memcpy(data, odata, real);

        // eof?
        if (stream_filter->beof && !real) real = -1;
    }

    // ok?
    return real;
}
static xu_long_t xu_stream_filter_writ(xu_stream_ref_t stream, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return_val(stream_filter && stream_filter->stream && data, -1);
    xu_check_return_val(size, 0);

    // done filter
    if (stream_filter->filter)
    {
        // save mode: writ
        if (!stream_filter->mode) stream_filter->mode = -1;

        // check mode
        xu_assert_and_check_return_val(stream_filter->mode == -1, -1);

        // write the left data first
        if (stream_filter->wdata && stream_filter->wsize)
        {
            xu_long_t writ = xu_stream_writ(stream_filter->stream, stream_filter->wdata, stream_filter->wsize);
            if (writ > 0 && writ <= stream_filter->wsize)
            {
                stream_filter->wdata = stream_filter->wdata + writ;
                stream_filter->wsize -= writ;
            }
            xu_check_return_val(stream_filter->wsize, 0);
            stream_filter->wdata = xu_null;
        }

        // spak data
        xu_long_t real = xu_filter_spak(stream_filter->filter, data, size, &data, size, 0);
        xu_assert_and_check_return_val(real >= 0, -1);

        // no output data?
        xu_check_return_val(real, size);

        // write output data, @note need return the input size, filter/spak will cache all input data
        xu_long_t writ = xu_stream_writ(stream_filter->stream, data, real);
        if (writ >= 0)
        {
            // save the left data
            if (writ < real)
            {
                stream_filter->wdata = data + writ;
                stream_filter->wsize = real - writ;
            }
            return size;
        }
        else
            return -1;
    }
    else
        return xu_stream_writ(stream_filter->stream, data, size);
}
static xu_bool_t xu_stream_filter_sync(xu_stream_ref_t stream, xu_bool_t bclosing)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return_val(stream_filter && stream_filter->stream, xu_false);

    // done filter
    if (stream_filter->filter)
    {
        // save mode: writ
        if (!stream_filter->mode) stream_filter->mode = -1;

        // check mode
        xu_assert_and_check_return_val(stream_filter->mode == -1, xu_false);

        // flush the left data first
        if (stream_filter->wdata && stream_filter->wsize)
        {
            if (!xu_stream_bwrit(stream_filter->stream, stream_filter->wdata, stream_filter->wsize)) return xu_false;
            stream_filter->wdata = xu_null;
            stream_filter->wsize = 0;
        }

        // spak data
        xu_byte_t const* data = xu_null;
        xu_long_t        real = -1;
        while (!xu_stream_is_killed(stream) &&
               (real = xu_filter_spak(stream_filter->filter, xu_null, 0, &data, 0, bclosing ? -1 : 1)) > 0 && data)
        {
            if (!xu_stream_bwrit(stream_filter->stream, data, real)) return xu_false;
        }
    }

    // writ
    return xu_stream_sync(stream_filter->stream, bclosing);
}
static xu_long_t xu_stream_filter_wait(xu_stream_ref_t stream, xu_size_t wait, xu_long_t timeout)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return_val(stream_filter && stream_filter->stream, -1);

    // done
    xu_long_t ok = -1;
    if (stream_filter->filter && stream_filter->mode == 1)
    {
        // wait ok
        if (stream_filter->last > 0 || stream_filter->wsize) ok = wait;
        // need wait
        else if (!stream_filter->last && !stream_filter->beof && !xu_filter_beof(stream_filter->filter))
        {
            // wait
            ok = xu_stream_wait(stream_filter->stream, wait, timeout);

            // eof?
            if (!ok)
            {
                // wait ok and continue to read or writ
                ok = wait;

                // set eof
                stream_filter->beof = xu_true;
            }
            // wait ok
            else
                stream_filter->wait = xu_true;
        }
        // eof
        else
        {
            // wait ok and continue to read or writ
            ok = wait;

            // set eof
            stream_filter->beof = xu_true;
        }
    }
    else
        ok = xu_stream_wait(stream_filter->stream, wait, timeout);

    // ok?
    return ok;
}
static xu_bool_t xu_stream_filter_ctrl(xu_stream_ref_t stream, xu_size_t ctrl, xu_va_list_t args)
{
    // check
    xu_stream_filter_t* stream_filter = xu_stream_filter_cast(stream);
    xu_assert_and_check_return_val(stream_filter, xu_false);

    // ctrl
    switch (ctrl)
    {
    case XU_STREAM_CTRL_FLTR_SET_STREAM:
    {
        // check
        xu_assert_and_check_break(xu_stream_is_closed(stream));

        // set stream
        stream_filter->stream = (xu_stream_ref_t)xu_va_arg(args, xu_stream_ref_t);

        // ok
        return xu_true;
    }
    case XU_STREAM_CTRL_FLTR_GET_STREAM:
    {
        // the pstream
        xu_stream_ref_t* pstream = (xu_stream_ref_t*)xu_va_arg(args, xu_stream_ref_t*);
        xu_assert_and_check_break(pstream);

        // set stream
        *pstream = stream_filter->stream;

        // ok
        return xu_true;
    }
    case XU_STREAM_CTRL_FLTR_SET_FILTER:
    {
        // check
        xu_assert_and_check_break(xu_stream_is_closed(stream));

        // exit filter first if exists
        if (!stream_filter->bref && stream_filter->filter) xu_filter_exit(stream_filter->filter);

        // set filter
        xu_filter_ref_t filter = (xu_filter_ref_t)xu_va_arg(args, xu_filter_ref_t);
        stream_filter->filter  = filter;
        stream_filter->bref    = filter ? xu_true : xu_false;

        // ok
        return xu_true;
    }
    case XU_STREAM_CTRL_FLTR_GET_FILTER:
    {
        // the pfilter
        xu_filter_ref_t* pfilter = (xu_filter_ref_t*)xu_va_arg(args, xu_filter_ref_t*);
        xu_assert_and_check_break(pfilter);

        // set filter
        *pfilter = stream_filter->filter;

        // ok
        return xu_true;
    }
    default: break;
    }

    // failed
    return xu_false;
}

xu_stream_ref_t xu_stream_init_filter()
{
    return xu_stream_init(XU_STREAM_TYPE_FLTR, sizeof(xu_stream_filter_t), 0, xu_stream_filter_open,
                          xu_stream_filter_clos, xu_stream_filter_exit, xu_stream_filter_ctrl, xu_stream_filter_wait,
                          xu_stream_filter_read, xu_stream_filter_writ, xu_null, xu_stream_filter_sync,
                          xu_stream_filter_kill);
}
xu_stream_ref_t xu_stream_init_filter_from_null(xu_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream, xu_null);

    // done
    xu_bool_t       ok            = xu_false;
    xu_stream_ref_t stream_filter = xu_null;
    do
    {
        // init stream
        stream_filter = xu_stream_init_filter();
        xu_assert_and_check_break(stream_filter);

        // set stream
        if (!xu_stream_ctrl(stream_filter, XU_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream_filter) xu_stream_exit(stream_filter);
        stream_filter = xu_null;
    }

    // ok
    return stream_filter;
}
#ifdef XU_CONFIG_MODULE_HAVE_ZIP
xu_stream_ref_t xu_stream_init_filter_from_zip(xu_stream_ref_t stream, xu_size_t algo, xu_size_t action)
{
    // check
    xu_assert_and_check_return_val(stream, xu_null);

    // done
    xu_bool_t       ok            = xu_false;
    xu_stream_ref_t stream_filter = xu_null;
    do
    {
        // init stream
        stream_filter = xu_stream_init_filter();
        xu_assert_and_check_break(stream_filter);

        // set stream
        if (!xu_stream_ctrl(stream_filter, XU_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // set filter
        ((xu_stream_filter_t*)stream_filter)->bref   = xu_false;
        ((xu_stream_filter_t*)stream_filter)->filter = xu_filter_init_from_zip(algo, action);
        xu_assert_and_check_break(((xu_stream_filter_t*)stream_filter)->filter);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream_filter) xu_stream_exit(stream_filter);
        stream_filter = xu_null;
    }

    // ok
    return stream_filter;
}
#endif
xu_stream_ref_t xu_stream_init_filter_from_cache(xu_stream_ref_t stream, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(stream, xu_null);

    // done
    xu_bool_t       ok            = xu_false;
    xu_stream_ref_t stream_filter = xu_null;
    do
    {
        // init stream
        stream_filter = xu_stream_init_filter();
        xu_assert_and_check_break(stream_filter);

        // set stream
        if (!xu_stream_ctrl(stream_filter, XU_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // set filter
        ((xu_stream_filter_t*)stream_filter)->bref   = xu_false;
        ((xu_stream_filter_t*)stream_filter)->filter = xu_filter_init_from_cache(size);
        xu_assert_and_check_break(((xu_stream_filter_t*)stream_filter)->filter);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream_filter) xu_stream_exit(stream_filter);
        stream_filter = xu_null;
    }

    // ok
    return stream_filter;
}
#ifdef XU_CONFIG_MODULE_HAVE_CHARSET
xu_stream_ref_t xu_stream_init_filter_from_charset(xu_stream_ref_t stream, xu_size_t fr, xu_size_t to)
{
    // check
    xu_assert_and_check_return_val(stream, xu_null);

    // done
    xu_bool_t       ok            = xu_false;
    xu_stream_ref_t stream_filter = xu_null;
    do
    {
        // init stream
        stream_filter = xu_stream_init_filter();
        xu_assert_and_check_break(stream_filter);

        // set stream
        if (!xu_stream_ctrl(stream_filter, XU_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // set filter
        ((xu_stream_filter_t*)stream_filter)->bref   = xu_false;
        ((xu_stream_filter_t*)stream_filter)->filter = xu_filter_init_from_charset(fr, to);
        xu_assert_and_check_break(((xu_stream_filter_t*)stream_filter)->filter);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream_filter) xu_stream_exit(stream_filter);
        stream_filter = xu_null;
    }

    // ok
    return stream_filter;
}
#endif
xu_stream_ref_t xu_stream_init_filter_from_chunked(xu_stream_ref_t stream, xu_bool_t dechunked)
{
    // check
    xu_assert_and_check_return_val(stream, xu_null);

    // done
    xu_bool_t       ok            = xu_false;
    xu_stream_ref_t stream_filter = xu_null;
    do
    {
        // init stream
        stream_filter = xu_stream_init_filter();
        xu_assert_and_check_break(stream_filter);

        // set stream
        if (!xu_stream_ctrl(stream_filter, XU_STREAM_CTRL_FLTR_SET_STREAM, stream)) break;

        // set filter
        ((xu_stream_filter_t*)stream_filter)->bref   = xu_false;
        ((xu_stream_filter_t*)stream_filter)->filter = xu_filter_init_from_chunked(dechunked);
        xu_assert_and_check_break(((xu_stream_filter_t*)stream_filter)->filter);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream_filter) xu_stream_exit(stream_filter);
        stream_filter = xu_null;
    }

    // ok
    return stream_filter;
}
