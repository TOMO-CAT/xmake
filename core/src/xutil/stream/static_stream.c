#include "xutil/stream/static_stream.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/utils/bits.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_bool_t xu_static_stream_init(xu_static_stream_ref_t stream, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(stream && data, xu_false);

    // init
    stream->p = data;
    stream->b = 0;
    stream->n = size;
    stream->e = data + size;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_goto(xu_static_stream_ref_t stream, xu_byte_t* data)
{
    // check
    xu_assert_and_check_return_val(stream && data <= stream->e, xu_false);

    // goto
    stream->b = 0;
    if (data <= stream->e) stream->p = data;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_sync(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream, xu_false);

    // sync
    if (stream->b)
    {
        // check
        xu_assert_and_check_return_val(stream->p + 1 <= stream->e, xu_false);

        // p++
        stream->p++;
        stream->b = 0;
    }

    // ok
    return xu_true;
}
xu_byte_t const* xu_static_stream_beg(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream, xu_null);

    // the head
    return stream->e ? (stream->e - stream->n) : xu_null;
}
xu_byte_t const* xu_static_stream_pos(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p <= stream->e, xu_null);

    // sync
    xu_static_stream_sync(stream);

    // the position
    return stream->p;
}
xu_byte_t const* xu_static_stream_end(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream, xu_null);

    // the end
    return stream->e;
}
xu_size_t xu_static_stream_size(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream, 0);

    // the size
    return stream->n;
}
xu_size_t xu_static_stream_offset(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream, 0);

    // sync
    xu_static_stream_sync(stream);

    // the offset
    return (((stream->p + stream->n) > stream->e) ? (stream->p + stream->n - stream->e) : 0);
}
xu_size_t xu_static_stream_left(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p <= stream->e, 0);

    // sync
    xu_static_stream_sync(stream);

    // the left
    return (stream->e - stream->p);
}
xu_size_t xu_static_stream_left_bits(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream, 0);

    // the left bits
    return ((stream->p < stream->e) ? (((stream->e - stream->p) << 3) - stream->b) : 0);
}
xu_bool_t xu_static_stream_valid(xu_static_stream_ref_t stream)
{
    // null?
    if (!stream) return xu_false;

    // out of range?
    if (stream->p && stream->p > stream->e) return xu_false;
    if (stream->p == stream->e && stream->b) return xu_false;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_skip(xu_static_stream_ref_t stream, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(stream, xu_false);

    // sync it first
    if (!xu_static_stream_sync(stream)) return xu_false;

    // check
    xu_assert_and_check_return_val(stream->p + size <= stream->e, xu_false);

    // skip
    stream->p += size;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_skip_bits(xu_static_stream_ref_t stream, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p <= stream->e, xu_false);

    // the new position
    xu_byte_t* p = stream->p + ((stream->b + nbits) >> 3);
    xu_size_t  b = (stream->b + nbits) & 0x07;
    xu_assert_and_check_return_val(p <= stream->e && (p < stream->e || !b), xu_false);

    // skip it
    stream->p = p;
    stream->b = b;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_skip_cstr(xu_static_stream_ref_t stream)
{
    // read it
    xu_static_stream_read_cstr(stream);

    // ok?
    return xu_static_stream_valid(stream);
}
xu_uint32_t xu_static_stream_peek_ubits32(xu_static_stream_ref_t stream, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream, 0);

    // no nbits?
    xu_check_return_val(nbits, 0);

    // save
    xu_byte_t* p = stream->p;
    xu_size_t  b = stream->b;

    // peek value
    xu_uint32_t val = xu_static_stream_read_ubits32(stream, nbits);

    // restore
    stream->p = p;
    stream->b = b;

    // ok?
    return val;
}
xu_sint32_t xu_static_stream_peek_sbits32(xu_static_stream_ref_t stream, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream, 0);

    // no nbits?
    xu_check_return_val(nbits, 0);

    // save
    xu_byte_t* p = stream->p;
    xu_size_t  b = stream->b;

    // peek value
    xu_sint32_t val = xu_static_stream_read_sbits32(stream, nbits);

    // restore
    stream->p = p;
    stream->b = b;

    // ok?
    return val;
}
xu_uint32_t xu_static_stream_read_ubits32(xu_static_stream_ref_t stream, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && nbits, 0);

    // read value
    xu_uint32_t val = xu_bits_get_ubits32(stream->p, stream->b, nbits);

    // skip bits
    if (!xu_static_stream_skip_bits(stream, nbits)) return 0;

    // ok?
    return val;
}
xu_sint32_t xu_static_stream_read_sbits32(xu_static_stream_ref_t stream, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && nbits, 0);

    // read value
    xu_sint32_t val = xu_bits_get_sbits32(stream->p, stream->b, nbits);

    // skip bits
    if (!xu_static_stream_skip_bits(stream, nbits)) return 0;

    // ok?
    return val;
}
xu_uint64_t xu_static_stream_read_ubits64(xu_static_stream_ref_t stream, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && nbits, 0);

    // read value
    xu_uint64_t val = xu_bits_get_ubits64(stream->p, stream->b, nbits);

    // skip bits
    if (!xu_static_stream_skip_bits(stream, nbits)) return 0;

    // ok?
    return val;
}
xu_sint64_t xu_static_stream_read_sbits64(xu_static_stream_ref_t stream, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && nbits, 0);

    // read value
    xu_sint64_t val = xu_bits_get_sbits64(stream->p, stream->b, nbits);

    // skip bits
    if (!xu_static_stream_skip_bits(stream, nbits)) return 0;

    // ok?
    return val;
}
xu_char_t const* xu_static_stream_read_cstr(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p <= stream->e, xu_null);

    // sync it first
    if (!xu_static_stream_sync(stream)) return xu_null;

    // the string data
    xu_char_t const* data = (xu_char_t const*)stream->p;

    // the string size
    xu_size_t size = xu_strnlen(data, stream->e - stream->p);

    // skip bytes
    if (!xu_static_stream_skip(stream, size + 1)) return xu_null;

    // ok
    return data;
}
xu_size_t xu_static_stream_read_data(xu_static_stream_ref_t stream, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p <= stream->e && data, 0);

    // no size?
    xu_check_return_val(size, 0);

    // sync it first
    if (!xu_static_stream_sync(stream)) return 0;

    // the need size
    xu_size_t need = size;
    if (stream->e - stream->p < need) need = stream->e - stream->p;
    if (need)
    {
        // copy it
        xu_memcpy(data, stream->p, need);

        // skip it
        if (!xu_static_stream_skip(stream, need)) return 0;
    }

    // ok?
    return need;
}
xu_uint8_t xu_static_stream_read_u1(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e, 0);

    // the value
    xu_uint8_t val = ((*stream->p) >> (7 - stream->b)) & 1;

    // update position
    stream->b++;
    if (stream->b >= 8)
    {
        // check
        xu_assert_and_check_return_val(stream->p <= stream->e, 0);

        // update
        stream->p++;
        stream->b = 0;
    }

    // ok?
    return val;
}
xu_uint8_t xu_static_stream_read_u8(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && !stream->b, 0);

    // read it
    return *(stream->p++);
}
xu_sint8_t xu_static_stream_read_s8(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && !stream->b, 0);

    // read it
    return *(stream->p++);
}
xu_uint16_t xu_static_stream_read_u16_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 1 < stream->e && !stream->b, 0);

    // read it
    xu_uint16_t val = xu_bits_get_u16_be(stream->p);
    stream->p += 2;

    // ok?
    return val;
}
xu_sint16_t xu_static_stream_read_s16_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 1 < stream->e && !stream->b, 0);

    // read it
    xu_sint16_t val = xu_bits_get_s16_be(stream->p);
    stream->p += 2;

    // ok?
    return val;
}
xu_uint16_t xu_static_stream_read_u16_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 1 < stream->e && !stream->b, 0);

    // read it
    xu_uint16_t val = xu_bits_get_u16_le(stream->p);
    stream->p += 2;

    // ok?
    return val;
}
xu_sint16_t xu_static_stream_read_s16_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 1 < stream->e && !stream->b, 0);

    // read it
    xu_sint16_t val = xu_bits_get_s16_le(stream->p);
    stream->p += 2;

    // ok?
    return val;
}
xu_uint32_t xu_static_stream_read_u24_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 2 < stream->e && !stream->b, 0);

    // read it
    xu_uint32_t val = xu_bits_get_u24_be(stream->p);
    stream->p += 3;

    // ok?
    return val;
}
xu_sint32_t xu_static_stream_read_s24_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 2 < stream->e && !stream->b, 0);

    // read it
    xu_sint32_t val = xu_bits_get_s24_be(stream->p);
    stream->p += 3;

    // ok?
    return val;
}
xu_uint32_t xu_static_stream_read_u24_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 2 < stream->e && !stream->b, 0);

    // read it
    xu_uint32_t val = xu_bits_get_u24_le(stream->p);
    stream->p += 3;

    // ok?
    return val;
}
xu_sint32_t xu_static_stream_read_s24_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 2 < stream->e && !stream->b, 0);

    // read it
    xu_sint32_t val = xu_bits_get_s24_le(stream->p);
    stream->p += 3;

    // ok?
    return val;
}
xu_uint32_t xu_static_stream_read_u32_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, 0);

    // read it
    xu_uint32_t val = xu_bits_get_u32_be(stream->p);
    stream->p += 4;

    // ok?
    return val;
}
xu_sint32_t xu_static_stream_read_s32_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, 0);

    // read it
    xu_sint32_t val = xu_bits_get_s32_be(stream->p);
    stream->p += 4;

    // ok?
    return val;
}
xu_uint32_t xu_static_stream_read_u32_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, 0);

    // read it
    xu_uint32_t val = xu_bits_get_u32_le(stream->p);
    stream->p += 4;

    // ok?
    return val;
}
xu_sint32_t xu_static_stream_read_s32_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, 0);

    // read it
    xu_sint32_t val = xu_bits_get_s32_le(stream->p);
    stream->p += 4;

    // ok?
    return val;
}
xu_uint64_t xu_static_stream_read_u64_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, 0);

    // read it
    xu_uint64_t val = xu_bits_get_u64_be(stream->p);
    stream->p += 8;

    // ok?
    return val;
}
xu_sint64_t xu_static_stream_read_s64_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, 0);

    // read it
    xu_sint64_t val = xu_bits_get_s64_be(stream->p);
    stream->p += 8;

    // ok?
    return val;
}
xu_uint64_t xu_static_stream_read_u64_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, 0);

    // read it
    xu_uint64_t val = xu_bits_get_u64_le(stream->p);
    stream->p += 8;

    // ok?
    return val;
}
xu_sint64_t xu_static_stream_read_s64_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, 0);

    // read it
    xu_sint64_t val = xu_bits_get_s64_le(stream->p);
    stream->p += 8;

    // ok?
    return val;
}
xu_bool_t xu_static_stream_writ_ubits32(xu_static_stream_ref_t stream, xu_uint32_t val, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && nbits, 0);

    // writ bits
    xu_bits_set_ubits32(stream->p, stream->b, val, nbits);

    // skip bits
    return xu_static_stream_skip_bits(stream, nbits);
}
xu_bool_t xu_static_stream_writ_sbits32(xu_static_stream_ref_t stream, xu_sint32_t val, xu_size_t nbits)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && nbits, 0);

    // writ bits
    xu_bits_set_sbits32(stream->p, stream->b, val, nbits);

    // skip bits
    return xu_static_stream_skip_bits(stream, nbits);
}
xu_size_t xu_static_stream_writ_data(xu_static_stream_ref_t stream, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p <= stream->e && data, 0);

    // no size?
    xu_check_return_val(size, 0);

    // sync it first
    if (!xu_static_stream_sync(stream)) return 0;

    // the need size
    xu_size_t need = size;
    if (stream->e - stream->p < need) need = stream->e - stream->p;
    if (need)
    {
        // copy it
        xu_memcpy(stream->p, data, need);

        // skip it
        if (!xu_static_stream_skip(stream, need)) return 0;
    }

    // ok?
    return need;
}
xu_char_t* xu_static_stream_writ_cstr(xu_static_stream_ref_t stream, xu_char_t const* cstr)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p <= stream->e && cstr, xu_null);

    // sync it first
    if (!xu_static_stream_sync(stream)) return xu_null;

    // writ string
    xu_char_t*       b = (xu_char_t*)stream->p;
    xu_char_t*       p = (xu_char_t*)stream->p;
    xu_char_t const* e = (xu_char_t const*)stream->e - 1;
    while (*cstr && p < e)
        *p++ = *cstr++;
    *p++ = '\0';

    // check
    xu_assert_and_check_return_val(!*cstr, xu_null);

    // update position
    stream->p = (xu_byte_t*)p;

    // ok
    return b;
}
xu_bool_t xu_static_stream_writ_u1(xu_static_stream_ref_t stream, xu_uint8_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e, xu_false);

    // writ bits
    *(stream->p) &= ~(0x1 << (7 - stream->b));
    *(stream->p) |= ((val & 0x1) << (7 - stream->b));

    // update position
    stream->b++;
    if (stream->b >= 8)
    {
        // check
        xu_assert_and_check_return_val(stream->p <= stream->e, 0);

        // update
        stream->p++;
        stream->b = 0;
    }

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u8(xu_static_stream_ref_t stream, xu_uint8_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && !stream->b, xu_false);

    // writ it
    *(stream->p++) = val;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s8(xu_static_stream_ref_t stream, xu_sint8_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p < stream->e && !stream->b, xu_false);

    // writ it
    *(stream->p++) = val;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u16_be(xu_static_stream_ref_t stream, xu_uint16_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 1 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_u16_be(stream->p, val);
    stream->p += 2;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s16_be(xu_static_stream_ref_t stream, xu_sint16_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 1 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_s16_be(stream->p, val);
    stream->p += 2;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u16_le(xu_static_stream_ref_t stream, xu_uint16_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 1 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_u16_le(stream->p, val);
    stream->p += 2;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s16_le(xu_static_stream_ref_t stream, xu_sint16_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 1 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_s16_le(stream->p, val);
    stream->p += 2;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u24_be(xu_static_stream_ref_t stream, xu_uint32_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 2 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_u24_be(stream->p, val);
    stream->p += 3;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s24_be(xu_static_stream_ref_t stream, xu_sint32_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 2 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_s24_be(stream->p, val);
    stream->p += 3;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u24_le(xu_static_stream_ref_t stream, xu_uint32_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 2 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_u24_le(stream->p, val);
    stream->p += 3;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s24_le(xu_static_stream_ref_t stream, xu_sint32_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 2 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_s24_le(stream->p, val);
    stream->p += 3;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u32_be(xu_static_stream_ref_t stream, xu_uint32_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_u32_be(stream->p, val);
    stream->p += 4;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s32_be(xu_static_stream_ref_t stream, xu_sint32_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_s32_be(stream->p, val);
    stream->p += 4;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u32_le(xu_static_stream_ref_t stream, xu_uint32_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_u32_le(stream->p, val);
    stream->p += 4;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s32_le(xu_static_stream_ref_t stream, xu_sint32_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_s32_le(stream->p, val);
    stream->p += 4;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u64_be(xu_static_stream_ref_t stream, xu_uint64_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_u64_be(stream->p, val);
    stream->p += 8;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s64_be(xu_static_stream_ref_t stream, xu_sint64_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_s64_be(stream->p, val);
    stream->p += 8;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_u64_le(xu_static_stream_ref_t stream, xu_uint64_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_u64_le(stream->p, val);
    stream->p += 8;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_s64_le(xu_static_stream_ref_t stream, xu_sint64_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_s64_le(stream->p, val);
    stream->p += 8;

    // ok
    return xu_true;
}

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
xu_float_t xu_static_stream_read_float_le(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, 0);

    // read it
    xu_float_t val = xu_bits_get_float_le(stream->p);
    stream->p += 4;

    // ok?
    return val;
}
xu_float_t xu_static_stream_read_float_be(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, 0);

    // read it
    xu_float_t val = xu_bits_get_float_be(stream->p);
    stream->p += 4;

    // ok?
    return val;
}
xu_double_t xu_static_stream_read_double_ble(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, 0);

    // read it
    xu_double_t val = xu_bits_get_double_ble(stream->p);
    stream->p += 8;

    // ok?
    return val;
}
xu_double_t xu_static_stream_read_double_bbe(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, 0);

    // read it
    xu_double_t val = xu_bits_get_double_bbe(stream->p);
    stream->p += 8;

    // ok?
    return val;
}
xu_double_t xu_static_stream_read_double_lle(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, 0);

    // read it
    xu_double_t val = xu_bits_get_double_lle(stream->p);
    stream->p += 8;

    // ok?
    return val;
}
xu_double_t xu_static_stream_read_double_lbe(xu_static_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, 0);

    // read it
    xu_double_t val = xu_bits_get_double_lbe(stream->p);
    stream->p += 8;

    // ok?
    return val;
}
xu_bool_t xu_static_stream_writ_float_le(xu_static_stream_ref_t stream, xu_float_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_float_le(stream->p, val);
    stream->p += 4;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_float_be(xu_static_stream_ref_t stream, xu_float_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 3 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_float_be(stream->p, val);
    stream->p += 4;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_double_ble(xu_static_stream_ref_t stream, xu_double_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_double_ble(stream->p, val);
    stream->p += 8;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_double_bbe(xu_static_stream_ref_t stream, xu_double_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_double_bbe(stream->p, val);
    stream->p += 8;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_double_lle(xu_static_stream_ref_t stream, xu_double_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_double_lle(stream->p, val);
    stream->p += 8;

    // ok
    return xu_true;
}
xu_bool_t xu_static_stream_writ_double_lbe(xu_static_stream_ref_t stream, xu_double_t val)
{
    // check
    xu_assert_and_check_return_val(stream && stream->p && stream->p + 7 < stream->e && !stream->b, xu_false);

    // writ it
    xu_bits_set_double_lbe(stream->p, val);
    stream->p += 8;

    // ok
    return xu_true;
}

#endif
