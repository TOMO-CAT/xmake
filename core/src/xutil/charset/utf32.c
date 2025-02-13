#include "xutil/charset/charset.h"

xu_long_t xu_charset_utf32_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_utf32_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch)
{
    *ch = be ? xu_static_stream_read_u32_be(sstream) : xu_static_stream_read_u32_le(sstream);
    return 1;
}

xu_long_t xu_charset_utf32_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);
xu_long_t xu_charset_utf32_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch)
{
    if (be)
        xu_static_stream_writ_u32_be(sstream, ch);
    else
        xu_static_stream_writ_u32_le(sstream, ch);
    return 1;
}
