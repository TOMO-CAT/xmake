#include "xutil/charset/charset.h"

xu_long_t xu_charset_utf16_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_utf16_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch)
{
    // init
    xu_byte_t const* p = xu_static_stream_pos(sstream);
    xu_byte_t const* q = p;
    xu_size_t        n = xu_static_stream_left(sstream);

    // not enough? break it
    xu_check_return_val(n > 1, -1);

    // the first character
    xu_uint32_t c = be ? xu_bits_get_u16_be(p) : xu_bits_get_u16_le(p);
    p += 2;

    // large?
    if (c >= 0xd800 && c <= 0xdbff)
    {
        // not enough? break it
        xu_check_return_val(n > 3, -1);

        // the next character
        xu_uint32_t c2 = be ? xu_bits_get_u16_be(p) : xu_bits_get_u16_le(p);
        if (c2 >= 0xdc00 && c2 <= 0xdfff)
        {
            c = ((c - 0xd800) << 10) + (c2 - 0xdc00) + 0x0010000;
            p += 2;
        }
    };

    // next
    if (p > q) xu_static_stream_skip(sstream, p - q);

    // set character
    *ch = c;

    // ok?
    return p > q ? 1 : 0;
}

xu_long_t xu_charset_utf16_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);
xu_long_t xu_charset_utf16_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch)
{
    // init
    xu_size_t n = xu_static_stream_left(sstream);

    if (ch <= 0x0000ffff)
    {
        // not enough? break it
        xu_check_return_val(n > 1, -1);

        // set character
        if (be)
            xu_static_stream_writ_u16_be(sstream, ch);
        else
            xu_static_stream_writ_u16_le(sstream, ch);
    }
    else if (ch > 0x0010ffff)
    {
        // not enough? break it
        xu_check_return_val(n > 1, -1);

        // set character
        if (be)
            xu_static_stream_writ_u16_be(sstream, 0x0000fffd);
        else
            xu_static_stream_writ_u16_le(sstream, 0x0000fffd);
    }
    else
    {
        // not enough? break it
        xu_check_return_val(n > 3, -1);

        // set character
        ch -= 0x0010000;
        if (be)
        {
            xu_static_stream_writ_u16_be(sstream, (ch >> 10) + 0xd800);
            xu_static_stream_writ_u16_be(sstream, (ch & 0x3ff) + 0xdc00);
        }
        else
        {
            xu_static_stream_writ_u16_le(sstream, (ch >> 10) + 0xd800);
            xu_static_stream_writ_u16_le(sstream, (ch & 0x3ff) + 0xdc00);
        }
    }

    // ok
    return 1;
}
