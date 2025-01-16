#include "xutil/utils/state.h"

xu_char_t const* xu_state_cstr(xu_size_t state)
{
    // done
    switch (state)
    {
    case XU_STATE_OK: return "ok";
    case XU_STATE_END: return "end";
    case XU_STATE_DEAD: return "dead";
    case XU_STATE_READY: return "ready";
    case XU_STATE_FAILED: return "failed";
    case XU_STATE_KILLED: return "killed";
    case XU_STATE_PAUSED: return "paused";
    case XU_STATE_CLOSED: return "closed";
    case XU_STATE_OPENED: return "opened";
    case XU_STATE_OPENING: return "opening";
    case XU_STATE_SUSPEND: return "suspend";
    case XU_STATE_RUNNING: return "running";
    case XU_STATE_PENDING: return "pending";
    case XU_STATE_WAITING: return "waiting";
    case XU_STATE_WORKING: return "working";
    case XU_STATE_KILLING: return "killing";
    case XU_STATE_EXITING: return "exiting";
    case XU_STATE_PAUSING: return "pausing";
    case XU_STATE_TIMEOUT: return "timeout";
    case XU_STATE_FINISHED: return "finished";
    case XU_STATE_WAIT_FAILED: return "wait failed";
    case XU_STATE_POST_FAILED: return "post failed";
    case XU_STATE_NOT_SUPPORTED: return "not supported";
    case XU_STATE_UNKNOWN_ERROR: return "unknown error";

    case XU_STATE_SOCK_DNS_FAILED: return "sock: dns: failed";
    case XU_STATE_SOCK_CONNECT_FAILED: return "sock: connect: failed";
    case XU_STATE_SOCK_CONNECT_TIMEOUT: return "sock: connect: timeout";
    case XU_STATE_SOCK_RECV_TIMEOUT: return "sock: recv: timeout";
    case XU_STATE_SOCK_SEND_TIMEOUT: return "sock: send: timeout";
    case XU_STATE_SOCK_RECV_FAILED: return "sock: recv: failed";
    case XU_STATE_SOCK_SEND_FAILED: return "sock: send: failed";
    case XU_STATE_SOCK_OPEN_FAILED: return "sock: open: failed";
    case XU_STATE_SOCK_UNKNOWN_ERROR: return "sock: unknown error";
    case XU_STATE_SOCK_SSL_FAILED: return "sock: ssl: failed";
    case XU_STATE_SOCK_SSL_TIMEOUT: return "sock: ssl: timeout";
    case XU_STATE_SOCK_SSL_WANT_READ: return "sock: ssl: want read";
    case XU_STATE_SOCK_SSL_WANT_WRIT: return "sock: ssl: want writ";
    case XU_STATE_SOCK_SSL_WAIT_FAILED: return "sock: ssl: wait failed";
    case XU_STATE_SOCK_SSL_READ_FAILED: return "sock: ssl: read: failed";
    case XU_STATE_SOCK_SSL_WRIT_FAILED: return "sock: ssl: writ: failed";
    case XU_STATE_SOCK_SSL_NOT_SUPPORTED: return "sock: ssl: not supported";
    case XU_STATE_SOCK_SSL_UNKNOWN_ERROR: return "sock: ssl: unknown error";

    case XU_STATE_HTTP_RESPONSE_100: return "http: response: 100";
    case XU_STATE_HTTP_RESPONSE_101: return "http: response: 101";
    case XU_STATE_HTTP_RESPONSE_102: return "http: response: 102";
    case XU_STATE_HTTP_RESPONSE_200: return "http: response: 200";
    case XU_STATE_HTTP_RESPONSE_204: return "http: response: 204";
    case XU_STATE_HTTP_RESPONSE_300: return "http: response: 300";
    case XU_STATE_HTTP_RESPONSE_301: return "http: response: 301";
    case XU_STATE_HTTP_RESPONSE_302: return "http: response: 302";
    case XU_STATE_HTTP_RESPONSE_303: return "http: response: 303";
    case XU_STATE_HTTP_RESPONSE_304: return "http: response: 304";
    case XU_STATE_HTTP_RESPONSE_305: return "http: response: 305";
    case XU_STATE_HTTP_RESPONSE_306: return "http: response: 306";
    case XU_STATE_HTTP_RESPONSE_307: return "http: response: 307";
    case XU_STATE_HTTP_RESPONSE_400: return "http: response: 400";
    case XU_STATE_HTTP_RESPONSE_401: return "http: response: 401";
    case XU_STATE_HTTP_RESPONSE_402: return "http: response: 402";
    case XU_STATE_HTTP_RESPONSE_403: return "http: response: 403";
    case XU_STATE_HTTP_RESPONSE_404: return "http: response: 404";
    case XU_STATE_HTTP_RESPONSE_405: return "http: response: 405";
    case XU_STATE_HTTP_RESPONSE_406: return "http: response: 406";
    case XU_STATE_HTTP_RESPONSE_407: return "http: response: 407";
    case XU_STATE_HTTP_RESPONSE_408: return "http: response: 408";
    case XU_STATE_HTTP_RESPONSE_409: return "http: response: 409";
    case XU_STATE_HTTP_RESPONSE_410: return "http: response: 410";
    case XU_STATE_HTTP_RESPONSE_411: return "http: response: 411";
    case XU_STATE_HTTP_RESPONSE_412: return "http: response: 412";
    case XU_STATE_HTTP_RESPONSE_413: return "http: response: 413";
    case XU_STATE_HTTP_RESPONSE_414: return "http: response: 414";
    case XU_STATE_HTTP_RESPONSE_415: return "http: response: 415";
    case XU_STATE_HTTP_RESPONSE_416: return "http: response: 416";
    case XU_STATE_HTTP_RESPONSE_500: return "http: response: 500";
    case XU_STATE_HTTP_RESPONSE_501: return "http: response: 501";
    case XU_STATE_HTTP_RESPONSE_502: return "http: response: 502";
    case XU_STATE_HTTP_RESPONSE_503: return "http: response: 503";
    case XU_STATE_HTTP_RESPONSE_504: return "http: response: 504";
    case XU_STATE_HTTP_RESPONSE_505: return "http: response: 505";
    case XU_STATE_HTTP_RESPONSE_506: return "http: response: 506";
    case XU_STATE_HTTP_RESPONSE_507: return "http: response: 507";
    case XU_STATE_HTTP_RESPONSE_UNK: return "http: response: unknown code";
    case XU_STATE_HTTP_RESPONSE_NUL: return "http: response: no";
    case XU_STATE_HTTP_REQUEST_FAILED: return "http: request: failed";
    case XU_STATE_HTTP_REDIRECT_FAILED: return "http: redirect: failed";
    case XU_STATE_HTTP_POST_FAILED: return "http: post: failed";
    case XU_STATE_HTTP_RANGE_INVALID: return "http: range: invalid";
    case XU_STATE_HTTP_GZIP_NOT_SUPPORTED: return "http: gzip: not supported";
    case XU_STATE_HTTP_UNKNOWN_ERROR: return "http: unknown error";

    case XU_STATE_DATABASE_NO_SUCH_TABLE: return "database: no such table";
    case XU_STATE_DATABASE_NO_SUCH_FIELD: return "database: no such field";
    case XU_STATE_DATABASE_NO_SUCH_DATABASE: return "database: no such database";
    case XU_STATE_DATABASE_ACCESS_DENIED: return "database: access denied";
    case XU_STATE_DATABASE_PARSE_ERROR: return "database: parse error";
    case XU_STATE_DATABASE_VALUE_COUNT_ERROR: return "database: value count error";
    case XU_STATE_DATABASE_UNKNOWN_HOST: return "database: unknown host";
    case XU_STATE_DATABASE_UNKNOWN_ERROR: return "database: unknown error";

    case XU_STATE_SYSERROR_NOT_PERM: return "syserror: permission denied";
    case XU_STATE_SYSERROR_NOT_FILEDIR: return "syserror: not file or directory";
    case XU_STATE_SYSERROR_NOT_ACCESS: return "syserror: not access";
    case XU_STATE_SYSERROR_UNKNOWN_ERROR: return "syserror: unknown error";

    default: return "unknown";
    }

    return xu_null;
}
