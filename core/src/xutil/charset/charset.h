#pragma once

#include "xutil/prefix.h"
#include "xutil/stream/static_stream.h"
#include "xutil/utils/bits.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 * macros
 */

// the endian for the charset type
#define XU_CHARSET_TYPE_LE (0x0100)
#define XU_CHARSET_TYPE_BE (0x0000)
#define XU_CHARSET_TYPE_ME (0x0100)
#ifdef XU_WORDS_BIGENDIAN
#    define XU_CHARSET_TYPE_NE (XU_CHARSET_TYPE_BE)
#else
#    define XU_CHARSET_TYPE_NE (XU_CHARSET_TYPE_LE)
#endif

// type
#define XU_CHARSET_TYPE(type) (((type) & ~XU_CHARSET_TYPE_ME))

// ok?
#define XU_CHARSET_TYPE_OK(type) (XU_CHARSET_TYPE(type) != XU_CHARSET_TYPE_NONE)

/* *******************************************************
 * types
 */

/*! the charset type enum
 *
 * @note default: big endian
 */
typedef enum __xu_charset_type_e
{
    XU_CHARSET_TYPE_NONE    = 0x00,
    XU_CHARSET_TYPE_ASCII   = 0x01,
    XU_CHARSET_TYPE_GB2312  = 0x02,
    XU_CHARSET_TYPE_GBK     = 0x03,
    XU_CHARSET_TYPE_ISO8859 = 0x04,
    XU_CHARSET_TYPE_UCS2    = 0x05,
    XU_CHARSET_TYPE_UCS4    = 0x06,
    XU_CHARSET_TYPE_UTF16   = 0x07,
    XU_CHARSET_TYPE_UTF32   = 0x08,
    XU_CHARSET_TYPE_UTF8    = 0x09,
    XU_CHARSET_TYPE_ANSI    = 0x10

} xu_charset_type_e;

// the charset type
typedef struct __xu_charset_t
{
    /// the charset type
    xu_size_t type;

    /// the charset name
    xu_char_t const* name;

    /*! get ucs4 character
     *
     * return: -1, 0 or 1
     *
     * -1:  failed, break it
     * 0:   no character, skip and continue it
     * 1:   ok, continue it
     */
    xu_long_t (*get)(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);

    /*! set ucs4 character
     *
     * return: -1, 0 or 1
     *
     * -1:  failed, break it
     * 0:   no character, skip and continue it
     * 1:   ok, continue it
     */
    xu_long_t (*set)(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

} xu_charset_t;

// the charset ref type
typedef xu_charset_t* xu_charset_ref_t;

/* *******************************************************
 * interfaces
 */

/*! the charset name
 *
 * @param type      the charset type
 *
 * @return          the charset name
 */
xu_char_t const* xu_charset_name(xu_size_t type);

/*! the charset type
 *
 * @param name      the charset name
 *
 * @return          the charset type
 */
xu_size_t xu_charset_type(xu_char_t const* name);

/*! find the charset
 *
 * @param type      the charset type
 *
 * @return          the charset pointer
 */
xu_charset_ref_t xu_charset_find(xu_size_t type);

/*! convert charset from static stream
 *
 * @param ftype     the from charset
 * @param ttype     the to charset
 * @param fst       the from stream
 * @param tst       the to stream
 *
 * @return          the converted bytes for output or -1
 */
xu_long_t xu_charset_conv_bst(xu_size_t ftype, xu_size_t ttype, xu_static_stream_ref_t fst, xu_static_stream_ref_t tst);

/*! convert charset from cstr
 *
 * @param ftype     the from charset
 * @param ttype     the to charset
 * @param cstr      the cstr
 * @param data      the data
 * @param size      the size
 *
 * @return          the converted bytes for output or -1
 */
xu_long_t xu_charset_conv_cstr(xu_size_t ftype, xu_size_t ttype, xu_char_t const* cstr, xu_byte_t* data,
                               xu_size_t size);

/*! convert charset from data
 *
 * @param ftype     the from charset
 * @param ttype     the to charset
 * @param idata     the idata
 * @param isize     the isize
 * @param odata     the odata
 * @param osize     the osize
 *
 * @return          the converted bytes for output or -1
 */
xu_long_t xu_charset_conv_data(xu_size_t ftype, xu_size_t ttype, xu_byte_t const* idata, xu_size_t isize,
                               xu_byte_t* odata, xu_size_t osize);
