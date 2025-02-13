#pragma once

#include "xutil/prefix.h"

/* get the http date from the given cstring
 *
 * <pre>
 * supports format:
 *    Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
 *    Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
 *    Sun Nov 6 08:49:37 1994        ; ANSI C's asctime() format
 *
 * for cookies(RFC 822, RFC 850, RFC 1036, and RFC 1123):
 *    Sun, 06-Nov-1994 08:49:37 GMT
 *
 * </pre>
 *
 * @param cstr          the cstring
 * @param size          the cstring length
 *
 * @return              the date
 */
xu_time_t xu_http_date_from_cstr(xu_char_t const* cstr, xu_size_t size);
