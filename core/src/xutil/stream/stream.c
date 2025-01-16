/* *******************************************************
 * trace
 * *******************************************************
 */

#define XU_TRACE_MODULE_NAME "stream"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/stream/stream.h"
#include "xutil/stream/impl/stream.h"
#include "xutil/stream/prefix.h"
#include "xutil/utils/state.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_stream_ref_t xu_stream_init(xu_size_t type, xu_size_t type_size, xu_size_t cache,
                               xu_bool_t (*open)(xu_stream_ref_t self), xu_bool_t (*clos)(xu_stream_ref_t self),
                               xu_void_t (*exit)(xu_stream_ref_t self),
                               xu_bool_t (*ctrl)(xu_stream_ref_t self, xu_size_t ctrl, xu_va_list_t args),
                               xu_long_t (*wait)(xu_stream_ref_t self, xu_size_t wait, xu_long_t timeout),
                               xu_long_t (*read)(xu_stream_ref_t self, xu_byte_t* data, xu_size_t size),
                               xu_long_t (*writ)(xu_stream_ref_t self, xu_byte_t const* data, xu_size_t size),
                               xu_bool_t (*seek)(xu_stream_ref_t self, xu_hize_t offset),
                               xu_bool_t (*sync)(xu_stream_ref_t self, xu_bool_t bclosing),
                               xu_void_t (*kill)(xu_stream_ref_t self))
{
    // check
    xu_assert_and_check_return_val(type_size, xu_null);
    xu_assert_and_check_return_val(open && clos && ctrl && wait, xu_null);
    xu_assert_and_check_return_val(read || writ, xu_null);

    // done
    xu_bool_t       ok         = xu_false;
    xu_stream_t*    stream     = xu_null;
    xu_stream_ref_t stream_ref = xu_null;
    do
    {
        // make stream
        stream = (xu_stream_t*)xu_malloc0(sizeof(xu_stream_t) + type_size);
        xu_assert_and_check_break(stream);

        // init stream reference
        stream_ref = (xu_stream_ref_t)&stream[1];

        // init type
        stream->type = (xu_uint8_t)type;

        // init timeout, 10s
        stream->timeout = XU_STREAM_DEFAULT_TIMEOUT;

        // init internal state
        stream->istate = XU_STATE_CLOSED;

        // init url
        if (!xu_url_init(&stream->url)) break;

        // init cache
        if (!xu_queue_buffer_init(&stream->cache, cache)) break;

        // init func
        stream->open = open;
        stream->clos = clos;
        stream->exit = exit;
        stream->ctrl = ctrl;
        stream->wait = wait;
        stream->read = read;
        stream->writ = writ;
        stream->seek = seek;
        stream->sync = sync;
        stream->kill = kill;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream_ref) xu_stream_exit(stream_ref);
        stream_ref = xu_null;
    }

    // ok?
    return stream_ref;
}
xu_stream_ref_t xu_stream_init_from_url(xu_char_t const* url)
{
    // check
    xu_assert_and_check_return_val(url, xu_null);

    // the init
    static xu_stream_ref_t (*s_init[])() = {xu_null, xu_stream_init_file, xu_stream_init_sock, xu_stream_init_http,
                                            xu_stream_init_data};

    // probe protocol
    xu_size_t protocol = xu_url_protocol_probe(url);
    xu_assert_static((xu_size_t)XU_URL_PROTOCOL_FILE == (xu_size_t)XU_STREAM_TYPE_FILE);
    xu_assert_static((xu_size_t)XU_URL_PROTOCOL_HTTP == (xu_size_t)XU_STREAM_TYPE_HTTP);
    xu_assert_static((xu_size_t)XU_URL_PROTOCOL_SOCK == (xu_size_t)XU_STREAM_TYPE_SOCK);
    xu_assert_static((xu_size_t)XU_URL_PROTOCOL_DATA == (xu_size_t)XU_STREAM_TYPE_DATA);

    // protocol => type
    xu_size_t type = protocol;
    if (!type || type > XU_STREAM_TYPE_DATA)
    {
        xu_trace_e("unknown self for url: %s", url);
        return xu_null;
    }
    xu_assert_and_check_return_val(type && type < xu_arrayn(s_init) && s_init[type], xu_null);

    // done
    xu_bool_t       ok   = xu_false;
    xu_stream_ref_t self = xu_null;
    do
    {
        // init self
        self = s_init[type]();
        xu_assert_and_check_break(self);

        // init url
        if (!xu_stream_ctrl(self, XU_STREAM_CTRL_SET_URL, url)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit self
        if (self) xu_stream_exit(self);
        self = xu_null;
    }

    // ok?
    return self;
}
xu_void_t xu_stream_exit(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return(stream);

    // close it
    xu_stream_clos(self);

    // exit it
    if (stream->exit) stream->exit(self);

    // exit cache
    xu_queue_buffer_exit(&stream->cache);

    // exit url
    xu_url_exit(&stream->url);

    // free it
    xu_free(stream);
}
xu_long_t xu_stream_wait(xu_stream_ref_t self, xu_size_t wait, xu_long_t timeout)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream && stream->wait, -1);

    // stoped?
    xu_assert_and_check_return_val(XU_STATE_OPENED == xu_atomic32_get(&stream->istate), -1);

    // wait it
    xu_long_t ok = stream->wait(self, wait, timeout);

    // wait failed? save state
    if (ok < 0 && !stream->state) stream->state = XU_STATE_WAIT_FAILED;

    // ok?
    xu_check_return_val(!ok, ok);

    // cached?
    if (xu_queue_buffer_maxn(&stream->cache))
    {
        // have read cache?
        if ((wait & XU_STREAM_WAIT_READ) && !stream->bwritted && !xu_queue_buffer_null(&stream->cache))
            ok |= XU_STREAM_WAIT_READ;
        // have writ cache?
        else if ((wait & XU_STREAM_WAIT_WRIT) && stream->bwritted && !xu_queue_buffer_full(&stream->cache))
            ok |= XU_STREAM_WAIT_WRIT;
    }

    // ok?
    return ok;
}
xu_size_t xu_stream_state(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, XU_STATE_UNKNOWN_ERROR);

    // the self state
    return stream->state;
}
xu_void_t xu_stream_state_set(xu_stream_ref_t self, xu_size_t state)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return(stream);

    // set the self state
    stream->state = state;
}
xu_size_t xu_stream_type(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, XU_STREAM_TYPE_NONE);

    // the type
    return stream->type;
}
xu_hong_t xu_stream_size(xu_stream_ref_t self)
{
    // check
    xu_assert_and_check_return_val(self, 0);

    // get the size
    xu_hong_t size = -1;
    return xu_stream_ctrl((xu_stream_ref_t)self, XU_STREAM_CTRL_GET_SIZE, &size) ? size : -1;
}
xu_hize_t xu_stream_left(xu_stream_ref_t self)
{
    // check
    xu_assert_and_check_return_val(self, 0);

    // the size
    xu_hong_t size = xu_stream_size(self);
    xu_check_return_val(size >= 0, -1);

    // the offset
    xu_hize_t offset = xu_stream_offset(self);
    xu_assert_and_check_return_val(offset <= size, 0);

    // the left
    return size - offset;
}
xu_bool_t xu_stream_beof(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, xu_true);

    // wait failed? eof
    xu_check_return_val(stream->state != XU_STATE_WAIT_FAILED, xu_true);

    // size
    xu_hong_t size   = xu_stream_size(self);
    xu_hize_t offset = xu_stream_offset(self);

    // eof?
    return (size >= 0 && offset >= size) ? xu_true : xu_false;
}
xu_hize_t xu_stream_offset(xu_stream_ref_t self)
{
    // check
    xu_assert_and_check_return_val(self, 0);

    // get the offset
    xu_hize_t offset = 0;
    return xu_stream_ctrl(self, XU_STREAM_CTRL_GET_OFFSET, &offset) ? offset : 0;
}
xu_url_ref_t xu_stream_url(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, xu_null);

    // get the url
    return &stream->url;
}
xu_long_t xu_stream_timeout(xu_stream_ref_t self)
{
    // check
    xu_assert_and_check_return_val(self, -1);

    // get the timeout
    xu_long_t timeout = -1;
    return xu_stream_ctrl(self, XU_STREAM_CTRL_GET_TIMEOUT, &timeout) ? timeout : -1;
}
xu_bool_t xu_stream_is_opened(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, xu_false);

    // the state
    xu_size_t state = xu_atomic32_get(&stream->istate);

    // is opened?
    return (XU_STATE_OPENED == state || XU_STATE_KILLING == state) ? xu_true : xu_false;
}
xu_bool_t xu_stream_is_closed(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, xu_false);

    // the state
    xu_size_t state = xu_atomic32_get(&stream->istate);

    // is closed?
    return (XU_STATE_CLOSED == state || XU_STATE_KILLED == state) ? xu_true : xu_false;
}
xu_bool_t xu_stream_is_killed(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, xu_false);

    // the state
    xu_size_t state = xu_atomic32_get(&stream->istate);

    // is killed?
    return (XU_STATE_KILLED == state || XU_STATE_KILLING == state) ? xu_true : xu_false;
}
xu_bool_t xu_stream_ctrl(xu_stream_ref_t self, xu_size_t ctrl, ...)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream && stream->ctrl, xu_false);

    // init args
    xu_va_list_t args;
    xu_va_start(args, ctrl);

    // ctrl it
    xu_bool_t ok = xu_stream_ctrl_with_args(self, ctrl, args);

    // exit args
    xu_va_end(args);

    // ok?
    return ok;
}
xu_bool_t xu_stream_ctrl_with_args(xu_stream_ref_t self, xu_size_t ctrl, xu_va_list_t args)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream && stream->ctrl, xu_false);

    // save args
    xu_va_list_t args_saved;
    xu_va_copy(args_saved, args);

    // done
    xu_bool_t ok = xu_false;
    switch (ctrl)
    {
    case XU_STREAM_CTRL_GET_OFFSET:
    {
        // the poffset
        xu_hize_t* poffset = (xu_hize_t*)xu_va_arg(args, xu_hize_t*);
        xu_assert_and_check_return_val(poffset, xu_false);

        // get offset
        *poffset = stream->offset;

        // ok
        ok = xu_true;
    }
    break;
    case XU_STREAM_CTRL_SET_URL:
    {
        // check
        xu_assert_and_check_return_val(xu_stream_is_closed(self), xu_false);

        // set url
        xu_char_t const* url = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        if (url && xu_url_cstr_set(&stream->url, url)) ok = xu_true;
    }
    break;
    case XU_STREAM_CTRL_GET_URL:
    {
        // get url
        xu_char_t const** purl = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        if (purl)
        {
            xu_char_t const* url = xu_url_cstr(&stream->url);
            if (url)
            {
                *purl = url;
                ok    = xu_true;
            }
        }
    }
    break;
    case XU_STREAM_CTRL_SET_HOST:
    {
        // check
        xu_assert_and_check_return_val(xu_stream_is_closed(self), xu_false);

        // set host
        xu_char_t const* host = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        if (host)
        {
            xu_url_host_set(&stream->url, host);
            ok = xu_true;
        }
    }
    break;
    case XU_STREAM_CTRL_GET_HOST:
    {
        // get host
        xu_char_t const** phost = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        if (phost)
        {
            xu_char_t const* host = xu_url_host(&stream->url);
            if (host)
            {
                *phost = host;
                ok     = xu_true;
            }
        }
    }
    break;
    case XU_STREAM_CTRL_SET_PORT:
    {
        // check
        xu_assert_and_check_return_val(xu_stream_is_closed(self), xu_false);

        // set port
        xu_size_t port = (xu_size_t)xu_va_arg(args, xu_size_t);
        if (port)
        {
            xu_url_port_set(&stream->url, (xu_uint16_t)port);
            ok = xu_true;
        }
    }
    break;
    case XU_STREAM_CTRL_GET_PORT:
    {
        // get port
        xu_size_t* pport = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        if (pport)
        {
            *pport = xu_url_port(&stream->url);
            ok     = xu_true;
        }
    }
    break;
    case XU_STREAM_CTRL_SET_PATH:
    {
        // check
        xu_assert_and_check_return_val(xu_stream_is_closed(self), xu_false);

        // set path
        xu_char_t const* path = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        if (path)
        {
            xu_url_path_set(&stream->url, path);
            ok = xu_true;
        }
    }
    break;
    case XU_STREAM_CTRL_GET_PATH:
    {
        // get path
        xu_char_t const** ppath = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        if (ppath)
        {
            xu_char_t const* path = xu_url_path(&stream->url);
            if (path)
            {
                *ppath = path;
                ok     = xu_true;
            }
        }
    }
    break;
    case XU_STREAM_CTRL_SET_SSL:
    {
        // check
        xu_assert_and_check_return_val(xu_stream_is_closed(self), xu_false);

        // set ssl
        xu_bool_t bssl = (xu_bool_t)xu_va_arg(args, xu_bool_t);
        xu_url_ssl_set(&stream->url, bssl);
        ok = xu_true;
    }
    break;
    case XU_STREAM_CTRL_GET_SSL:
    {
        // get ssl
        xu_bool_t* pssl = (xu_bool_t*)xu_va_arg(args, xu_bool_t*);
        if (pssl)
        {
            *pssl = xu_url_ssl(&stream->url);
            ok    = xu_true;
        }
    }
    break;
    case XU_STREAM_CTRL_SET_TIMEOUT:
    {
        // check
        xu_assert_and_check_return_val(xu_stream_is_closed(self), xu_false);

        // set timeout
        xu_long_t timeout = (xu_long_t)xu_va_arg(args, xu_long_t);
        stream->timeout   = timeout ? timeout : XU_STREAM_DEFAULT_TIMEOUT;
        ok                = xu_true;
    }
    break;
    case XU_STREAM_CTRL_GET_TIMEOUT:
    {
        // get timeout
        xu_long_t* ptimeout = (xu_long_t*)xu_va_arg(args, xu_long_t*);
        if (ptimeout)
        {
            *ptimeout = stream->timeout;
            ok        = xu_true;
        }
    }
    break;
    default: break;
    }

    // restore args
    xu_va_copy(args, args_saved);

    // ctrl self
    ok = (stream->ctrl(self, ctrl, args) || ok) ? xu_true : xu_false;

    // ok?
    return ok;
}
xu_void_t xu_stream_kill(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return(stream);

    // trace
    xu_trace_d("kill: %s: state: %s: ..", xu_url_cstr(&stream->url), xu_state_cstr(xu_atomic32_get(&stream->istate)));

    // opened? kill it
    if (XU_STATE_OPENED == xu_atomic32_fetch_and_cmpset(&stream->istate, XU_STATE_OPENED, XU_STATE_KILLING))
    {
        // kill it
        if (stream->kill) stream->kill(self);

        // trace
        xu_trace_d("kill: %s: ok", xu_url_cstr(&stream->url));
    }
    // opening? kill it
    else if (XU_STATE_OPENING == xu_atomic32_fetch_and_cmpset(&stream->istate, XU_STATE_OPENING, XU_STATE_KILLING))
    {
        // kill it
        if (stream->kill) stream->kill(self);

        // trace
        xu_trace_d("kill: %s: ok", xu_url_cstr(&stream->url));
    }
    else
    {
        // closed? killed
        xu_atomic32_fetch_and_cmpset(&stream->istate, XU_STATE_CLOSED, XU_STATE_KILLED);
    }
}
xu_bool_t xu_stream_open(xu_stream_ref_t self)
{
    // check self
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream && stream->open, xu_false);

    // already been opened?
    xu_check_return_val(!xu_stream_is_opened(self), xu_true);

    // closed?
    xu_assert_and_check_return_val(XU_STATE_CLOSED == xu_atomic32_get(&stream->istate), xu_false);

    // init offset
    stream->offset = 0;

    // init state
    stream->state = XU_STATE_OK;

    // open it
    xu_bool_t ok = stream->open(self);

    // opened
    if (ok) xu_atomic32_set(&stream->istate, XU_STATE_OPENED);

    // ok?
    return ok;
}
xu_bool_t xu_stream_clos(xu_stream_ref_t self)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, xu_false);

    // already been closed?
    xu_check_return_val(xu_stream_is_opened(self), xu_true);

    // flush writted data first
    if (stream->bwritted) xu_stream_sync(self, xu_true);

    // has close?
    if (stream->clos && !stream->clos(self)) return xu_false;

    // clear state
    stream->offset   = 0;
    stream->bwritted = 0;
    stream->state    = XU_STATE_OK;
    xu_atomic32_set(&stream->istate, XU_STATE_CLOSED);

    // clear cache
    xu_queue_buffer_clear(&stream->cache);

    // ok
    return xu_true;
}
xu_bool_t xu_stream_need(xu_stream_ref_t self, xu_byte_t** data, xu_size_t size)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(data && size, xu_false);

    // check self
    xu_assert_and_check_return_val(stream && xu_stream_is_opened(self) && stream->read && stream->wait, xu_false);

    // stoped?
    xu_assert_and_check_return_val(XU_STATE_OPENED == xu_atomic32_get(&stream->istate), xu_false);

    // have writted cache? sync first
    if (stream->bwritted && !xu_queue_buffer_null(&stream->cache) && !xu_stream_sync(self, xu_false)) return xu_false;

    // switch to the read cache mode
    if (stream->bwritted && xu_queue_buffer_null(&stream->cache)) stream->bwritted = 0;

    // check the cache mode, must be read cache
    xu_assert_and_check_return_val(!stream->bwritted, xu_false);

    // not enough? grow the cache first
    if (xu_queue_buffer_maxn(&stream->cache) < size) xu_queue_buffer_resize(&stream->cache, size);

    // check
    xu_assert_and_check_return_val(xu_queue_buffer_maxn(&stream->cache) && size <= xu_queue_buffer_maxn(&stream->cache),
                                   xu_false);

    // enough?
    if (size <= xu_queue_buffer_size(&stream->cache))
    {
        // save data
        *data = xu_queue_buffer_head(&stream->cache);
        return xu_true;
    }

    // enter cache for push
    xu_size_t  push = 0;
    xu_size_t  need = size - xu_queue_buffer_size(&stream->cache);
    xu_byte_t* tail = xu_queue_buffer_push_init(&stream->cache, &push);
    xu_assert_and_check_return_val(tail && push, xu_false);
    if (push > need) push = need;

    // fill cache
    xu_size_t read = 0;
    while (read < push && (XU_STATE_OPENED == xu_atomic32_get(&stream->istate)))
    {
        // read data
        xu_long_t real = stream->read(self, tail + read, push - read);

        // ok?
        if (real > 0)
        {
            // save read
            read += real;
        }
        // no data?
        else if (!real)
        {
            // wait
            real = stream->wait(self, XU_STREAM_WAIT_READ, xu_stream_timeout(self));

            // ok?
            xu_check_break(real > 0);
        }
        else
            break;
    }

    // leave cache for push
    xu_queue_buffer_push_exit(&stream->cache, read);

    // not enough?
    if (size > xu_queue_buffer_size(&stream->cache))
    {
        // killed? save state
        if (!stream->state && (XU_STATE_KILLING == xu_atomic32_get(&stream->istate))) stream->state = XU_STATE_KILLED;

        // failed
        return xu_false;
    }

    // save data
    *data = xu_queue_buffer_head(&stream->cache);

    // ok
    return xu_true;
}
xu_long_t xu_stream_peek(xu_stream_ref_t self, xu_byte_t** data, xu_size_t size)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(data && size, -1);

    // check self
    xu_assert_and_check_return_val(stream && xu_stream_is_opened(self) && stream->read && stream->wait, -1);

    // stoped?
    xu_assert_and_check_return_val(XU_STATE_OPENED == xu_atomic32_get(&stream->istate), -1);

    // have writted cache? sync first
    if (stream->bwritted && !xu_queue_buffer_null(&stream->cache) && !xu_stream_sync(self, xu_false)) return -1;

    // switch to the read cache mode
    if (stream->bwritted && xu_queue_buffer_null(&stream->cache)) stream->bwritted = 0;

    // check the cache mode, must be read cache
    xu_assert_and_check_return_val(!stream->bwritted, -1);

    // not enough? grow the cache first
    if (xu_queue_buffer_maxn(&stream->cache) < size) xu_queue_buffer_resize(&stream->cache, size);

    // check
    xu_assert_and_check_return_val(xu_queue_buffer_maxn(&stream->cache) && size <= xu_queue_buffer_maxn(&stream->cache),
                                   -1);

    // attempt to peek data from cache directly?
    xu_size_t cached_size = xu_queue_buffer_size(&stream->cache);
    if (cached_size)
    {
        *data = xu_queue_buffer_head(&stream->cache);
        return xu_min(size, cached_size);
    }

    // cache is null now.
    xu_assert(xu_queue_buffer_null(&stream->cache));

    // enter cache for push
    xu_size_t  push = 0;
    xu_byte_t* tail = xu_queue_buffer_push_init(&stream->cache, &push);
    xu_assert_and_check_return_val(tail && push, -1);

    // push data to cache from self
    xu_assert(stream->read);
    xu_long_t real = stream->read(self, tail, push);
    xu_check_return_val(real >= 0, -1);

    // peek data from cache
    if (real > 0)
    {
        // leave cache for push
        xu_queue_buffer_push_exit(&stream->cache, real);

        // peek data from cache again
        *data = xu_queue_buffer_head(&stream->cache);
        return xu_min(size, real);
    }
    // need wait
    else
        return 0;
}
xu_long_t xu_stream_read(xu_stream_ref_t self, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(data, -1);

    // no size?
    xu_check_return_val(size, 0);

    // check self
    xu_assert_and_check_return_val(stream && xu_stream_is_opened(self) && stream->read, -1);

    // done
    xu_long_t read = 0;
    do
    {
        if (xu_queue_buffer_maxn(&stream->cache))
        {
            // switch to the read cache mode
            if (stream->bwritted && xu_queue_buffer_null(&stream->cache)) stream->bwritted = 0;

            // check the cache mode, must be read cache
            xu_assert_and_check_return_val(!stream->bwritted, -1);

            // read data from cache first
            read = xu_queue_buffer_read(&stream->cache, data, size);
            xu_check_return_val(read >= 0, -1);

            // ok?
            xu_check_break(!read);

            // cache is null now.
            xu_assert_and_check_return_val(xu_queue_buffer_null(&stream->cache), -1);

            // enter cache for push
            xu_size_t  push = 0;
            xu_byte_t* tail = xu_queue_buffer_push_init(&stream->cache, &push);
            xu_assert_and_check_return_val(tail && push, -1);

            // push data to cache from self
            xu_assert(stream->read);
            xu_long_t real = stream->read(self, tail, push);
            xu_check_return_val(real >= 0, -1);

            // read the left data from cache
            if (real > 0)
            {
                // leave cache for push
                xu_queue_buffer_push_exit(&stream->cache, real);

                // read cache
                real = xu_queue_buffer_read(&stream->cache, data + read, xu_min(real, size - read));
                xu_check_return_val(real >= 0, -1);

                // save read
                read += real;
            }
        }
        else
        {
            // read it directly
            read = stream->read(self, data, size);
            xu_check_return_val(read >= 0, -1);
        }
    } while (0);

    // update offset
    stream->offset += read;

    //  xu_trace_d("read: %d", read);
    return read;
}
xu_long_t xu_stream_writ(xu_stream_ref_t self, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(data, -1);

    // no size?
    xu_check_return_val(size, 0);

    // check self
    xu_assert_and_check_return_val(stream && xu_stream_is_opened(self) && stream->writ, -1);

    // done
    xu_long_t writ = 0;
    do
    {
        if (xu_queue_buffer_maxn(&stream->cache))
        {
            // switch to the writ cache mode
            if (!stream->bwritted && xu_queue_buffer_null(&stream->cache)) stream->bwritted = 1;

            // check the cache mode, must be writ cache
            xu_assert_and_check_return_val(stream->bwritted, -1);

            // writ data to cache first
            writ = xu_queue_buffer_writ(&stream->cache, data, size);
            xu_check_return_val(writ >= 0, -1);

            // ok?
            xu_check_break(!writ);

            // cache is full now.
            xu_assert_and_check_return_val(xu_queue_buffer_full(&stream->cache), -1);

            // enter cache for pull
            xu_size_t  pull = 0;
            xu_byte_t* head = xu_queue_buffer_pull_init(&stream->cache, &pull);
            xu_assert_and_check_return_val(head && pull, -1);

            // pull data to self from cache
            xu_long_t real = stream->writ(self, head, pull);
            xu_check_return_val(real >= 0, -1);

            // writ the left data to cache
            if (real > 0)
            {
                // leave cache for pull
                xu_queue_buffer_pull_exit(&stream->cache, real);

                // writ cache
                real = xu_queue_buffer_writ(&stream->cache, data + writ, xu_min(real, size - writ));
                xu_check_return_val(real >= 0, -1);

                // save writ
                writ += real;
            }
        }
        else
        {
            // writ it directly
            writ = stream->writ(self, data, size);
            xu_check_return_val(writ >= 0, -1);
        }

    } while (0);

    // update offset
    stream->offset += writ;

    //  xu_trace_d("writ: %d", writ);
    return writ;
}
xu_bool_t xu_stream_bread(xu_stream_ref_t self, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream && data, xu_false);
    xu_check_return_val(size, xu_true);

    // have writted cache? sync first
    if (stream->bwritted && !xu_queue_buffer_null(&stream->cache) && !xu_stream_sync(self, xu_false)) return xu_false;

    // check the left
    xu_hize_t left = xu_stream_left(self);
    xu_check_return_val(size <= left, xu_false);

    // read data from cache
    xu_long_t read = 0;
    while (read < size && (XU_STATE_OPENED == xu_atomic32_get(&stream->istate)))
    {
        // read data
        xu_long_t real = xu_stream_read(self, data + read, xu_min(size - read, XU_STREAM_BLOCK_MAXN));
        if (real > 0)
            read += real;
        else if (!real)
        {
            // wait
            real = xu_stream_wait(self, XU_STREAM_WAIT_READ, xu_stream_timeout(self));
            xu_check_break(real > 0);

            // has read?
            xu_assert_and_check_break(real & XU_STREAM_WAIT_READ);
        }
        else
            break;
    }

    // killed? save state
    if (read != size && !stream->state && (XU_STATE_KILLING == xu_atomic32_get(&stream->istate)))
        stream->state = XU_STATE_KILLED;

    // ok?
    return (read == size ? xu_true : xu_false);
}
xu_bool_t xu_stream_bwrit(xu_stream_ref_t self, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream && data, xu_false);
    xu_check_return_val(size, xu_true);

    // writ data to cache
    xu_long_t writ = 0;
    while (writ < size && (XU_STATE_OPENED == xu_atomic32_get(&stream->istate)))
    {
        // writ data
        xu_long_t real = xu_stream_writ(self, data + writ, xu_min(size - writ, XU_STREAM_BLOCK_MAXN));
        if (real > 0)
            writ += real;
        else if (!real)
        {
            // wait
            real = xu_stream_wait(self, XU_STREAM_WAIT_WRIT, xu_stream_timeout(self));
            xu_check_break(real > 0);

            // has writ?
            xu_assert_and_check_break(real & XU_STREAM_WAIT_WRIT);
        }
        else
            break;
    }

    // killed? save state
    if (writ != size && !stream->state && (XU_STATE_KILLING == xu_atomic32_get(&stream->istate)))
        stream->state = XU_STATE_KILLED;

    // ok?
    return (writ == size ? xu_true : xu_false);
}
xu_bool_t xu_stream_sync(xu_stream_ref_t self, xu_bool_t bclosing)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream && stream->writ && stream->wait && xu_stream_is_opened(self), xu_false);

    // stoped?
    xu_assert_and_check_return_val((XU_STATE_OPENED == xu_atomic32_get(&stream->istate)), xu_false);

    // cached? sync cache first
    if (xu_queue_buffer_maxn(&stream->cache))
    {
        // have data?
        if (!xu_queue_buffer_null(&stream->cache))
        {
            // check: must be writted cache
            xu_assert_and_check_return_val(stream->bwritted, xu_false);

            // enter cache for pull
            xu_size_t  size = 0;
            xu_byte_t* head = xu_queue_buffer_pull_init(&stream->cache, &size);
            xu_assert_and_check_return_val(head && size, xu_false);

            // writ cache data to self
            xu_size_t writ = 0;
            while (writ < size && (XU_STATE_OPENED == xu_atomic32_get(&stream->istate)))
            {
                // writ
                xu_long_t real = stream->writ(self, head + writ, size - writ);

                // ok?
                if (real > 0)
                {
                    // save writ
                    writ += real;
                }
                // no data?
                else if (!real)
                {
                    // wait
                    real = stream->wait(self, XU_STREAM_WAIT_WRIT, xu_stream_timeout(self));

                    // ok?
                    xu_check_break(real > 0);
                }
                // error or end?
                else
                    break;
            }

            // leave cache for pull
            xu_queue_buffer_pull_exit(&stream->cache, writ);

            // cache be not cleared?
            if (!xu_queue_buffer_null(&stream->cache))
            {
                // killed? save state
                if (!stream->state && (XU_STATE_KILLING == xu_atomic32_get(&stream->istate)))
                    stream->state = XU_STATE_KILLED;

                // failed
                return xu_false;
            }
        }
        else
            stream->bwritted = 1;
    }

    // sync
    return stream->sync ? stream->sync(self, bclosing) : xu_true;
}
xu_bool_t xu_stream_seek(xu_stream_ref_t self, xu_hize_t offset)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream && xu_stream_is_opened(self), xu_false);

    // stoped?
    xu_assert_and_check_return_val((XU_STATE_OPENED == xu_atomic32_get(&stream->istate)), xu_false);

    // sync writted data first, @note must be called before xu_stream_size()
    if (stream->bwritted && !xu_stream_sync(self, xu_false)) return xu_false;

    // limit offset
    xu_hong_t size = xu_stream_size(self);
    if (size >= 0 && offset > size) offset = size;

    // the offset be not changed?
    xu_hize_t curt = xu_stream_offset(self);
    xu_check_return_val(offset != curt, xu_true);

    // for writing
    if (stream->bwritted)
    {
        // check cache, must not cache or empty cache
        xu_assert_and_check_return_val(!xu_queue_buffer_maxn(&stream->cache) || xu_queue_buffer_null(&stream->cache),
                                       xu_false);

        // seek it
        xu_bool_t ok = stream->seek ? stream->seek(self, offset) : xu_false;

        // save offset
        if (ok) stream->offset = offset;
    }
    // for reading
    else
    {
        // cached? try to seek it at the cache
        xu_bool_t ok = xu_false;
        if (xu_queue_buffer_maxn(&stream->cache))
        {
            xu_size_t  pull = 0;
            xu_byte_t* data = xu_queue_buffer_pull_init(&stream->cache, &pull);
            if (data && pull && offset > curt && offset <= curt + pull)
            {
                // seek it at the cache
                xu_queue_buffer_pull_exit(&stream->cache, (xu_size_t)(offset - curt));

                // save offset
                stream->offset = offset;

                // ok
                ok = xu_true;
            }
        }

        // seek it
        if (!ok)
        {
            // seek it
            ok = stream->seek ? stream->seek(self, offset) : xu_false;

            // ok?
            if (ok)
            {
                // save offset
                stream->offset = offset;

                // clear cache
                xu_queue_buffer_clear(&stream->cache);
            }
        }

        // try to read and seek
        if (!ok && offset > curt)
        {
            // read some data for updating offset
            xu_byte_t data[XU_STREAM_BLOCK_MAXN];
            while (xu_stream_offset(self) != offset)
            {
                xu_size_t need = (xu_size_t)xu_min(offset - curt, XU_STREAM_BLOCK_MAXN);
                if (!xu_stream_bread(self, data, need)) return xu_false;
            }
        }
    }

    // ok?
    return xu_stream_offset(self) == offset ? xu_true : xu_false;
}
xu_bool_t xu_stream_skip(xu_stream_ref_t self, xu_hize_t size)
{
    return xu_stream_seek(self, xu_stream_offset(self) + size);
}
xu_long_t xu_stream_bread_line(xu_stream_ref_t self, xu_char_t* data, xu_size_t size)
{
    // check
    xu_stream_t* stream = xu_stream_cast(self);
    xu_assert_and_check_return_val(stream, -1);

    // init static buffer
    xu_static_buffer_t buffer;
    if (!xu_static_buffer_init(&buffer, (xu_byte_t*)data, size)) return -1;

    // read line
    xu_bool_t  eof  = xu_false;
    xu_byte_t* line = xu_null;
    while ((XU_STATE_OPENED == xu_atomic32_get(&stream->istate)))
    {
        xu_long_t real = xu_stream_peek(self, &line, xu_min(size, XU_STREAM_BLOCK_MAXN));
        if (real > 0)
        {
            xu_assert(real <= size);
            xu_char_t const* e = xu_strnchr((xu_char_t const*)line, real, '\n');
            if (e)
            {
                xu_size_t n = (xu_byte_t const*)e + 1 - line;
                if (!xu_stream_skip(self, n)) return -1;
                xu_static_buffer_memncat(&buffer, line, n);
                break;
            }
            else
            {
                if (!xu_stream_skip(self, real)) return -1;
                xu_static_buffer_memncat(&buffer, line, real);
            }
        }
        else if (!real)
        {
            real = xu_stream_wait(self, XU_STREAM_WAIT_READ, xu_stream_timeout(self));
            if (real <= 0)
            {
                eof = xu_true;
                break;
            }
        }
        else
        {
            eof = xu_true;
            break;
        }
    }

    // killed?
    if ((XU_STATE_KILLING == xu_atomic32_get(&stream->istate))) return -1;

    // ok?
    xu_size_t linesize = xu_static_buffer_size(&buffer);
    if (linesize)
    {
        if (linesize && data[linesize - 1] == '\n') linesize--;
        if (linesize && data[linesize - 1] == '\r') linesize--;
        data[linesize] = '\0';
        return linesize;
    }
    else
        return (eof || xu_stream_beof(self)) ? -1 : 0;
}
xu_long_t xu_stream_bwrit_line(xu_stream_ref_t self, xu_char_t* data, xu_size_t size)
{
    // writ data
    xu_long_t writ = 0;
    if (size)
    {
        if (!xu_stream_bwrit(self, (xu_byte_t*)data, size)) return -1;
    }
    else
    {
        xu_char_t* p = data;
        while (*p)
        {
            if (!xu_stream_bwrit(self, (xu_byte_t*)p, 1)) return -1;
            p++;
        }

        writ = p - data;
    }

    // writ "\n"
    xu_char_t le[] = "\n";
    xu_size_t ln   = 1;
    if (!xu_stream_bwrit(self, (xu_byte_t*)le, ln)) return -1;
    writ += ln;

    // ok
    return writ;
}
xu_long_t xu_stream_printf(xu_stream_ref_t self, xu_char_t const* fmt, ...)
{
    // init data
    xu_char_t data[XU_STREAM_BLOCK_MAXN] = {0};
    xu_size_t size                       = 0;

    // format data
    xu_vsnprintf_format(data, XU_STREAM_BLOCK_MAXN, fmt, &size);
    xu_check_return_val(size, 0);

    // writ data
    return xu_stream_bwrit(self, (xu_byte_t*)data, size) ? size : -1;
}
xu_byte_t* xu_stream_bread_all(xu_stream_ref_t self, xu_bool_t is_cstr, xu_size_t* psize)
{
    // attempt to get self size
    xu_bool_t  ok   = xu_false;
    xu_byte_t* data = xu_null;
    xu_hong_t  size = xu_stream_size(self);
    do
    {
        // has size?
        if (size > 0)
        {
            // check
            xu_assert(size < XU_MAXS32);

            // make data
            data = xu_malloc_bytes((xu_size_t)(is_cstr ? size + 1 : size));
            xu_assert_and_check_break(data);

            // read data
            if (!xu_stream_bread(self, data, (xu_size_t)size)) break;

            // append '\0' if be c-string
            if (is_cstr) data[size] = '\0';

            // save size
            if (psize) *psize = (xu_size_t)size;

            // ok
            ok = xu_true;
        }
        // no size?
        else
        {
            // init maxn
            xu_size_t maxn = XU_STREAM_BLOCK_MAXN;

            // make data
            data = xu_malloc_bytes(is_cstr ? maxn + 1 : maxn);
            xu_assert_and_check_break(data);

            // done
            xu_long_t read = 0;
            while (!xu_stream_beof(self))
            {
                // space is too small? grow it first
                if (maxn - read < XU_STREAM_BLOCK_MAXN)
                {
                    // grow maxn
                    maxn = xu_max(maxn << 1, maxn + XU_STREAM_BLOCK_MAXN);

                    // grow data
                    data = (xu_byte_t*)xu_ralloc(data, is_cstr ? maxn + 1 : maxn);
                    xu_assert_and_check_break(data);
                }

                // read data
                xu_long_t real = xu_stream_read(self, data + read, maxn - read);

                // ok?
                if (real > 0)
                {
                    // update size
                    read += real;
                }
                // no data? continue it
                else if (!real)
                {
                    // wait
                    real = xu_stream_wait(self, XU_STREAM_WAIT_READ, xu_stream_timeout(self));
                    xu_check_break(real > 0);

                    // has read?
                    xu_assert_and_check_break(real & XU_STREAM_WAIT_READ);
                }
                // failed or end?
                else
                    break;
            }

            // check
            xu_assert_and_check_break(data && read <= maxn);

            // append '\0' if be c-string
            if (is_cstr) data[read] = '\0';

            // save size
            if (psize) *psize = read;

            // ok
            ok = xu_true;
        }

    } while (0);

    // failed?
    if (!ok)
    {
        // exit data
        if (data) xu_free(data);
        data = xu_null;
    }

    // ok?
    return data;
}
xu_bool_t xu_stream_bread_u8(xu_stream_ref_t self, xu_uint8_t* pvalue)
{
    return xu_stream_bread(self, (xu_byte_t*)pvalue, 1);
}
xu_bool_t xu_stream_bread_s8(xu_stream_ref_t self, xu_sint8_t* pvalue)
{
    return xu_stream_bread(self, (xu_byte_t*)pvalue, 1);
}
xu_bool_t xu_stream_bread_u16_le(xu_stream_ref_t self, xu_uint16_t* pvalue)
{
    // read data
    xu_byte_t b[2];
    xu_bool_t ok = xu_stream_bread(self, b, 2);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_u16_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_s16_le(xu_stream_ref_t self, xu_sint16_t* pvalue)
{
    // read data
    xu_byte_t b[2];
    xu_bool_t ok = xu_stream_bread(self, b, 2);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_s16_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_u24_le(xu_stream_ref_t self, xu_uint32_t* pvalue)
{
    // read data
    xu_byte_t b[3];
    xu_bool_t ok = xu_stream_bread(self, b, 3);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_u24_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_s24_le(xu_stream_ref_t self, xu_sint32_t* pvalue)
{
    // read data
    xu_byte_t b[3];
    xu_bool_t ok = xu_stream_bread(self, b, 3);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_s24_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_u32_le(xu_stream_ref_t self, xu_uint32_t* pvalue)
{
    // read data
    xu_byte_t b[4];
    xu_bool_t ok = xu_stream_bread(self, b, 4);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_u32_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_s32_le(xu_stream_ref_t self, xu_sint32_t* pvalue)
{
    // read data
    xu_byte_t b[4];
    xu_bool_t ok = xu_stream_bread(self, b, 4);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_s32_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_u64_le(xu_stream_ref_t self, xu_uint64_t* pvalue)
{
    // read data
    xu_byte_t b[8];
    xu_bool_t ok = xu_stream_bread(self, b, 8);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_u64_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_s64_le(xu_stream_ref_t self, xu_sint64_t* pvalue)
{
    // read data
    xu_byte_t b[8];
    xu_bool_t ok = xu_stream_bread(self, b, 8);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_s64_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_u16_be(xu_stream_ref_t self, xu_uint16_t* pvalue)
{
    // read data
    xu_byte_t b[2];
    xu_bool_t ok = xu_stream_bread(self, b, 2);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_u16_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_s16_be(xu_stream_ref_t self, xu_sint16_t* pvalue)
{
    // read data
    xu_byte_t b[2];
    xu_bool_t ok = xu_stream_bread(self, b, 2);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_s16_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_u24_be(xu_stream_ref_t self, xu_uint32_t* pvalue)
{
    // read data
    xu_byte_t b[3];
    xu_bool_t ok = xu_stream_bread(self, b, 3);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_u24_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_s24_be(xu_stream_ref_t self, xu_sint32_t* pvalue)
{
    // read data
    xu_byte_t b[3];
    xu_bool_t ok = xu_stream_bread(self, b, 3);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_s24_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_u32_be(xu_stream_ref_t self, xu_uint32_t* pvalue)
{
    // read data
    xu_byte_t b[4];
    xu_bool_t ok = xu_stream_bread(self, b, 4);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_u32_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_s32_be(xu_stream_ref_t self, xu_sint32_t* pvalue)
{
    // read data
    xu_byte_t b[4];
    xu_bool_t ok = xu_stream_bread(self, b, 4);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_s32_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_u64_be(xu_stream_ref_t self, xu_uint64_t* pvalue)
{
    // read data
    xu_byte_t b[8];
    xu_bool_t ok = xu_stream_bread(self, b, 8);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_u64_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_s64_be(xu_stream_ref_t self, xu_sint64_t* pvalue)
{
    // read data
    xu_byte_t b[8];
    xu_bool_t ok = xu_stream_bread(self, b, 8);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_s64_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bwrit_u8(xu_stream_ref_t self, xu_uint8_t value)
{
    xu_byte_t b[1];
    xu_bits_set_u8(b, value);
    return xu_stream_bwrit(self, b, 1);
}
xu_bool_t xu_stream_bwrit_s8(xu_stream_ref_t self, xu_sint8_t value)
{
    xu_byte_t b[1];
    xu_bits_set_s8(b, value);
    return xu_stream_bwrit(self, b, 1);
}
xu_bool_t xu_stream_bwrit_u16_le(xu_stream_ref_t self, xu_uint16_t value)
{
    xu_byte_t b[2];
    xu_bits_set_u16_le(b, value);
    return xu_stream_bwrit(self, b, 2);
}
xu_bool_t xu_stream_bwrit_s16_le(xu_stream_ref_t self, xu_sint16_t value)
{
    xu_byte_t b[2];
    xu_bits_set_s16_le(b, value);
    return xu_stream_bwrit(self, b, 2);
}
xu_bool_t xu_stream_bwrit_u24_le(xu_stream_ref_t self, xu_uint32_t value)
{
    xu_byte_t b[3];
    xu_bits_set_u24_le(b, value);
    return xu_stream_bwrit(self, b, 3);
}
xu_bool_t xu_stream_bwrit_s24_le(xu_stream_ref_t self, xu_sint32_t value)
{
    xu_byte_t b[3];
    xu_bits_set_s24_le(b, value);
    return xu_stream_bwrit(self, b, 3);
}
xu_bool_t xu_stream_bwrit_u32_le(xu_stream_ref_t self, xu_uint32_t value)
{
    xu_byte_t b[4];
    xu_bits_set_u32_le(b, value);
    return xu_stream_bwrit(self, b, 4);
}
xu_bool_t xu_stream_bwrit_s32_le(xu_stream_ref_t self, xu_sint32_t value)
{
    xu_byte_t b[4];
    xu_bits_set_s32_le(b, value);
    return xu_stream_bwrit(self, b, 4);
}
xu_bool_t xu_stream_bwrit_u64_le(xu_stream_ref_t self, xu_uint64_t value)
{
    xu_byte_t b[8];
    xu_bits_set_u64_le(b, value);
    return xu_stream_bwrit(self, b, 8);
}
xu_bool_t xu_stream_bwrit_s64_le(xu_stream_ref_t self, xu_sint64_t value)
{
    xu_byte_t b[8];
    xu_bits_set_s64_le(b, value);
    return xu_stream_bwrit(self, b, 8);
}
xu_bool_t xu_stream_bwrit_u16_be(xu_stream_ref_t self, xu_uint16_t value)
{
    xu_byte_t b[2];
    xu_bits_set_u16_be(b, value);
    return xu_stream_bwrit(self, b, 2);
}
xu_bool_t xu_stream_bwrit_s16_be(xu_stream_ref_t self, xu_sint16_t value)
{
    xu_byte_t b[2];
    xu_bits_set_s16_be(b, value);
    return xu_stream_bwrit(self, b, 2);
}
xu_bool_t xu_stream_bwrit_u24_be(xu_stream_ref_t self, xu_uint32_t value)
{
    xu_byte_t b[3];
    xu_bits_set_u24_be(b, value);
    return xu_stream_bwrit(self, b, 3);
}
xu_bool_t xu_stream_bwrit_s24_be(xu_stream_ref_t self, xu_sint32_t value)
{
    xu_byte_t b[3];
    xu_bits_set_s24_be(b, value);
    return xu_stream_bwrit(self, b, 3);
}
xu_bool_t xu_stream_bwrit_u32_be(xu_stream_ref_t self, xu_uint32_t value)
{
    xu_byte_t b[4];
    xu_bits_set_u32_be(b, value);
    return xu_stream_bwrit(self, b, 4);
}
xu_bool_t xu_stream_bwrit_s32_be(xu_stream_ref_t self, xu_sint32_t value)
{
    xu_byte_t b[4];
    xu_bits_set_s32_be(b, value);
    return xu_stream_bwrit(self, b, 4);
}
xu_bool_t xu_stream_bwrit_u64_be(xu_stream_ref_t self, xu_uint64_t value)
{
    xu_byte_t b[8];
    xu_bits_set_u64_be(b, value);
    return xu_stream_bwrit(self, b, 8);
}
xu_bool_t xu_stream_bwrit_s64_be(xu_stream_ref_t self, xu_sint64_t value)
{
    xu_byte_t b[8];
    xu_bits_set_s64_be(b, value);
    return xu_stream_bwrit(self, b, 8);
}
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
xu_bool_t xu_stream_bread_float_le(xu_stream_ref_t self, xu_float_t* pvalue)
{
    // read data
    xu_byte_t b[4];
    xu_bool_t ok = xu_stream_bread(self, b, 4);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_float_le(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_float_be(xu_stream_ref_t self, xu_float_t* pvalue)
{
    // read data
    xu_byte_t b[4];
    xu_bool_t ok = xu_stream_bread(self, b, 4);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_float_be(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_double_ble(xu_stream_ref_t self, xu_double_t* pvalue)
{
    // read data
    xu_byte_t b[8];
    xu_bool_t ok = xu_stream_bread(self, b, 8);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_double_ble(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_double_bbe(xu_stream_ref_t self, xu_double_t* pvalue)
{
    // read data
    xu_byte_t b[8];
    xu_bool_t ok = xu_stream_bread(self, b, 8);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_double_bbe(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_double_lle(xu_stream_ref_t self, xu_double_t* pvalue)
{
    // read data
    xu_byte_t b[8];
    xu_bool_t ok = xu_stream_bread(self, b, 8);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_double_lle(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bread_double_lbe(xu_stream_ref_t self, xu_double_t* pvalue)
{
    // read data
    xu_byte_t b[8];
    xu_bool_t ok = xu_stream_bread(self, b, 8);

    // save value
    if (ok && pvalue) *pvalue = xu_bits_get_double_lbe(b);

    // ok?
    return ok;
}
xu_bool_t xu_stream_bwrit_float_le(xu_stream_ref_t self, xu_float_t value)
{
    xu_byte_t b[4];
    xu_bits_set_float_le(b, value);
    return xu_stream_bwrit(self, b, 4);
}
xu_bool_t xu_stream_bwrit_float_be(xu_stream_ref_t self, xu_float_t value)
{
    xu_byte_t b[4];
    xu_bits_set_float_be(b, value);
    return xu_stream_bwrit(self, b, 4);
}
xu_bool_t xu_stream_bwrit_double_ble(xu_stream_ref_t self, xu_double_t value)
{
    xu_byte_t b[8];
    xu_bits_set_double_ble(b, value);
    return xu_stream_bwrit(self, b, 8);
}
xu_bool_t xu_stream_bwrit_double_bbe(xu_stream_ref_t self, xu_double_t value)
{
    xu_byte_t b[8];
    xu_bits_set_double_bbe(b, value);
    return xu_stream_bwrit(self, b, 8);
}
xu_bool_t xu_stream_bwrit_double_lle(xu_stream_ref_t self, xu_double_t value)
{
    xu_byte_t b[8];
    xu_bits_set_double_lle(b, value);
    return xu_stream_bwrit(self, b, 8);
}
xu_bool_t xu_stream_bwrit_double_lbe(xu_stream_ref_t self, xu_double_t value)
{
    xu_byte_t b[8];
    xu_bits_set_double_lbe(b, value);
    return xu_stream_bwrit(self, b, 8);
}

#endif
