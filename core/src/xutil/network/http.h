#pragma once

#include "xutil/network/cookies.h"
#include "xutil/network/url.h"
#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

// the http option code: get
#define XU_HTTP_OPTION_CODE_GET(x) ((x) + 1)

// the http option code: set
#define XU_HTTP_OPTION_CODE_SET(x) (0xff00 | ((x) + 1))

// the http option code is setter?
#define XU_HTTP_OPTION_CODE_IS_SET(x) ((x)&0xff00)

/* *******************************************************
 * types
 */

// the http method enum
typedef enum __xu_http_method_e
{
    XU_HTTP_METHOD_GET     = 0,
    XU_HTTP_METHOD_POST    = 1,
    XU_HTTP_METHOD_HEAD    = 2,
    XU_HTTP_METHOD_PUT     = 3,
    XU_HTTP_METHOD_OPTIONS = 4,
    XU_HTTP_METHOD_DELETE  = 5,
    XU_HTTP_METHOD_TRACE   = 6,
    XU_HTTP_METHOD_CONNECT = 7

} xu_http_method_e;

// the http code enum
typedef enum __xu_http_code_e
{
    XU_HTTP_CODE_CONTINUE            = 100,
    XU_HTTP_CODE_SWITCHING_PROTOCOLS = 101,
    XU_HTTP_CODE_PROCESSING          = 102

    ,
    XU_HTTP_CODE_OK                            = 200,
    XU_HTTP_CODE_CREATED                       = 201,
    XU_HTTP_CODE_ACCEPTED                      = 202,
    XU_HTTP_CODE_NON_AUTHORITATIVE_INFORMATION = 203,
    XU_HTTP_CODE_NO_CONTENT                    = 204,
    XU_HTTP_CODE_RESET_CONTENT                 = 205,
    XU_HTTP_CODE_PARTIAL_CONTENT               = 206,
    XU_HTTP_CODE_MULTI_STATUS                  = 207

    ,
    XU_HTTP_CODE_MULTIPLE_CHOICES   = 300,
    XU_HTTP_CODE_MOVED_PERMANENTLY  = 301,
    XU_HTTP_CODE_MOVED_TEMPORARILY  = 302,
    XU_HTTP_CODE_SEE_OTHER          = 303,
    XU_HTTP_CODE_NOT_MODIFIED       = 304,
    XU_HTTP_CODE_USE_PROXY          = 305,
    XU_HTTP_CODE_SWITCH_PROXY       = 306,
    XU_HTTP_CODE_TEMPORARY_REDIRECT = 307

    ,
    XU_HTTP_CODE_BAD_REQUEST             = 400,
    XU_HTTP_CODE_UNAUTHORIZED            = 401,
    XU_HTTP_CODE_FORBIDDEN               = 403,
    XU_HTTP_CODE_NOT_FOUND               = 404,
    XU_HTTP_CODE_METHOD_NOT_ALLOWED      = 405,
    XU_HTTP_CODE_NOT_ACCEPTABLE          = 406,
    XU_HTTP_CODE_REQUEST_TIMEOUT         = 408,
    XU_HTTP_CODE_CONFLICT                = 409,
    XU_HTTP_CODE_GONE                    = 410,
    XU_HTTP_CODE_LENGTH_REQUIRED         = 411,
    XU_HTTP_CODE_PRECONDITION_FAILED     = 412,
    XU_HTTP_CODE_REQUEST_ENTITY_TOO_LONG = 413,
    XU_HTTP_CODE_REQUEST_URI_TOO_LONG    = 414,
    XU_HTTP_CODE_UNSUPPORTED_MEDIA_TYPE  = 415,
    XU_HTTP_CODE_RANGE_NOT_SATISFIABLE   = 416,
    XU_HTTP_CODE_EXPECTATION_FAILED      = 417,
    XU_HTTP_CODE_UNPROCESSABLE_ENTITY    = 422,
    XU_HTTP_CODE_LOCKED                  = 423,
    XU_HTTP_CODE_FAILED_DEPENDENCY       = 424,
    XU_HTTP_CODE_UNORDERED_COLLECTION    = 425,
    XU_HTTP_CODE_UPGRADE_REQUIRED        = 426,
    XU_HTTP_CODE_RETRY_WITH              = 449

    ,
    XU_HTTP_CODE_INTERNAL_SERVER_ERROR = 500,
    XU_HTTP_CODE_NOT_IMPLEMENTED       = 501,
    XU_HTTP_CODE_BAD_GATEWAY           = 502,
    XU_HTTP_CODE_SERVICE_UNAVAILABLE   = 503,
    XU_HTTP_CODE_GATEWAY_TIMEOUT       = 504,
    XU_HTTP_CODE_INSUFFICIENT_STORAGE  = 507,
    XU_HTTP_CODE_LOOP_DETECTED         = 508,
    XU_HTTP_CODE_NOT_EXTENDED          = 510

} xu_http_code_e;

