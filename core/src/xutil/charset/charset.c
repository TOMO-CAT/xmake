#include "xutil/charset/charset.h"
#include "xutil/algorithm/binary_find.h"
#include "xutil/container/array_iterator.h"
#include "xutil/container/iterator.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/platform/impl/charset.h"
#include "xutil/utils/utils.h"

// ascii
xu_long_t xu_charset_ascii_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_ascii_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

// utf8
xu_long_t xu_charset_utf8_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_utf8_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

// utf16
xu_long_t xu_charset_utf16_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_utf16_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

// utf32
xu_long_t xu_charset_utf32_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_utf32_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

// ucs2
xu_long_t xu_charset_ucs2_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_ucs2_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

// ucs4
xu_long_t xu_charset_ucs4_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_ucs4_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

// gb2312
// xu_long_t xu_charset_gb2312_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
// xu_long_t xu_charset_gb2312_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

// iso8859
xu_long_t xu_charset_iso8859_get(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t* ch);
xu_long_t xu_charset_iso8859_set(xu_static_stream_ref_t sstream, xu_bool_t be, xu_uint32_t ch);

// the charsets, @note: type & name is sorted
static xu_charset_t g_charsets[] = {
    {XU_CHARSET_TYPE_ASCII, "ascii", xu_charset_ascii_get, xu_charset_ascii_set},
    // {XU_CHARSET_TYPE_GB2312, "gb2312", xu_charset_gb2312_get, xu_charset_gb2312_set},
    // {XU_CHARSET_TYPE_GBK, "gbk", xu_charset_gb2312_get, xu_charset_gb2312_set},
    {XU_CHARSET_TYPE_ISO8859, "iso8859", xu_charset_iso8859_get, xu_charset_iso8859_set},
    {XU_CHARSET_TYPE_UCS2, "ucs3", xu_charset_ucs2_get, xu_charset_ucs2_set},
    {XU_CHARSET_TYPE_UCS4, "ucs4", xu_charset_ucs4_get, xu_charset_ucs4_set},
    {XU_CHARSET_TYPE_UTF16, "utf16", xu_charset_utf16_get, xu_charset_utf16_set},
    {XU_CHARSET_TYPE_UTF32, "utf32", xu_charset_utf32_get, xu_charset_utf32_set},
    {XU_CHARSET_TYPE_UTF8, "utf8", xu_charset_utf8_get, xu_charset_utf8_set}};

static xu_long_t xu_charset_comp_by_name(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t name)
{
    // check
    xu_assert(item);

    // comp
    return xu_stricmp(((xu_charset_ref_t)item)->name, (xu_char_t const*)name);
}
static xu_long_t xu_charset_comp_by_type(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t type)
{
    // check
    xu_assert(item && type);

    // comp
    return (xu_long_t)((xu_charset_ref_t)item)->type - (xu_long_t)type;
}
static xu_charset_ref_t xu_charset_find_by_name(xu_char_t const* name)
{
    // make iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator =
        xu_array_iterator_init_mem(&array_iterator, g_charsets, xu_arrayn(g_charsets), sizeof(xu_charset_t));
    xu_assert_and_check_return_val(iterator, xu_null);

    // find it by the binary search
    xu_size_t itor = xu_binary_find_all_if(iterator, xu_charset_comp_by_name, name);

    // ok?
    if (itor != xu_iterator_tail(iterator))
        return (xu_charset_ref_t)xu_iterator_item(iterator, itor);
    else
        return xu_null;
}
static xu_charset_ref_t xu_charset_find_by_type(xu_size_t type)
{
    // make iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator =
        xu_array_iterator_init_mem(&array_iterator, g_charsets, xu_arrayn(g_charsets), sizeof(xu_charset_t));
    xu_assert_and_check_return_val(iterator, xu_null);

    // find it by the binary search
    xu_size_t itor = xu_binary_find_all_if(iterator, xu_charset_comp_by_type, (xu_cpointer_t)XU_CHARSET_TYPE(type));

    // ok?
    if (itor != xu_iterator_tail(iterator))
        return (xu_charset_ref_t)xu_iterator_item(iterator, itor);
    else
        return xu_null;
}

