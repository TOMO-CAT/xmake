#pragma once

#include "xutil/prefix.h"
#include "xutil/stream/static_stream.h"

/* convert charset encoding data using the platform api
 *
 * @param ftype     the from charset
 * @param ttype     the to charset
 * @param fst       the from stream
 * @param tst       the to stream
 *
 * @return          the converted bytes for output or -1
 */
static __xu_inline__ xu_long_t xu_charset_conv_impl(xu_size_t ftype, xu_size_t ttype, xu_static_stream_ref_t fst,
                                      xu_static_stream_ref_t tst)
{
    return -1;
}
