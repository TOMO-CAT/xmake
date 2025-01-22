/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "http_method"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/network/impl/http/method.h"

/* *******************************************************
 * globals
 */
static xu_char_t const* g_http_methods[] = {"GET", "POST", "HEAD", "PUT", "OPTIONS", "DELETE", "TRACE", "CONNECT"};

/* *******************************************************
 * implementation
 */
xu_char_t const* xu_http_method_cstr(xu_size_t method)
{
    // check
    xu_assert_and_check_return_val(method < xu_arrayn(g_http_methods), xu_null);

    // ok
    return g_http_methods[method];
}
