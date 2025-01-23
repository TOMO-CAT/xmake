#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

#ifdef XU_WORDS_BIGENDIAN
#    define xu_static_stream_read_u16_ne(stream) xu_static_stream_read_u16_be(stream)
#    define xu_static_stream_read_s16_ne(stream) xu_static_stream_read_s16_be(stream)
#    define xu_static_stream_read_u24_ne(stream) xu_static_stream_read_u24_be(stream)
#    define xu_static_stream_read_s24_ne(stream) xu_static_stream_read_s24_be(stream)
#    define xu_static_stream_read_u32_ne(stream) xu_static_stream_read_u32_be(stream)
#    define xu_static_stream_read_s32_ne(stream) xu_static_stream_read_s32_be(stream)
#    define xu_static_stream_read_u64_ne(stream) xu_static_stream_read_u64_be(stream)
#    define xu_static_stream_read_s64_ne(stream) xu_static_stream_read_s64_be(stream)

#    define xu_static_stream_writ_u16_ne(stream, val) xu_static_stream_writ_u16_be(stream, val)
#    define xu_static_stream_writ_s16_ne(stream, val) xu_static_stream_writ_s16_be(stream, val)
#    define xu_static_stream_writ_u24_ne(stream, val) xu_static_stream_writ_u24_be(stream, val)
#    define xu_static_stream_writ_s24_ne(stream, val) xu_static_stream_writ_s24_be(stream, val)
#    define xu_static_stream_writ_u32_ne(stream, val) xu_static_stream_writ_u32_be(stream, val)
#    define xu_static_stream_writ_s32_ne(stream, val) xu_static_stream_writ_s32_be(stream, val)
#    define xu_static_stream_writ_u64_ne(stream, val) xu_static_stream_writ_u64_be(stream, val)
#    define xu_static_stream_writ_s64_ne(stream, val) xu_static_stream_writ_s64_be(stream, val)

#else
#    define xu_static_stream_read_u16_ne(stream) xu_static_stream_read_u16_le(stream)
#    define xu_static_stream_read_s16_ne(stream) xu_static_stream_read_s16_le(stream)
#    define xu_static_stream_read_u24_ne(stream) xu_static_stream_read_u24_le(stream)
#    define xu_static_stream_read_s24_ne(stream) xu_static_stream_read_s24_le(stream)
#    define xu_static_stream_read_u32_ne(stream) xu_static_stream_read_u32_le(stream)
#    define xu_static_stream_read_s32_ne(stream) xu_static_stream_read_s32_le(stream)
#    define xu_static_stream_read_u64_ne(stream) xu_static_stream_read_u64_le(stream)
#    define xu_static_stream_read_s64_ne(stream) xu_static_stream_read_s64_le(stream)

#    define xu_static_stream_writ_u16_ne(stream, val) xu_static_stream_writ_u16_le(stream, val)
#    define xu_static_stream_writ_s16_ne(stream, val) xu_static_stream_writ_s16_le(stream, val)
#    define xu_static_stream_writ_u24_ne(stream, val) xu_static_stream_writ_u24_le(stream, val)
#    define xu_static_stream_writ_s24_ne(stream, val) xu_static_stream_writ_s24_le(stream, val)
#    define xu_static_stream_writ_u32_ne(stream, val) xu_static_stream_writ_u32_le(stream, val)
#    define xu_static_stream_writ_s32_ne(stream, val) xu_static_stream_writ_s32_le(stream, val)
#    define xu_static_stream_writ_u64_ne(stream, val) xu_static_stream_writ_u64_le(stream, val)
#    define xu_static_stream_writ_s64_ne(stream, val) xu_static_stream_writ_s64_le(stream, val)

#endif

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#    ifdef XU_FLOAT_BIGENDIAN
#        define xu_static_stream_read_double_nbe(stream) xu_static_stream_read_double_bbe(stream)
#        define xu_static_stream_read_double_nle(stream) xu_static_stream_read_double_ble(stream)