// the http option enum
typedef enum __xu_http_option_e
{
    XU_HTTP_OPTION_NONE = 0

    ,
    XU_HTTP_OPTION_GET_SSL        = XU_HTTP_OPTION_CODE_GET(1),
    XU_HTTP_OPTION_GET_URL        = XU_HTTP_OPTION_CODE_GET(2),
    XU_HTTP_OPTION_GET_HOST       = XU_HTTP_OPTION_CODE_GET(3),
    XU_HTTP_OPTION_GET_PORT       = XU_HTTP_OPTION_CODE_GET(4),
    XU_HTTP_OPTION_GET_PATH       = XU_HTTP_OPTION_CODE_GET(5),
    XU_HTTP_OPTION_GET_HEAD       = XU_HTTP_OPTION_CODE_GET(6),
    XU_HTTP_OPTION_GET_RANGE      = XU_HTTP_OPTION_CODE_GET(7),
    XU_HTTP_OPTION_GET_METHOD     = XU_HTTP_OPTION_CODE_GET(8),
    XU_HTTP_OPTION_GET_VERSION    = XU_HTTP_OPTION_CODE_GET(9),
    XU_HTTP_OPTION_GET_TIMEOUT    = XU_HTTP_OPTION_CODE_GET(10),
    XU_HTTP_OPTION_GET_COOKIES    = XU_HTTP_OPTION_CODE_GET(11),
    XU_HTTP_OPTION_GET_REDIRECT   = XU_HTTP_OPTION_CODE_GET(12),
    XU_HTTP_OPTION_GET_HEAD_FUNC  = XU_HTTP_OPTION_CODE_GET(13),
    XU_HTTP_OPTION_GET_HEAD_PRIV  = XU_HTTP_OPTION_CODE_GET(14),
    XU_HTTP_OPTION_GET_AUTO_UNZIP = XU_HTTP_OPTION_CODE_GET(15),
    XU_HTTP_OPTION_GET_POST_URL   = XU_HTTP_OPTION_CODE_GET(16),
    XU_HTTP_OPTION_GET_POST_DATA  = XU_HTTP_OPTION_CODE_GET(17),
    XU_HTTP_OPTION_GET_POST_FUNC  = XU_HTTP_OPTION_CODE_GET(18),
    XU_HTTP_OPTION_GET_POST_PRIV  = XU_HTTP_OPTION_CODE_GET(19),
    XU_HTTP_OPTION_GET_POST_LRATE = XU_HTTP_OPTION_CODE_GET(20)

        ,
    XU_HTTP_OPTION_SET_SSL        = XU_HTTP_OPTION_CODE_SET(1),
    XU_HTTP_OPTION_SET_URL        = XU_HTTP_OPTION_CODE_SET(2),
    XU_HTTP_OPTION_SET_HOST       = XU_HTTP_OPTION_CODE_SET(3),
    XU_HTTP_OPTION_SET_PORT       = XU_HTTP_OPTION_CODE_SET(4),
    XU_HTTP_OPTION_SET_PATH       = XU_HTTP_OPTION_CODE_SET(5),
    XU_HTTP_OPTION_SET_HEAD       = XU_HTTP_OPTION_CODE_SET(6),
    XU_HTTP_OPTION_SET_RANGE      = XU_HTTP_OPTION_CODE_SET(7),
    XU_HTTP_OPTION_SET_METHOD     = XU_HTTP_OPTION_CODE_SET(8),
    XU_HTTP_OPTION_SET_VERSION    = XU_HTTP_OPTION_CODE_SET(9),
    XU_HTTP_OPTION_SET_TIMEOUT    = XU_HTTP_OPTION_CODE_SET(10),
    XU_HTTP_OPTION_SET_COOKIES    = XU_HTTP_OPTION_CODE_SET(11),
    XU_HTTP_OPTION_SET_REDIRECT   = XU_HTTP_OPTION_CODE_SET(12),
    XU_HTTP_OPTION_SET_HEAD_FUNC  = XU_HTTP_OPTION_CODE_SET(13),
    XU_HTTP_OPTION_SET_HEAD_PRIV  = XU_HTTP_OPTION_CODE_SET(14),
    XU_HTTP_OPTION_SET_AUTO_UNZIP = XU_HTTP_OPTION_CODE_SET(15),
    XU_HTTP_OPTION_SET_POST_URL   = XU_HTTP_OPTION_CODE_SET(16),
    XU_HTTP_OPTION_SET_POST_DATA  = XU_HTTP_OPTION_CODE_SET(17),
    XU_HTTP_OPTION_SET_POST_FUNC  = XU_HTTP_OPTION_CODE_SET(18),
    XU_HTTP_OPTION_SET_POST_PRIV  = XU_HTTP_OPTION_CODE_SET(19),
    XU_HTTP_OPTION_SET_POST_LRATE = XU_HTTP_OPTION_CODE_SET(20)

} xu_http_option_e;

