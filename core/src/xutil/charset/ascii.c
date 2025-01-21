#include "xutil/charset/chartset.h"

xu_long_t xu_charset_ascii_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_ascii_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch)
{
    *ch = xu_static_stream_read_u8(sstream);
    return 1;
}

xu_long_t xu_charset_ascii_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);
xu_long_t xu_charset_ascii_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch)
{
    if (ch <= 0xff) xu_static_stream_writ_u8(sstream, (xu_uint8_t)ch);
    return 1;
}