#        define xu_static_stream_writ_double_nbe(stream, val) xu_static_stream_writ_double_bbe(stream, val)
#        define xu_static_stream_writ_double_nle(stream, val) xu_static_stream_writ_double_ble(stream, val)
#    else
#        define xu_static_stream_read_double_nbe(stream) xu_static_stream_read_double_lbe(stream)
#        define xu_static_stream_read_double_nle(stream) xu_static_stream_read_double_lle(stream)

#        define xu_static_stream_writ_double_nbe(stream, val) xu_static_stream_writ_double_lbe(stream, val)
#        define xu_static_stream_writ_double_nle(stream, val) xu_static_stream_writ_double_lle(stream, val)
#    endif
#    ifdef XU_WORDS_BIGENDIAN
#        define xu_static_stream_read_float_ne(stream) xu_static_stream_read_float_be(stream)
#        define xu_static_stream_writ_float_ne(stream, val) xu_static_stream_writ_float_be(stream, val)

#        define xu_static_stream_read_double_nne(stream) xu_static_stream_read_double_nbe(stream)
#        define xu_static_stream_read_double_bne(stream) xu_static_stream_read_double_bbe(stream)
#        define xu_static_stream_read_double_lne(stream) xu_static_stream_read_double_lbe(stream)

#        define xu_static_stream_writ_double_nne(stream, val) xu_static_stream_writ_double_nbe(stream, val)
#        define xu_static_stream_writ_double_bne(stream, val) xu_static_stream_writ_double_bbe(stream, val)
#        define xu_static_stream_writ_double_lne(stream, val) xu_static_stream_writ_double_lbe(stream, val)
#    else
#        define xu_static_stream_read_float_ne(stream) xu_static_stream_read_float_le(stream)
#        define xu_static_stream_writ_float_ne(stream, val) xu_static_stream_writ_float_le(stream, val)

#        define xu_static_stream_read_double_nne(stream) xu_static_stream_read_double_nle(stream)
#        define xu_static_stream_read_double_bne(stream) xu_static_stream_read_double_ble(stream)
#        define xu_static_stream_read_double_lne(stream) xu_static_stream_read_double_lle(stream)

#        define xu_static_stream_writ_double_nne(stream, val) xu_static_stream_writ_double_nle(stream, val)
#        define xu_static_stream_writ_double_bne(stream, val) xu_static_stream_writ_double_ble(stream, val)
#        define xu_static_stream_writ_double_lne(stream, val) xu_static_stream_writ_double_lle(stream, val)
#    endif
#endif

/* *******************************************************
 * types
 */
// the static stream type
typedef struct __xu_static_stream_t
{
    /// the pointer to the current position
    xu_byte_t* p;

    /// the bit offset < 8
    xu_size_t b;

    /// the pointer to the end
    xu_byte_t* e;

    /// the data size
    xu_size_t n;

} xu_static_stream_t;

// the static stream ref type
typedef xu_static_stream_t* xu_static_stream_ref_t;

/* *******************************************************
 * interfaces
 */

/*! init the static stream
 *
 * @param stream    the stream
 * @param data      the data address
 * @param size      the data size
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_init(xu_static_stream_ref_t stream, xu_byte_t* data, xu_size_t size);

/*! goto the new data address for updating the stream position
 *
 * @param stream    the stream
 * @param data      the data address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_goto(xu_static_stream_ref_t stream, xu_byte_t* data);

/*! sync the stream position if update some bits offset
 *
 * @param stream    the stream
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_sync(xu_static_stream_ref_t stream);

/*! the stream start data address
 *
 * @param stream    the stream
 *
 * @return          the start data address
 */
xu_byte_t const* xu_static_stream_beg(xu_static_stream_ref_t stream);

/*! the stream current data address
 *
 * @param stream    the stream
 *
 * @return          the current data address
 */
xu_byte_t const* xu_static_stream_pos(xu_static_stream_ref_t stream);

/*! the stream end data address
 *
 * @param stream    the stream
 *
 * @return          the end data address
 */
xu_byte_t const* xu_static_stream_end(xu_static_stream_ref_t stream);

/*! the stream offset
 *
 * @param stream    the stream
 *
 * @return          the offset
 */