// the http range type
typedef struct __xu_http_range_t
{
    // the begin offset
    xu_hize_t bof;

    // the end offset
    xu_hize_t eof;

} xu_http_range_t;

// the http ref type
typedef __xu_typeref__(http);

/*! the http head func type
 *
 * @param line          the http head line
 * @param priv          the func private data
 *
 * @return              xu_true: ok and continue it if need, xu_false: break it
 */
typedef xu_bool_t (*xu_http_head_func_t)(xu_char_t const* line, xu_cpointer_t priv);

/*! the http post func type
 *
 * @param offset        the istream offset
 * @param size          the istream size, no size: -1
 * @param save          the saved size
 * @param rate          the current rate, bytes/s
 * @param priv          the func private data
 *
 * @return              xu_true: ok and continue it if need, xu_false: break it
 */
typedef xu_bool_t (*xu_http_post_func_t)(xu_size_t state, xu_hize_t offset, xu_hong_t size, xu_hize_t save,
                                         xu_size_t rate, xu_cpointer_t priv);

// the http option type
typedef struct __xu_http_option_t
{
    // the method
    xu_uint16_t method : 4;

    // auto unzip for gzip encoding?
    xu_uint16_t bunzip : 1;

    // the http version, 0: HTTP/1.0, 1: HTTP/1.1
    xu_uint16_t version : 1;

    // the redirect maxn
    xu_uint16_t redirect : 10;

    // the url
    xu_url_t url;

    // timeout: ms
    xu_long_t timeout;

    // range
    xu_http_range_t range;

    // the cookies
    xu_cookies_ref_t cookies;

    // the priv data
    xu_pointer_t head_priv;

    // the head func
    xu_http_head_func_t head_func;

    // the head data
    xu_buffer_t head_data;

    // the post url
    xu_url_t post_url;

    // the post data
    xu_byte_t const* post_data;

    // the post size
    xu_size_t post_size;

    // the post func
    xu_http_post_func_t post_func;

    // the post data
    xu_cpointer_t post_priv;

    // the post limit rate
    xu_size_t post_lrate;

} xu_http_option_t;

// the http status type
typedef struct __xu_http_status_t
{
    // the http code
    xu_uint16_t code : 10;

    // the http version
    xu_uint16_t version : 1;

    // keep alive?
    xu_uint16_t balived : 1;

    // be able to seek?
    xu_uint16_t bseeked : 1;

    // is chunked?
    xu_uint16_t bchunked : 1;

    // is gzip?
    xu_uint16_t bgzip : 1;

    // is deflate?
    xu_uint16_t bdeflate : 1;

    // the state
    xu_size_t state;

    // the document size
    xu_hong_t document_size;

    // the current content size, maybe in range
    xu_hong_t content_size;

    // the content type
    xu_string_t content_type;

    // the location
    xu_string_t location;

} xu_http_status_t;

/* //////////////////////////////////////////////////////////
 * interfaces
 */

/*! init http
 *
 * return               the http
 */
xu_http_ref_t xu_http_init(xu_noarg_t);

/*! exit http
 *
 * @param http          the http
 */
xu_void_t xu_http_exit(xu_http_ref_t http);

/*! kill http
 *
 * @param http          the http
 */
xu_void_t xu_http_kill(xu_http_ref_t http);

/*! wait the http
 *
 * blocking wait the single event object, so need not aiop
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param http          the http
 * @param events          the events
 * @param timeout       the timeout value, return immediately if 0, infinity if -1
 *
 * @return              the event type, return 0 if timeout, return -1 if error
 */
xu_long_t xu_http_wait(xu_http_ref_t http, xu_size_t events, xu_long_t timeout);

/*! open the http
 *
 * @param http          the http
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_http_open(xu_http_ref_t http);

/*! close http
 *
 * @param http          the http
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_http_close(xu_http_ref_t http);

/*! seek http
 *
 * @param http          the http
 * @param offset        the offset
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_http_seek(xu_http_ref_t http, xu_hize_t offset);

/*! read data, non-blocking
 *
 * @param http          the http
 * @param data          the data
 * @param size          the size
 *
 * @return              ok: real size, continue: 0, fail: -1
 */
xu_long_t xu_http_read(xu_http_ref_t http, xu_byte_t* data, xu_size_t size);

/*! read data, blocking
 *
 * @param http          the http
 * @param data          the data
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_http_bread(xu_http_ref_t http, xu_byte_t* data, xu_size_t size);

/*! ctrl the http option
 *
 * @param http          the http
 * @param option        the ctrl option
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_http_ctrl(xu_http_ref_t http, xu_size_t option, ...);

/*! the http status
 *
 * @param http          the http
 *
 * @return              the http status
 */
xu_http_status_t const* xu_http_status(xu_http_ref_t http);
