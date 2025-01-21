#include "xutil/network/http.h"
#include "xutil/stream/prefix.h"
#include "xutil/stream/stream.h"
#include "xutil/utils/state.h"

/* *******************************************************
 * types
 * *******************************************************
 */

// the http stream type
typedef struct __xu_stream_http_t
{
    // the http
    xu_http_ref_t http;

} xu_stream_http_t;

/* *******************************************************
 * implementation
 * *******************************************************
 */

static __xu_inline__ xu_stream_http_t* xu_stream_http_cast(xu_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && xu_stream_type(stream) == XU_STREAM_TYPE_HTTP, xu_null);

    // ok?
    return (xu_stream_http_t*)stream;
}
static xu_bool_t xu_stream_http_open(xu_stream_ref_t stream)
{
    // check
    xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
    xu_assert_and_check_return_val(stream_http && stream_http->http, xu_false);

    // the http status
    xu_http_status_t const* status = xu_http_status(stream_http->http);
    xu_assert_and_check_return_val(status, xu_false);

    // open it
    xu_bool_t ok = xu_http_open(stream_http->http);

    // save state
    xu_stream_state_set(stream, ok ? XU_STATE_OK : status->state);

    // ok?
    return ok;
}
static xu_bool_t xu_stream_http_clos(xu_stream_ref_t stream)
{
    // check
    xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
    xu_assert_and_check_return_val(stream_http && stream_http->http, xu_false);

    // close it
    return xu_http_close(stream_http->http);
}
static xu_void_t xu_stream_http_exit(xu_stream_ref_t stream)
{
    xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
    if (stream_http && stream_http->http) xu_http_exit(stream_http->http);
}
static xu_void_t xu_stream_http_kill(xu_stream_ref_t stream)
{
    xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
    if (stream_http && stream_http->http) xu_http_kill(stream_http->http);
}
static xu_long_t xu_stream_http_read(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
    xu_assert_and_check_return_val(stream_http && stream_http->http, -1);

    // the http status
    xu_http_status_t const* status = xu_http_status(stream_http->http);
    xu_assert_and_check_return_val(status, -1);

    // check
    xu_check_return_val(data, -1);
    xu_check_return_val(size, 0);

    // read data
    xu_long_t ok = xu_http_read(stream_http->http, data, size);

    // save state
    xu_stream_state_set(stream, ok >= 0 ? XU_STATE_OK : status->state);

    // ok?
    return ok;
}
static xu_bool_t xu_stream_http_seek(xu_stream_ref_t stream, xu_hize_t offset)
{
    // check
    xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
    xu_assert_and_check_return_val(stream_http && stream_http->http, xu_false);

    // seek
    return xu_http_seek(stream_http->http, offset);
}
static xu_long_t xu_stream_http_wait(xu_stream_ref_t stream, xu_size_t wait, xu_long_t timeout)
{
    // check
    xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
    xu_assert_and_check_return_val(stream_http && stream_http->http, -1);

    // the http status
    xu_http_status_t const* status = xu_http_status(stream_http->http);
    xu_assert_and_check_return_val(status, -1);

    // wait
    xu_long_t ok = xu_http_wait(stream_http->http, wait, timeout);

    // save state
    xu_stream_state_set(stream, ok >= 0 ? XU_STATE_OK : status->state);

    // ok?
    return ok;
}
static xu_bool_t xu_stream_http_ctrl(xu_stream_ref_t stream, xu_size_t ctrl, xu_va_list_t args)
{
    // check
    xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
    xu_assert_and_check_return_val(stream_http && stream_http->http, xu_false);

    // done
    switch (ctrl)
    {
    case XU_STREAM_CTRL_GET_SIZE:
    {
        // psize
        xu_hong_t* psize = (xu_hong_t*)xu_va_arg(args, xu_hong_t*);
        xu_assert_and_check_return_val(psize, xu_false);

        // status
        xu_http_status_t const* status = xu_http_status(stream_http->http);
        xu_assert_and_check_return_val(status, 0);

        // get size
        *psize = (!status->bgzip && !status->bdeflate && !status->bchunked) ? status->document_size : -1;
        return xu_true;
    }
    case XU_STREAM_CTRL_SET_URL:
    {
        // url
        xu_char_t const* url = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(url, xu_false);

        // set url
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_URL, url);
    }
    break;
    case XU_STREAM_CTRL_GET_URL:
    {
        // purl
        xu_char_t const** purl = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(purl, xu_false);

        // get url
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_URL, purl);
    }
    break;
    case XU_STREAM_CTRL_SET_HOST:
    {
        // host
        xu_char_t const* host = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(host, xu_false);

        // set host
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_HOST, host);
    }
    break;
    case XU_STREAM_CTRL_GET_HOST:
    {
        // phost
        xu_char_t const** phost = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(phost, xu_false);

        // get host
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_HOST, phost);
    }
    break;
    case XU_STREAM_CTRL_SET_PORT:
    {
        // port
        xu_size_t port = (xu_size_t)xu_va_arg(args, xu_size_t);
        xu_assert_and_check_return_val(port, xu_false);

        // set port
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_PORT, port);
    }
    break;
    case XU_STREAM_CTRL_GET_PORT:
    {
        // pport
        xu_size_t* pport = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pport, xu_false);

        // get port
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_PORT, pport);
    }
    break;
    case XU_STREAM_CTRL_SET_PATH:
    {
        // path
        xu_char_t const* path = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(path, xu_false);

        // set path
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_PATH, path);
    }
    break;
    case XU_STREAM_CTRL_GET_PATH:
    {
        // ppath
        xu_char_t const** ppath = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(ppath, xu_false);

        // get path
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_PATH, ppath);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_METHOD:
    {
        // method
        xu_size_t method = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set method
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_METHOD, method);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_METHOD:
    {
        // pmethod
        xu_size_t* pmethod = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pmethod, xu_false);

        // get method
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_METHOD, pmethod);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_HEAD:
    {
        // key
        xu_char_t const* key = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(key, xu_false);

        // val
        xu_char_t const* val = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(val, xu_false);

        // set head
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_HEAD, key, val);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_HEAD:
    {
        // key
        xu_char_t const* key = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(key, xu_false);

        // pval
        xu_char_t const** pval = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(pval, xu_false);

        // get head
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_HEAD, key, pval);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_HEAD_FUNC:
    {
        // head_func
        xu_http_head_func_t head_func = (xu_http_head_func_t)xu_va_arg(args, xu_http_head_func_t);

        // set head_func
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_HEAD_FUNC, head_func);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_HEAD_FUNC:
    {
        // phead_func
        xu_http_head_func_t* phead_func = (xu_http_head_func_t*)xu_va_arg(args, xu_http_head_func_t*);
        xu_assert_and_check_return_val(phead_func, xu_false);

        // get head_func
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_HEAD_FUNC, phead_func);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_HEAD_PRIV:
    {
        // head_priv
        xu_pointer_t head_priv = (xu_pointer_t)xu_va_arg(args, xu_pointer_t);

        // set head_priv
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_HEAD_PRIV, head_priv);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_HEAD_PRIV:
    {
        // phead_priv
        xu_pointer_t* phead_priv = (xu_pointer_t*)xu_va_arg(args, xu_pointer_t*);
        xu_assert_and_check_return_val(phead_priv, xu_false);

        // get head_priv
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_HEAD_PRIV, phead_priv);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_RANGE:
    {
        xu_hize_t bof = (xu_hize_t)xu_va_arg(args, xu_hize_t);
        xu_hize_t eof = (xu_hize_t)xu_va_arg(args, xu_hize_t);
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_RANGE, bof, eof);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_RANGE:
    {
        // pbof
        xu_hize_t* pbof = (xu_hize_t*)xu_va_arg(args, xu_hize_t*);
        xu_assert_and_check_return_val(pbof, xu_false);

        // peof
        xu_hize_t* peof = (xu_hize_t*)xu_va_arg(args, xu_hize_t*);
        xu_assert_and_check_return_val(peof, xu_false);

        // ok
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_RANGE, pbof, peof);
    }
    break;
    case XU_STREAM_CTRL_SET_SSL:
    {
        // bssl
        xu_bool_t bssl = (xu_bool_t)xu_va_arg(args, xu_bool_t);

        // set ssl
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_SSL, bssl);
    }
    break;
    case XU_STREAM_CTRL_GET_SSL:
    {
        // pssl
        xu_bool_t* pssl = (xu_bool_t*)xu_va_arg(args, xu_bool_t*);
        xu_assert_and_check_return_val(pssl, xu_false);

        // get ssl
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_SSL, pssl);
    }
    break;
    case XU_STREAM_CTRL_SET_TIMEOUT:
    {
        // timeout
        xu_size_t timeout = (xu_size_t)xu_va_arg(args, xu_size_t);
        xu_assert_and_check_return_val(timeout, xu_false);

        // set timeout
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_TIMEOUT, timeout);
    }
    break;
    case XU_STREAM_CTRL_GET_TIMEOUT:
    {
        // ptimeout
        xu_size_t* ptimeout = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(ptimeout, xu_false);

        // get timeout
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_TIMEOUT, ptimeout);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_POST_URL:
    {
        // url
        xu_char_t const* url = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(url, xu_false);

        // set url
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_POST_URL, url);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_POST_URL:
    {
        // purl
        xu_char_t const** purl = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(purl, xu_false);

        // get url
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_POST_URL, purl);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_POST_DATA:
    {
        // post data
        xu_byte_t const* data = (xu_byte_t const*)xu_va_arg(args, xu_byte_t const*);

        // post size
        xu_size_t size = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set data and size
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_POST_DATA, data, size);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_POST_DATA:
    {
        // pdata and psize
        xu_byte_t const** pdata = (xu_byte_t const**)xu_va_arg(args, xu_byte_t const**);
        xu_size_t*        psize = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pdata && psize, xu_false);

        // get post data and size
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_POST_DATA, pdata, psize);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_POST_FUNC:
    {
        // func
        xu_http_post_func_t func = (xu_http_post_func_t)xu_va_arg(args, xu_http_post_func_t);

        // set post func
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_POST_FUNC, func);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_POST_FUNC:
    {
        // pfunc
        xu_http_post_func_t* pfunc = (xu_http_post_func_t*)xu_va_arg(args, xu_http_post_func_t*);
        xu_assert_and_check_return_val(pfunc, xu_false);

        // get post func
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_POST_FUNC, pfunc);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_POST_PRIV:
    {
        // post priv
        xu_cpointer_t priv = (xu_pointer_t)xu_va_arg(args, xu_pointer_t);

        // set post priv
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_POST_PRIV, priv);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_POST_PRIV:
    {
        // ppost priv
        xu_pointer_t* ppriv = (xu_pointer_t*)xu_va_arg(args, xu_pointer_t*);
        xu_assert_and_check_return_val(ppriv, xu_false);

        // get post priv
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_POST_PRIV, ppriv);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_POST_LRATE:
    {
        // post lrate
        xu_size_t lrate = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set post lrate
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_POST_LRATE, lrate);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_POST_LRATE:
    {
        // ppost lrate
        xu_size_t* plrate = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(plrate, xu_false);

        // get post lrate
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_POST_LRATE, plrate);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_AUTO_UNZIP:
    {
        // bunzip
        xu_bool_t bunzip = (xu_bool_t)xu_va_arg(args, xu_bool_t);

        // set bunzip
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_AUTO_UNZIP, bunzip);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_AUTO_UNZIP:
    {
        // pbunzip
        xu_bool_t* pbunzip = (xu_bool_t*)xu_va_arg(args, xu_bool_t*);
        xu_assert_and_check_return_val(pbunzip, xu_false);

        // get bunzip
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_AUTO_UNZIP, pbunzip);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_REDIRECT:
    {
        // redirect
        xu_size_t redirect = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set redirect
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_REDIRECT, redirect);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_REDIRECT:
    {
        // predirect
        xu_size_t* predirect = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(predirect, xu_false);

        // get redirect
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_REDIRECT, predirect);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_VERSION:
    {
        // version
        xu_size_t version = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set version
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_VERSION, version);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_VERSION:
    {
        // pversion
        xu_size_t* pversion = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pversion, xu_false);

        // get version
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_VERSION, pversion);
    }
    break;
    case XU_STREAM_CTRL_HTTP_SET_COOKIES:
    {
        // cookies
        xu_cookies_ref_t cookies = (xu_cookies_ref_t)xu_va_arg(args, xu_cookies_ref_t);

        // set cookies
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_SET_COOKIES, cookies);
    }
    break;
    case XU_STREAM_CTRL_HTTP_GET_COOKIES:
    {
        // pcookies
        xu_cookies_ref_t* pcookies = (xu_cookies_ref_t*)xu_va_arg(args, xu_cookies_ref_t*);
        xu_assert_and_check_return_val(pcookies, xu_false);

        // get version
        return xu_http_ctrl(stream_http->http, XU_HTTP_OPTION_GET_COOKIES, pcookies);
    }
    break;
    default: break;
    }
    return xu_false;
}