xu_size_t xu_static_stream_offset(xu_static_stream_ref_t stream);

/*! the stream data size
 *
 * @param stream    the stream
 *
 * @return          the data size
 */
xu_size_t xu_static_stream_size(xu_static_stream_ref_t stream);

/*! the stream left size
 *
 * @param stream    the stream
 *
 * @return          the left size
 */
xu_size_t xu_static_stream_left(xu_static_stream_ref_t stream);

/*! the stream left bits
 *
 * @param stream    the stream
 *
 * @return          the left bits
 */
xu_size_t xu_static_stream_left_bits(xu_static_stream_ref_t stream);

/*! the stream is valid?
 *
 * @param stream    the stream
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_valid(xu_static_stream_ref_t stream);

/*! skip the given size
 *
 * @param stream    the stream
 * @param size      the skiped size
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_skip(xu_static_stream_ref_t stream, xu_size_t size);

/*! skip the given bits
 *
 * @param stream    the stream
 * @param nbits     the skiped bits count
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_skip_bits(xu_static_stream_ref_t stream, xu_size_t nbits);

/*! skip the given c-string
 *
 * @param stream    the stream
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_skip_cstr(xu_static_stream_ref_t stream);

/*! peek ubits value for uint32
 *
 * @param stream    the stream
 * @param nbits     the bits count
 *
 * @return          the ubits value
 */
xu_uint32_t xu_static_stream_peek_ubits32(xu_static_stream_ref_t stream, xu_size_t nbits);

/*! peek sbits value for sint32
 *
 * @param stream    the stream
 * @param nbits     the bits count
 *
 * @return          the sbits value
 */
xu_sint32_t xu_static_stream_peek_sbits32(xu_static_stream_ref_t stream, xu_size_t nbits);

/*! read c-string
 *
 * @param stream    the stream
 *
 * @return          the c-string
 */
xu_char_t const* xu_static_stream_read_cstr(xu_static_stream_ref_t stream);

/*! read data
 *
 * @param stream    the stream
 * @param data      the data
 * @param size      the size
 *
 * @return          the data real size
 */
xu_size_t xu_static_stream_read_data(xu_static_stream_ref_t stream, xu_byte_t* data, xu_size_t size);

/*! read ubits value for uint32
 *
 * @param stream    the stream
 * @param nbits     the bits count
 *
 * @return          the ubits value
 */
xu_uint32_t xu_static_stream_read_ubits32(xu_static_stream_ref_t stream, xu_size_t nbits);

/*! read sbits value for sint32
 *
 * @param stream    the stream
 * @param nbits     the bits count
 *
 * @return          the sbits value
 */
xu_sint32_t xu_static_stream_read_sbits32(xu_static_stream_ref_t stream, xu_size_t nbits);

/*! read ubits value for uint64
 *
 * @param stream    the stream
 * @param nbits     the bits count
 *
 * @return          the ubits value
 */
xu_uint64_t xu_static_stream_read_ubits64(xu_static_stream_ref_t stream, xu_size_t nbits);

/*! read sbits value for sint64
 *
 * @param stream    the stream
 * @param nbits     the bits count
 *
 * @return          the sbits value
 */
xu_sint64_t xu_static_stream_read_sbits64(xu_static_stream_ref_t stream, xu_size_t nbits);

/*! read ubits1 value
 *
 * @param stream    the stream
 *
 * @return          the ubits1 value
 */
xu_uint8_t xu_static_stream_read_u1(xu_static_stream_ref_t stream);

/*! read uint8 integer
 *
 * @param stream    the stream
 *
 * @return          the uint8 integer
 */
xu_uint8_t xu_static_stream_read_u8(xu_static_stream_ref_t stream);

/*! read sint8 integer
 *
 * @param stream    the stream
 *
 * @return          the sint8 integer
 */
xu_sint8_t xu_static_stream_read_s8(xu_static_stream_ref_t stream);

/*! read uint16-be integer
 *
 * @param stream    the stream
 *
 * @return          the uint16-be integer
 */
xu_uint16_t xu_static_stream_read_u16_be(xu_static_stream_ref_t stream);

