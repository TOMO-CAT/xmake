#pragma once

#include "xutil/network/http.h"
#include "xutil/prefix.h"

/* get the http method string
 *
 * @param method        the method
 *
 * @return              the method string
 */
xu_char_t const* xu_http_method_cstr(xu_size_t method);