/* *******************************************************
 * interfaces
 * *******************************************************
 */
xu_stream_ref_t xu_stream_init_http()
{
    // done
    xu_bool_t       ok     = xu_false;
    xu_stream_ref_t stream = xu_null;
    do
    {
        // init stream
        stream = xu_stream_init(XU_STREAM_TYPE_HTTP, sizeof(xu_stream_http_t), 0, xu_stream_http_open,
                                xu_stream_http_clos, xu_stream_http_exit, xu_stream_http_ctrl, xu_stream_http_wait,
                                xu_stream_http_read, xu_null, xu_stream_http_seek, xu_null, xu_stream_http_kill);
        xu_assert_and_check_break(stream);

        // init the http stream
        xu_stream_http_t* stream_http = xu_stream_http_cast(stream);
        xu_assert_and_check_break(stream_http);

        // init http
        stream_http->http = xu_http_init();
        xu_assert_and_check_break(stream_http->http);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) xu_stream_exit(stream);
        stream = xu_null;
    }

    // ok?
    return (xu_stream_ref_t)stream;
}
xu_stream_ref_t xu_stream_init_from_http(xu_char_t const* host, xu_uint16_t port, xu_char_t const* path, xu_bool_t bssl)
{
    // check
    xu_assert_and_check_return_val(host && port && path, xu_null);

    // done
    xu_bool_t       ok     = xu_false;
    xu_stream_ref_t stream = xu_null;
    do
    {
        // init stream
        stream = xu_stream_init_http();
        xu_assert_and_check_break(stream);

        // ctrl
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_HOST, host)) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_PORT, port)) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_PATH, path)) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_SSL, bssl)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) xu_stream_exit(stream);
        stream = xu_null;
    }

    // ok?
    return stream;
}