/*! read sint16-be integer
 *
 * @param stream    the stream
 *
 * @return          the sint16-be integer
 */
xu_sint16_t xu_static_stream_read_s16_be(xu_static_stream_ref_t stream);

/*! read uint16-le integer
 *
 * @param stream    the stream
 *
 * @return          the uint16-le integer
 */
xu_uint16_t xu_static_stream_read_u16_le(xu_static_stream_ref_t stream);

/*! read sint16-le integer
 *
 * @param stream    the stream
 *
 * @return          the sint16-le integer
 */
xu_sint16_t xu_static_stream_read_s16_le(xu_static_stream_ref_t stream);

/*! read uint24-be integer
 *
 * @param stream    the stream
 *
 * @return          the uint24-be integer
 */
xu_uint32_t xu_static_stream_read_u24_be(xu_static_stream_ref_t stream);

/*! read sint24-be integer
 *
 * @param stream    the stream
 *
 * @return          the sint24-be integer
 */
xu_sint32_t xu_static_stream_read_s24_be(xu_static_stream_ref_t stream);

/*! read uint24-le integer
 *
 * @param stream    the stream
 *
 * @return          the uint24-le integer
 */
xu_uint32_t xu_static_stream_read_u24_le(xu_static_stream_ref_t stream);

/*! read sint24-le integer
 *
 * @param stream    the stream
 *
 * @return          the sint24-le integer
 */
xu_sint32_t xu_static_stream_read_s24_le(xu_static_stream_ref_t stream);

/*! read uint32-be integer
 *
 * @param stream    the stream
 *
 * @return          the uint32-be integer
 */
xu_uint32_t xu_static_stream_read_u32_be(xu_static_stream_ref_t stream);

/*! read sint32-be integer
 *
 * @param stream    the stream
 *
 * @return          the sint32-be integer
 */
xu_sint32_t xu_static_stream_read_s32_be(xu_static_stream_ref_t stream);

/*! read uint32-le integer
 *
 * @param stream    the stream
 *
 * @return          the uint32-le integer
 */
xu_uint32_t xu_static_stream_read_u32_le(xu_static_stream_ref_t stream);

/*! read sint32-le integer
 *
 * @param stream    the stream
 *
 * @return          the sint32-le integer
 */
xu_sint32_t xu_static_stream_read_s32_le(xu_static_stream_ref_t stream);

/*! read uint64-be integer
 *
 * @param stream    the stream
 *
 * @return          the uint64-be integer
 */
xu_uint64_t xu_static_stream_read_u64_be(xu_static_stream_ref_t stream);

/*! read sint64-be integer
 *
 * @param stream    the stream
 *
 * @return          the sint64-be integer
 */
xu_sint64_t xu_static_stream_read_s64_be(xu_static_stream_ref_t stream);

/*! read uint64-le integer
 *
 * @param stream    the stream
 *
 * @return          the uint64-le integer
 */
xu_uint64_t xu_static_stream_read_u64_le(xu_static_stream_ref_t stream);

/*! read sint64-le integer
 *
 * @param stream    the stream
 *
 * @return          the sint64-le integer
 */
xu_sint64_t xu_static_stream_read_s64_le(xu_static_stream_ref_t stream);

/*! writ c-string
 *
 * @param stream    the stream
 * @param cstr      the c-string
 *
 * @return          the writted c-string address
 */
xu_char_t* xu_static_stream_writ_cstr(xu_static_stream_ref_t stream, xu_char_t const* cstr);

/*! writ data
 *
 * @param stream    the stream
 * @param data      the data
 * @param size      the size
 *
 * @return          the writted data size
 */
xu_size_t xu_static_stream_writ_data(xu_static_stream_ref_t stream, xu_byte_t const* data, xu_size_t size);