xu_char_t const* xu_charset_name(xu_size_t type)
{
    // find
    xu_charset_ref_t charset = xu_charset_find_by_type(type);
    xu_assert_and_check_return_val(charset, xu_null);

    // type
    return charset->name;
}
xu_size_t xu_charset_type(xu_char_t const* name)
{
    // find
    xu_charset_ref_t charset = xu_charset_find_by_name(name);
    xu_assert_and_check_return_val(charset, XU_CHARSET_TYPE_NONE);

    // type
    return charset->type;
}
xu_charset_ref_t xu_charset_find(xu_size_t type) { return xu_charset_find_by_type(type); }
xu_long_t xu_charset_conv_bst(xu_size_t ftype, xu_size_t ttype, xu_static_stream_ref_t fst, xu_static_stream_ref_t tst)
{
    // check
    xu_assert_and_check_return_val(XU_CHARSET_TYPE_OK(ftype) && XU_CHARSET_TYPE_OK(ttype) && fst && tst, -1);
    xu_assert_and_check_return_val(xu_static_stream_valid(fst) && xu_static_stream_valid(tst), -1);

    // attempt to convert charset using the platform implementation
    xu_long_t ok = xu_charset_conv_impl(ftype, ttype, fst, tst);
    xu_check_return_val(ok < 0, ok);

    // init the charset
    xu_charset_ref_t fr = xu_charset_find_by_type(ftype);
    xu_charset_ref_t to = xu_charset_find_by_type(ttype);
    xu_assert_and_check_return_val(fr && to && fr->get && fr->set, -1);

    // no data?
    xu_check_return_val(xu_static_stream_left(fst), 0);

    // big endian?
    xu_bool_t fbe = !(ftype & XU_CHARSET_TYPE_LE) ? xu_true : xu_false;
    xu_bool_t tbe = !(ttype & XU_CHARSET_TYPE_LE) ? xu_true : xu_false;

    // walk
    xu_uint32_t      ch;
    xu_byte_t const* tp = xu_static_stream_pos(tst);
    while (xu_static_stream_left(fst) && xu_static_stream_left(tst))
    {
        // get ucs4 character
        xu_long_t ok = 0;
        if ((ok = fr->get(fst, fbe, &ch)) > 0)
        {
            // set ucs4 character
            if (to->set(tst, tbe, ch) < 0) break;
        }
        else if (ok < 0)
            break;
    }

    // ok?
    return xu_static_stream_pos(tst) - tp;
}
xu_long_t xu_charset_conv_cstr(xu_size_t ftype, xu_size_t ttype, xu_char_t const* cstr, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(XU_CHARSET_TYPE_OK(ftype) && XU_CHARSET_TYPE_OK(ttype) && cstr && data && size, -1);

    // conv
    return xu_charset_conv_data(ftype, ttype, (xu_byte_t const*)cstr, xu_strlen(cstr), data, size);
}
xu_long_t xu_charset_conv_data(xu_size_t ftype, xu_size_t ttype, xu_byte_t const* idata, xu_size_t isize,
                               xu_byte_t* odata, xu_size_t osize)
{
    // check
    xu_assert_and_check_return_val(XU_CHARSET_TYPE_OK(ftype) && XU_CHARSET_TYPE_OK(ttype) && idata && odata && osize,
                                   -1);
    xu_check_return_val(isize, 0);

    // init static stream
    xu_static_stream_t ist;
    xu_static_stream_t ost;
    xu_static_stream_init(&ist, (xu_byte_t*)idata, isize);
    xu_static_stream_init(&ost, odata, osize);

    // conv
    return xu_charset_conv_bst(ftype, ttype, &ist, &ost);
}
