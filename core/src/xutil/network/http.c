/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "http"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/network/http.h"
#include "xutil/algorithm/for.h"
#include "xutil/container/hash_map.h"
#include "xutil/network/impl/http/method.h"
#include "xutil/network/impl/http/option.h"
#include "xutil/network/impl/http/status.h"
#include "xutil/stream/stream.h"
#include "xutil/stream/transfer.h"
#include "xutil/utils/url.h"

/* *******************************************************
 * types
 */
// the http type
typedef struct __xu_http_t
{
    // the option
    xu_http_option_t option;

    // the status
    xu_http_status_t status;

    // the stream
    xu_stream_ref_t stream;

    // the sstream for sock
    xu_stream_ref_t sstream;

    // the cstream for chunked
    xu_stream_ref_t cstream;

    // the zstream for gzip/deflate
    xu_stream_ref_t zstream;

    // the head
    xu_hash_map_ref_t head;

    // is opened?
    xu_bool_t bopened;

    // the request data
    xu_string_t request;

    // the cookies
    xu_string_t cookies;

    // the request/response data for decreasing stack size
    xu_char_t data[8192];

} xu_http_t;

/* *******************************************************
 * implementation
 */
static xu_bool_t xu_http_connect(xu_http_t* http)
{
    // check
    xu_assert_and_check_return_val(http && http->stream, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // the host is changed?
        xu_bool_t        host_changed = xu_true;
        xu_char_t const* host_old     = xu_null;
        xu_char_t const* host_new     = xu_url_host(&http->option.url);
        xu_stream_ctrl(http->stream, XU_STREAM_CTRL_GET_HOST, &host_old);
        if (host_old && host_new && !xu_stricmp(host_old, host_new)) host_changed = xu_false;

        // trace
        xu_trace_d("connect: host: %s", host_changed ? "changed" : "keep");

        // set url and timeout
        if (!xu_stream_ctrl(http->stream, XU_STREAM_CTRL_SET_URL, xu_url_cstr(&http->option.url))) break;
        if (!xu_stream_ctrl(http->stream, XU_STREAM_CTRL_SET_TIMEOUT, http->option.timeout)) break;

        // reset keep-alive and close socket first before connecting anthor host
        if (host_changed && !xu_stream_ctrl(http->stream, XU_STREAM_CTRL_SOCK_KEEP_ALIVE, xu_false)) break;

            // dump option
#if defined(__xu_debug__) && XU_TRACE_MODULE_DEBUG
        xu_http_option_dump(&http->option);
#endif

        // trace
        xu_trace_d("connect: ..");

        // clear status
        xu_http_status_cler(&http->status, host_changed);

        // open stream
        if (!xu_stream_open(http->stream)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed? save state
    if (!ok) http->status.state = xu_stream_state(http->stream);

    // trace
    xu_trace_d("connect: %s, state: %s", ok ? "ok" : "failed", xu_state_cstr(http->status.state));

    // ok?
    return ok;
}
static xu_bool_t xu_http_request_post(xu_size_t state, xu_hize_t offset, xu_hong_t size, xu_hize_t save, xu_size_t rate,
                                      xu_cpointer_t priv)
{
    // check
    xu_http_t* http = (xu_http_t*)priv;
    xu_assert_and_check_return_val(http && http->stream, xu_false);

    // trace
    xu_trace_d("post: percent: %llu%%, size: %lu, state: %s", size > 0 ? (offset * 100 / size) : 0, save,
               xu_state_cstr(state));

    // done func
    if (http->option.post_func && !http->option.post_func(state, offset, size, save, rate, http->option.post_priv))
        return xu_false;

    // ok?
    return xu_true;
}
static xu_bool_t xu_http_request(xu_http_t* http)
{
    // check
    xu_assert_and_check_return_val(http && http->stream, xu_false);

    // done
    xu_bool_t       ok        = xu_false;
    xu_stream_ref_t pstream   = xu_null;
    xu_hong_t       post_size = 0;
    do
    {
        // clear line data
        xu_string_clear(&http->request);

        // init the head value
        xu_static_string_t value;
        if (!xu_static_string_init(&value, http->data, sizeof(http->data))) break;

        // init method
        xu_char_t const* method = xu_http_method_cstr(http->option.method);
        xu_assert_and_check_break(method);

        // init path
        xu_char_t const* path = xu_url_path(&http->option.url);
        xu_assert_and_check_break(path);

        // init args
        xu_char_t const* args = xu_url_args(&http->option.url);

        // init host
        xu_char_t const* host = xu_url_host(&http->option.url);
        xu_assert_and_check_break(host);
        xu_hash_map_insert(http->head, "Host", host);

        // init accept
        xu_hash_map_insert(http->head, "Accept", "*/*");

        // init connection
        xu_hash_map_insert(http->head, "Connection", http->status.balived ? "keep-alive" : "close");

        // init cookies
        xu_bool_t cookie = xu_false;
        if (http->option.cookies)
        {
            // set cookie
            if (xu_cookies_get(http->option.cookies, host, path, xu_url_ssl(&http->option.url), &http->cookies))
            {
                xu_hash_map_insert(http->head, "Cookie", xu_string_cstr(&http->cookies));
                cookie = xu_true;
            }
        }

        // no cookie? remove it
        if (!cookie) xu_hash_map_remove(http->head, "Cookie");

        // init range
        if (http->option.range.bof && http->option.range.eof >= http->option.range.bof)
            xu_static_string_cstrfcpy(&value, "bytes=%llu-%llu", http->option.range.bof, http->option.range.eof);
        else if (http->option.range.bof && !http->option.range.eof)
            xu_static_string_cstrfcpy(&value, "bytes=%llu-", http->option.range.bof);
        else if (!http->option.range.bof && http->option.range.eof)
            xu_static_string_cstrfcpy(&value, "bytes=0-%llu", http->option.range.eof);
        else if (http->option.range.bof > http->option.range.eof)
        {
            http->status.state = XU_STATE_HTTP_RANGE_INVALID;
            break;
        }

        // update range
        if (xu_static_string_size(&value)) xu_hash_map_insert(http->head, "Range", xu_static_string_cstr(&value));
        // remove range
        else
            xu_hash_map_remove(http->head, "Range");

        // init post
        if (http->option.method == XU_HTTP_METHOD_POST)
        {
            // done
            xu_bool_t post_ok = xu_false;
            do
            {
                // init pstream
                xu_char_t const* url = xu_url_cstr(&http->option.post_url);
                if (http->option.post_data && http->option.post_size)
                    pstream = xu_stream_init_from_data(http->option.post_data, http->option.post_size);
                else if (url)
                    pstream = xu_stream_init_from_url(url);
                xu_assert_and_check_break(pstream);

                // open pstream
                if (!xu_stream_open(pstream)) break;

                // the post size
                post_size = xu_stream_size(pstream);
                xu_assert_and_check_break(post_size >= 0);

                // append post size
                xu_static_string_cstrfcpy(&value, "%lld", post_size);
                xu_hash_map_insert(http->head, "Content-Length", xu_static_string_cstr(&value));

                // ok
                post_ok = xu_true;

            } while (0);

            // init post failed?
            if (!post_ok)
            {
                http->status.state = XU_STATE_HTTP_POST_FAILED;
                break;
            }
        }
        // remove post
        else
            xu_hash_map_remove(http->head, "Content-Length");

        // replace the custom head
        xu_char_t const* head_data = (xu_char_t const*)xu_buffer_data(&http->option.head_data);
        xu_char_t const* head_tail = head_data + xu_buffer_size(&http->option.head_data);
        while (head_data < head_tail)
        {
            // the name and data
            xu_char_t const* name = head_data;
            xu_char_t const* data = head_data + xu_strlen(name) + 1;
            xu_check_break(data < head_tail);

            // replace it
            xu_hash_map_insert(http->head, name, data);

            // next
            head_data = data + xu_strlen(data) + 1;
        }

        // exit the head value
        xu_static_string_exit(&value);

        // check head
        xu_assert_and_check_break(xu_hash_map_size(http->head));

        // append method
        xu_string_cstrcat(&http->request, method);

        // append ' '
        xu_string_chrcat(&http->request, ' ');

        // encode path
        xu_url_encode2(path, xu_strlen(path), http->data, sizeof(http->data) - 1);
        path = http->data;

        // append path
        xu_string_cstrcat(&http->request, path);

        // append args if exists
        if (args)
        {
            // append '?'
            xu_string_chrcat(&http->request, '?');

            // encode args
            xu_url_encode2(args, xu_strlen(args), http->data, sizeof(http->data) - 1);
            args = http->data;

            // append args
            xu_string_cstrcat(&http->request, args);
        }

        // append ' '
        xu_string_chrcat(&http->request, ' ');

        // append version, HTTP/1.1
        xu_string_cstrfcat(&http->request, "HTTP/1.%1u\r\n",
                           http->status.balived ? http->status.version : http->option.version);

        // append key: value
        xu_for_all(xu_hash_map_item_ref_t, item, http->head)
        {
            if (item && item->name && item->data)
                xu_string_cstrfcat(&http->request, "%s: %s\r\n", (xu_char_t const*)item->name,
                                   (xu_char_t const*)item->data);
        }

        // append end
        xu_string_cstrcat(&http->request, "\r\n");

        // the request data and size
        xu_char_t const* request_data = xu_string_cstr(&http->request);
        xu_size_t        request_size = xu_string_size(&http->request);
        xu_assert_and_check_break(request_data && request_size);

        // trace
        xu_trace_d("request[%lu]:\n%s", request_size, request_data);

        // writ request
        if (!xu_stream_bwrit(http->stream, (xu_byte_t const*)request_data, request_size)) break;

        // writ post
        if (http->option.method == XU_HTTP_METHOD_POST)
        {
            // post stream
            if (xu_transfer(pstream, http->stream, http->option.post_lrate, xu_http_request_post, http) != post_size)
            {
                http->status.state = XU_STATE_HTTP_POST_FAILED;
                break;
            }
        }

        // sync request
        if (!xu_stream_sync(http->stream, xu_false)) break;

        // ok
        ok = xu_true;
    } while (0);

    // failed?
    if (!ok && !http->status.state) http->status.state = XU_STATE_HTTP_REQUEST_FAILED;

    // exit pstream
    if (pstream) xu_stream_exit(pstream);
    pstream = xu_null;

    // ok?
    return ok;
}
/*
 * HTTP/1.1 206 Partial Content
 * Date: Fri, 23 Apr 2010 05:25:45 GMT
 * Server: Apache/2.2.9 (Ubuntu) PHP/5.2.6-2ubuntu4.5 with Suhosin-Patch
 * Last-Modified: Mon, 08 Mar 2010 09:58:09 GMT
 * ETag: "6cc014-8f47f-481471a322e40"
 * Accept-Ranges: bytes
 * Content-Length: 586879
 * Content-Range: bytes 0-586878/586879
 * Connection: close
 * Content-Type: application/x-shockwave-flash
 */
static xu_bool_t xu_http_response_done(xu_http_t* http, xu_char_t const* line, xu_size_t indx)
{
    // check
    xu_assert_and_check_return_val(http && http->sstream && line, xu_false);

    // the first line?
    xu_char_t const* p = line;
    if (!indx)
    {
        // check http response
        if (xu_strnicmp(p, "HTTP/1.", 7))
        {
            // failed
            xu_assert(0);
            return xu_false;
        }

        // seek to the http version
        p += 7;
        xu_assert_and_check_return_val(*p, xu_false);

        // parse version
        xu_assert_and_check_return_val((*p - '0') < 2, xu_false);
        http->status.version = *p - '0';

        // seek to the http code
        p++;
        while (xu_isspace(*p))
            p++;

        // parse code
        xu_assert_and_check_return_val(*p && xu_isdigit(*p), xu_false);
        http->status.code = xu_stou32(p);

        // save state
        if (http->status.code == 200 || http->status.code == 206)
            http->status.state = XU_STATE_OK;
        else if (http->status.code == 204)
            http->status.state = XU_STATE_HTTP_RESPONSE_204;
        else if (http->status.code >= 300 && http->status.code <= 307)
            http->status.state = XU_STATE_HTTP_RESPONSE_300 + (http->status.code - 300);
        else if (http->status.code >= 400 && http->status.code <= 416)
            http->status.state = XU_STATE_HTTP_RESPONSE_400 + (http->status.code - 400);
        else if (http->status.code >= 500 && http->status.code <= 507)
            http->status.state = XU_STATE_HTTP_RESPONSE_500 + (http->status.code - 500);
        else
            http->status.state = XU_STATE_HTTP_RESPONSE_UNK;

        // check state code: 4xx & 5xx
        if (http->status.code >= 400 && http->status.code < 600) return xu_false;
    }
    // key: value?
    else
    {
        // seek to value
        while (*p && *p != ':')
            p++;
        xu_assert_and_check_return_val(*p, xu_false);
        p++;
        while (*p && xu_isspace(*p))
            p++;

        // no value
        xu_check_return_val(*p, xu_true);

        // parse content size
        if (!xu_strnicmp(line, "Content-Length", 14))
        {
            http->status.content_size = xu_stou64(p);
            if (http->status.document_size < 0) http->status.document_size = http->status.content_size;
        }
        // parse content range: "bytes $from-$to/$document_size"
        else if (!xu_strnicmp(line, "Content-Range", 13))
        {
            xu_hize_t from          = 0;
            xu_hize_t to            = 0;
            xu_hize_t document_size = 0;
            if (!xu_strncmp(p, "bytes ", 6))
            {
                p += 6;
                from = xu_stou64(p);
                while (*p && *p != '-')
                    p++;
                if (*p && *p++ == '-') to = xu_stou64(p);
                while (*p && *p != '/')
                    p++;
                if (*p && *p++ == '/') document_size = xu_stou64(p);
            }
            // no stream, be able to seek
            http->status.bseeked       = 1;
            http->status.document_size = document_size;
            if (http->status.content_size < 0)
            {
                if (from && to > from)
                    http->status.content_size = to - from;
                else if (!from && to)
                    http->status.content_size = to;
                else if (from && !to && document_size > from)
                    http->status.content_size = document_size - from;
                else
                    http->status.content_size = document_size;
            }
        }
        // parse accept-ranges: "bytes "
        else if (!xu_strnicmp(line, "Accept-Ranges", 13))
        {
            // no stream, be able to seek
            http->status.bseeked = 1;
        }
        // parse content type
        else if (!xu_strnicmp(line, "Content-Type", 12))
        {
            xu_string_cstrcpy(&http->status.content_type, p);
            xu_assert_and_check_return_val(xu_string_size(&http->status.content_type), xu_false);
        }
        // parse transfer encoding
        else if (!xu_strnicmp(line, "Transfer-Encoding", 17))
        {
            if (!xu_stricmp(p, "chunked")) http->status.bchunked = 1;
        }
        // parse content encoding
        else if (!xu_strnicmp(line, "Content-Encoding", 16))
        {
            if (!xu_stricmp(p, "gzip"))
                http->status.bgzip = 1;
            else if (!xu_stricmp(p, "deflate"))
                http->status.bdeflate = 1;
        }
        // parse location
        else if (!xu_strnicmp(line, "Location", 8))
        {
            // redirect? check code: 301 - 307
            xu_assert_and_check_return_val(http->status.code > 300 && http->status.code < 308, xu_false);

            // save location
            xu_string_cstrcpy(&http->status.location, p);
        }
        // parse connection
        else if (!xu_strnicmp(line, "Connection", 10))
        {
            // keep alive?
            http->status.balived = !xu_stricmp(p, "close") ? 0 : 1;

            // ctrl stream for sock
            if (!xu_stream_ctrl(http->sstream, XU_STREAM_CTRL_SOCK_KEEP_ALIVE,
                                http->status.balived ? xu_true : xu_false))
                return xu_false;
        }
        // parse cookies
        else if (http->option.cookies && !xu_strnicmp(line, "Set-Cookie", 10))
        {
            // the host
            xu_char_t const* host = xu_null;
            xu_http_ctrl((xu_http_ref_t)http, XU_HTTP_OPTION_GET_HOST, &host);

            // the path
            xu_char_t const* path = xu_null;
            xu_http_ctrl((xu_http_ref_t)http, XU_HTTP_OPTION_GET_PATH, &path);

            // is ssl?
            xu_bool_t bssl = xu_false;
            xu_http_ctrl((xu_http_ref_t)http, XU_HTTP_OPTION_GET_SSL, &bssl);

            // set cookies
            xu_cookies_set(http->option.cookies, host, path, bssl, p);
        }
    }

    // ok
    return xu_true;
}
static xu_bool_t xu_http_response(xu_http_t* http)
{
    // check
    xu_assert_and_check_return_val(http && http->stream, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // read line
        xu_long_t real = 0;
        xu_size_t indx = 0;
        while ((real = xu_stream_bread_line(http->stream, http->data, sizeof(http->data) - 1)) >= 0)
        {
            // trace
            xu_trace_d("response: %s", http->data);

            // do callback
            if (http->option.head_func && !http->option.head_func(http->data, http->option.head_priv)) break;

            // end?
            if (!real)
            {
                // switch to cstream if chunked
                if (http->status.bchunked)
                {
                    // init cstream
                    if (http->cstream)
                    {
                        if (!xu_stream_ctrl(http->cstream, XU_STREAM_CTRL_FLTR_SET_STREAM, http->stream)) break;
                    }
                    else
                        http->cstream = xu_stream_init_filter_from_chunked(http->stream, xu_true);
                    xu_assert_and_check_break(http->cstream);

                    // open cstream, need not async
                    if (!xu_stream_open(http->cstream)) break;

                    // using cstream
                    http->stream = http->cstream;

                    // disable seek
                    http->status.bseeked = 0;
                }

                // switch to zstream if gzip or deflate
                if (http->option.bunzip && (http->status.bgzip || http->status.bdeflate))
                {
#if defined(XU_CONFIG_PACKAGE_HAVE_ZLIB) && defined(XU_CONFIG_MODULE_HAVE_ZIP)
                    // init zstream
                    if (http->zstream)
                    {
                        if (!xu_stream_ctrl(http->zstream, XU_STREAM_CTRL_FLTR_SET_STREAM, http->stream)) break;
                    }
                    else
                        http->zstream = xu_stream_init_filter_from_zip(
                            http->stream, http->status.bgzip ? XU_ZIP_ALGO_GZIP : XU_ZIP_ALGO_ZLIB,
                            XU_ZIP_ACTION_INFLATE);
                    xu_assert_and_check_break(http->zstream);

                    // the filter
                    xu_filter_ref_t filter = xu_null;
                    if (!xu_stream_ctrl(http->zstream, XU_STREAM_CTRL_FLTR_GET_FILTER, &filter)) break;
                    xu_assert_and_check_break(filter);

                    // ctrl filter
                    if (!xu_filter_ctrl(filter, XU_FILTER_CTRL_ZIP_SET_ALGO,
                                        http->status.bgzip ? XU_ZIP_ALGO_GZIP : XU_ZIP_ALGO_ZLIB,
                                        XU_ZIP_ACTION_INFLATE))
                        break;

                    // limit the filter input size
                    if (http->status.content_size > 0) xu_filter_limit(filter, http->status.content_size);

                    // open zstream, need not async
                    if (!xu_stream_open(http->zstream)) break;

                    // using zstream
                    http->stream = http->zstream;

                    // disable seek
                    http->status.bseeked = 0;
#else
                    // trace
                    xu_trace_w("gzip is not supported now! please enable it from config if you need it.");

                    // not supported
                    http->status.state = XU_STATE_HTTP_GZIP_NOT_SUPPORTED;
                    break;
#endif
                }

                // trace
                xu_trace_d("response: ok");

                // dump status
#if defined(__xu_debug__) && XU_TRACE_MODULE_DEBUG
                xu_http_status_dump(&http->status);
#endif

                // ok
                ok = xu_true;
                break;
            }

            // done it
            if (!xu_http_response_done(http, http->data, indx++)) break;
        }

    } while (0);

    // ok?
    return ok;
}
static xu_bool_t xu_http_redirect(xu_http_t* http)
{
    // check
    xu_assert_and_check_return_val(http && http->stream, xu_false);

    // done
    xu_size_t i  = 0;
    xu_bool_t ok = xu_true;
    for (i = 0; i < http->option.redirect && xu_string_size(&http->status.location); i++)
    {
        // read the redirect content
        if (http->status.content_size > 0)
        {
            xu_byte_t data[XU_STREAM_BLOCK_MAXN];
            xu_hize_t read = 0;
            xu_hize_t size = http->status.content_size;
            while (read < size)
            {
                // the need
                xu_size_t need = (xu_size_t)xu_min(size - read, (xu_hize_t)XU_STREAM_BLOCK_MAXN);

                // read it
                if (!xu_stream_bread(http->stream, data, need)) break;

                // save size
                read += need;
            }

            // check
            xu_assert_pass_and_check_break(read == size);
        }

        // close stream
        if (http->stream && !xu_stream_clos(http->stream)) break;

        // switch to sstream
        http->stream = http->sstream;

        // get location url
        xu_char_t const* location = xu_string_cstr(&http->status.location);
        xu_assert_and_check_break(location);

        // trace
        xu_trace_d("redirect: %s", location);

        // only path?
        xu_size_t protocol = xu_url_protocol_probe(location);
        if (protocol == XU_URL_PROTOCOL_FILE) xu_url_path_set(&http->option.url, location);
        // full http url?
        else if (protocol == XU_URL_PROTOCOL_HTTP)
        {
            // set url
            if (!xu_url_cstr_set(&http->option.url, location)) break;
        }
        else
        {
            // trace
            xu_trace_e("unsupported protocol for location %s", location);
            break;
        }

        // connect it
        if (!(ok = xu_http_connect(http))) break;

        // request it
        if (!(ok = xu_http_request(http))) break;

        // response it
        if (!(ok = xu_http_response(http))) break;
    }

    // ok?
    return ok && !xu_string_size(&http->status.location);
}

/* *******************************************************
 * interfaces
 */
xu_http_ref_t xu_http_init()
{
    // done
    xu_bool_t  ok   = xu_false;
    xu_http_t* http = xu_null;
    do
    {
        // make http
        http = xu_malloc0_type(xu_http_t);
        xu_assert_and_check_break(http);

        // init stream
        http->stream = http->sstream = xu_stream_init_sock();
        xu_assert_and_check_break(http->stream);

        // init head
        http->head = xu_hash_map_init(8, xu_element_str(xu_false), xu_element_str(xu_false));
        xu_assert_and_check_break(http->head);

        // init request data
        if (!xu_string_init(&http->request)) break;

        // init cookies data
        if (!xu_string_init(&http->cookies)) break;

        // init option
        if (!xu_http_option_init(&http->option)) break;

        // init status
        if (!xu_http_status_init(&http->status)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (http) xu_http_exit((xu_http_ref_t)http);
        http = xu_null;
    }

    // ok?
    return (xu_http_ref_t)http;
}
xu_void_t xu_http_kill(xu_http_ref_t self)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return(http);

    // kill stream
    if (http->stream) xu_stream_kill(http->stream);
}
xu_void_t xu_http_exit(xu_http_ref_t self)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return(http);

    // close it
    xu_http_close(self);

    // exit zstream
    if (http->zstream) xu_stream_exit(http->zstream);
    http->zstream = xu_null;

    // exit cstream
    if (http->cstream) xu_stream_exit(http->cstream);
    http->cstream = xu_null;

    // exit sstream
    if (http->sstream) xu_stream_exit(http->sstream);
    http->sstream = xu_null;

    // exit stream
    http->stream = xu_null;

    // exit status
    xu_http_status_exit(&http->status);

    // exit option
    xu_http_option_exit(&http->option);

    // exit cookies data
    xu_string_exit(&http->cookies);

    // exit request data
    xu_string_exit(&http->request);

    // exit head
    if (http->head) xu_hash_map_exit(http->head);
    http->head = xu_null;

    // free it
    xu_free(http);
}
xu_long_t xu_http_wait(xu_http_ref_t self, xu_size_t events, xu_long_t timeout)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return_val(http && http->stream, -1);

    // opened?
    xu_assert_and_check_return_val(http->bopened, -1);

    // wait it
    xu_long_t wait = xu_stream_wait(http->stream, events, timeout);

    // failed? save state
    if (wait < 0 && !http->status.state) http->status.state = xu_stream_state(http->stream);

    // ok?
    return wait;
}
xu_bool_t xu_http_open(xu_http_ref_t self)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return_val(http, xu_false);

    // opened?
    xu_assert_and_check_return_val(!http->bopened, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // connect it
        if (!xu_http_connect(http)) break;

        // request it
        if (!xu_http_request(http)) break;

        // response it
        if (!xu_http_response(http)) break;

        // redirect it
        if (!xu_http_redirect(http)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed? close it
    if (!ok)
    {
        // close stream
        if (http->stream) xu_stream_clos(http->stream);

        // switch to sstream
        http->stream = http->sstream;
    }

    // is opened?
    http->bopened = ok;

    // ok?
    return ok;
}
xu_bool_t xu_http_close(xu_http_ref_t self)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return_val(http, xu_false);

    // opened?
    xu_check_return_val(http->bopened, xu_true);

    // close stream
    if (http->stream && !xu_stream_clos(http->stream)) return xu_false;

    // switch to sstream
    http->stream = http->sstream;

    // clear opened
    http->bopened = xu_false;

    // ok
    return xu_true;
}
xu_bool_t xu_http_seek(xu_http_ref_t self, xu_hize_t offset)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return_val(http, xu_false);

    // opened?
    xu_assert_and_check_return_val(http->bopened, xu_false);

    // seeked?
    xu_check_return_val(http->status.bseeked, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // close stream
        if (http->stream && !xu_stream_clos(http->stream)) break;

        // switch to sstream
        http->stream = http->sstream;

        // trace
        xu_trace_d("seek: %llu", offset);

        // set range
        http->option.range.bof = offset;
        http->option.range.eof = http->status.document_size > 0 ? http->status.document_size - 1 : 0;

        // connect it
        if (!xu_http_connect(http)) break;

        // request it
        if (!xu_http_request(http)) break;

        // response it
        if (!xu_http_response(http)) break;

        // ok
        ok = xu_true;

    } while (0);

    // ok?
    return ok;
}
xu_long_t xu_http_read(xu_http_ref_t self, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return_val(http && http->stream, -1);

    // opened?
    xu_assert_and_check_return_val(http->bopened, -1);

    // read
    return xu_stream_read(http->stream, data, size);
}
xu_bool_t xu_http_bread(xu_http_ref_t self, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return_val(http && http->stream, xu_false);

    // opened?
    xu_assert_and_check_return_val(http->bopened, xu_false);

    // read
    xu_size_t read = 0;
    while (read < size)
    {
        // read data
        xu_long_t real = xu_stream_read(http->stream, data + read, size - read);

        // update size
        if (real > 0) read += real;
        // no data?
        else if (!real)
        {
            // wait
            xu_long_t e = xu_http_wait(self, XU_SOCKET_EVENT_RECV, http->option.timeout);
            xu_assert_and_check_break(e >= 0);

            // timeout?
            xu_check_break(e);

            // has read?
            xu_assert_and_check_break(e & XU_SOCKET_EVENT_RECV);
        }
        else
            break;
    }

    // ok?
    return read == size ? xu_true : xu_false;
}
xu_bool_t xu_http_ctrl(xu_http_ref_t self, xu_size_t option, ...)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return_val(http && option, xu_false);

    // check
    if (XU_HTTP_OPTION_CODE_IS_SET(option) && http->bopened)
    {
        // abort
        xu_assert(0);
        return xu_false;
    }

    // init args
    xu_va_list_t args;
    xu_va_start(args, option);

    // done
    xu_bool_t ok = xu_http_option_ctrl(&http->option, option, args);

    // exit args
    xu_va_end(args);

    // ok?
    return ok;
}
xu_http_status_t const* xu_http_status(xu_http_ref_t self)
{
    // check
    xu_http_t* http = (xu_http_t*)self;
    xu_assert_and_check_return_val(http, xu_null);

    // the status
    return &http->status;
}
