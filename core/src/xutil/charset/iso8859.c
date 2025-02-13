#include "xutil/charset/charset.h"

xu_long_t xu_charset_iso8859_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_iso8859_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch)
{
    xu_byte_t b = xu_static_stream_read_u8(sstream);
    if (b < 0xa0)
        *ch = b;
    else
    {
        // @note: need lookup characters table
        xu_trace_d("iso8859: unknown character: %x", b);
        *ch = 0;
    }
    return 1;
}

xu_long_t xu_charset_iso8859_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);
xu_long_t xu_charset_iso8859_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch)
{
    if (ch <= 0xa0)
        xu_static_stream_writ_u8(sstream, (xu_uint8_t)ch);
    else
    {
        // @note: need lookup characters table
        xu_trace_d("iso8859: unknown character: %x", ch);
    }
    return 1;
}