/*! writ ubits for uint32
 *
 * @param stream    the stream
 * @param val       the value
 * @param nbits     the bits count
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_ubits32(xu_static_stream_ref_t stream, xu_uint32_t val, xu_size_t nbits);

/*! writ sbits for sint32
 *
 * @param stream    the stream
 * @param val       the value
 * @param nbits     the bits count
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_sbits32(xu_static_stream_ref_t stream, xu_sint32_t val, xu_size_t nbits);

/*! writ ubits1 value
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u1(xu_static_stream_ref_t stream, xu_uint8_t val);

/*! writ uint8 integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u8(xu_static_stream_ref_t stream, xu_uint8_t val);

/*! writ sint8 integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s8(xu_static_stream_ref_t stream, xu_sint8_t val);

/*! writ uint16-be integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u16_be(xu_static_stream_ref_t stream, xu_uint16_t val);

/*! writ sint16-be integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s16_be(xu_static_stream_ref_t stream, xu_sint16_t val);

/*! writ uint16-le integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u16_le(xu_static_stream_ref_t stream, xu_uint16_t val);

/*! writ sint16-le integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s16_le(xu_static_stream_ref_t stream, xu_sint16_t val);

/*! writ uint24-be integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u24_be(xu_static_stream_ref_t stream, xu_uint32_t val);

/*! writ sint24-be integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s24_be(xu_static_stream_ref_t stream, xu_sint32_t val);

/*! writ uint24-le integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u24_le(xu_static_stream_ref_t stream, xu_uint32_t val);

/*! writ sint24-le integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s24_le(xu_static_stream_ref_t stream, xu_sint32_t val);

/*! writ uint32-be integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u32_be(xu_static_stream_ref_t stream, xu_uint32_t val);

/*! writ sint32-be integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s32_be(xu_static_stream_ref_t stream, xu_sint32_t val);

/*! writ uint32-le integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u32_le(xu_static_stream_ref_t stream, xu_uint32_t val);

/*! writ sint32-le integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s32_le(xu_static_stream_ref_t stream, xu_sint32_t val);

/*! writ uint64-be integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u64_be(xu_static_stream_ref_t stream, xu_uint64_t val);

/*! writ sint64-be integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s64_be(xu_static_stream_ref_t stream, xu_sint64_t val);

/*! writ uint64-le integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_u64_le(xu_static_stream_ref_t stream, xu_uint64_t val);

/*! writ sint64-le integer
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_s64_le(xu_static_stream_ref_t stream, xu_sint64_t val);

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT

/*! read float-le number
 *
 * @param stream    the stream
 *
 * @return          the float-le number
 */
xu_float_t xu_static_stream_read_float_le(xu_static_stream_ref_t stream);

/*! read float-be number
 *
 * @param stream    the stream
 *
 * @return          the float-be number
 */
xu_float_t xu_static_stream_read_float_be(xu_static_stream_ref_t stream);

/*! read double-ble number
 *
 * @param stream    the stream
 *
 * @return          the double-ble number
 */
xu_double_t xu_static_stream_read_double_ble(xu_static_stream_ref_t stream);

/*! read double-bbe number
 *
 * @param stream    the stream
 *
 * @return          the double-bbe number
 */
xu_double_t xu_static_stream_read_double_bbe(xu_static_stream_ref_t stream);

/*! read double-lle number
 *
 * @param stream    the stream
 *
 * @return          the double-lle number
 */
xu_double_t xu_static_stream_read_double_lle(xu_static_stream_ref_t stream);

/*! read double-lbe number
 *
 * @param stream    the stream
 *
 * @return          the double-lbe number
 */
xu_double_t xu_static_stream_read_double_lbe(xu_static_stream_ref_t stream);

/*! writ float-le number
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_float_le(xu_static_stream_ref_t stream, xu_float_t val);

/*! writ float-be number
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_float_be(xu_static_stream_ref_t stream, xu_float_t val);

/*! writ double-ble number
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_double_ble(xu_static_stream_ref_t stream, xu_double_t val);

/*! writ double-bbe number
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_double_bbe(xu_static_stream_ref_t stream, xu_double_t val);

/*! writ double-lle number
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_double_lle(xu_static_stream_ref_t stream, xu_double_t val);

/*! writ double-lbe number
 *
 * @param stream    the stream
 * @param val       the value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_static_stream_writ_double_lbe(xu_static_stream_ref_t stream, xu_double_t val);

#endif
