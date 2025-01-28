/*!A cross-platform build utility based on Lua
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2015-present, TBOOX Open Source Group.
 *
 * @author      OpportunityLiu, ruki
 * @file        file_open.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "file_open"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * macros
 */

// num of bytes read to guess encoding
#define CHECK_SIZE (1024)

// is utf-8 tail character
#define IS_UTF8_TAIL(c) (c >= 0x80 && c < 0xc0)

/* *******************************************************
 * private implementation
 */
static xu_size_t xm_io_file_detect_charset(tb_byte_t const** data_ptr, xu_long_t size)
{
    // check
    tb_assert(data_ptr && *data_ptr);

    tb_byte_t const* data    = *data_ptr;
    xu_size_t        charset = XM_IO_FILE_ENCODING_BINARY;
    do
    {
        // is luajit bitcode? open as binary
        if (size >= 3 && data[0] == 27 && data[1] == 'L' && data[2] == 'J') break;

        // utf-8 with bom
        if (size >= 3 && data[0] == 239 && data[1] == 187 && data[2] == 191)
        {
            charset = TB_CHARSET_TYPE_UTF8;
            data += 3; // skip bom
            break;
        }
        if (size >= 2)
        {
            // utf16be
            if (data[0] == 254 && data[1] == 255)
            {
                charset = TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_BE;
                data += 2; // skip bom
                break;
            }
            // utf16le
            else if (data[0] == 255 && data[1] == 254)
            {
                charset = TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE;
                data += 2; // skip bom
                break;
            }
        }

        tb_sint16_t utf16be_conf = 0;
        tb_sint16_t utf16le_conf = 0;
        tb_sint16_t utf8_conf    = 0;
        tb_sint16_t ascii_conf   = 0;
        tb_sint16_t zero_count   = 0;
        for (xu_long_t i = 0; i < (size - 4) && i < CHECK_SIZE; i++)
        {
            if (data[i] == 0) zero_count++;

            if (data[i] < 0x80)
                ascii_conf++;
            else
                ascii_conf = TB_MINS16;

            if (i % 2 == 0)
            {
                if (data[i] == 0) utf16be_conf++;
                if (data[i + 1] == 0) utf16le_conf++;
            }

            if (IS_UTF8_TAIL(data[i]))
                ;
            else if (data[i] < 0x80)
                utf8_conf++;
            else if (data[i] >= 0xc0 && data[i] < 0xe0 && IS_UTF8_TAIL(data[i + 1]))
                utf8_conf++;
            else if (data[i] >= 0xe0 && data[i] < 0xf0 && IS_UTF8_TAIL(data[i + 1]) && IS_UTF8_TAIL(data[i + 2]))
                utf8_conf++;
            else if (data[i] >= 0xf0 && data[i] < 0xf8 && IS_UTF8_TAIL(data[i + 1]) && IS_UTF8_TAIL(data[i + 2]) &&
                     IS_UTF8_TAIL(data[i + 3]))
                utf8_conf++;
            else
                utf8_conf = TB_MINS16;
        }

        if (ascii_conf > 0 && zero_count <= 1)
        {
            charset = TB_CHARSET_TYPE_UTF8;
            break;
        }
        if (utf8_conf > 0 && zero_count <= 1)
        {
            charset = TB_CHARSET_TYPE_UTF8;
            break;
        }
        if (utf16be_conf > 0 && utf16be_conf > utf16le_conf)
        {
            charset = TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_BE;
            break;
        }
        if (utf16le_conf > 0 && utf16le_conf >= utf16be_conf)
        {
            charset = TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE;
            break;
        }
        if (utf8_conf > 0)
        {
            charset = TB_CHARSET_TYPE_UTF8;
            break;
        }

    } while (0);

    *data_ptr = data;
    return charset;
}
static xu_size_t xm_io_file_detect_encoding(tb_stream_ref_t stream, xu_long_t* pbomoff)
{
    // check
    xu_assert_and_check_return_val(stream && pbomoff, XM_IO_FILE_ENCODING_BINARY);

    // detect encoding
    tb_byte_t* data     = xu_null;
    xu_size_t  encoding = XM_IO_FILE_ENCODING_BINARY;
    xu_long_t  size     = tb_stream_peek(stream, &data, CHECK_SIZE);
    if (size > 0)
    {
        tb_byte_t const* p = data;
        encoding           = xm_io_file_detect_charset(&p, size);
        *pbomoff           = p - data;
    }
    return encoding;
}

/* *******************************************************
 * implementation
 */

// io.file_open(path, modestr)
xu_int_t xm_io_file_open(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get file path and mode
    xu_char_t const* path    = luaL_checkstring(lua, 1);
    xu_char_t const* modestr = luaL_optstring(lua, 2, "r");
    xu_assert_and_check_return_val(path && modestr, 0);

    // get file mode value
    xu_size_t mode;
    switch (modestr[0])
    {
    case 'w': mode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC; break;
    case 'a': mode = TB_FILE_MODE_RW | TB_FILE_MODE_APPEND | TB_FILE_MODE_CREAT; break;
    case 'r':
    default: mode = TB_FILE_MODE_RO; break;
    }

    // get file encoding
    xu_long_t       bomoff   = 0;
    tb_stream_ref_t stream   = xu_null;
    xu_bool_t       update   = !!tb_strchr(modestr, '+');
    xu_size_t       encoding = XM_IO_FILE_ENCODING_UNKNOWN;
    if (modestr[1] == 'b' || (update && modestr[2] == 'b'))
        encoding = XM_IO_FILE_ENCODING_BINARY;
    else if (tb_strstr(modestr, "utf8") || tb_strstr(modestr, "utf-8"))
        encoding = TB_CHARSET_TYPE_UTF8;
    else if (tb_strstr(modestr, "utf16le") || tb_strstr(modestr, "utf-16le"))
        encoding = TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE;
    else if (tb_strstr(modestr, "utf16be") || tb_strstr(modestr, "utf-16be"))
        encoding = TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_BE;
    else if (tb_strstr(modestr, "utf16") || tb_strstr(modestr, "utf-16"))
        encoding = TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_NE;
    else if (tb_strstr(modestr, "ansi"))
        encoding = TB_CHARSET_TYPE_ANSI;
    else if (tb_strstr(modestr, "gbk"))
        encoding = TB_CHARSET_TYPE_GBK;
    else if (tb_strstr(modestr, "gb2312"))
        encoding = TB_CHARSET_TYPE_GB2312;
    else if (tb_strstr(modestr, "iso8859"))
        encoding = TB_CHARSET_TYPE_ISO8859;
    else if (modestr[0] == 'w' || modestr[0] == 'a') // set to utf-8 if not specified for the writing mode
        encoding = TB_CHARSET_TYPE_UTF8;
    else if (modestr[0] == 'r') // detect encoding if not specified for the reading mode
    {
        stream = tb_stream_init_from_file(path, mode);
        if (stream && tb_stream_open(stream))
            encoding = xm_io_file_detect_encoding(stream, &bomoff);
        else
        {
            if (stream) tb_stream_exit(stream);
            xm_io_return_error(lua, "file not found!");
        }
    }
    else
        xm_io_return_error(lua, "invalid open mode!");
    xu_assert_and_check_return_val(encoding != XM_IO_FILE_ENCODING_UNKNOWN, 0);

    // write data with utf bom? e.g. utf8bom, utf16lebom, utf16bom
    xu_bool_t utfbom = xu_false;
    if (tb_strstr(modestr, "bom")) utfbom = xu_true;

    // open file
    xu_bool_t       open_ok  = xu_false;
    tb_stream_ref_t file_ref = xu_null;
    tb_stream_ref_t fstream  = xu_null;
    do
    {
        // init stream from file
        stream = stream ? stream : tb_stream_init_from_file(path, mode);
        xu_assert_and_check_break(stream);

        // is transcode?
        xu_bool_t is_transcode = encoding != TB_CHARSET_TYPE_UTF8 && encoding != XM_IO_FILE_ENCODING_BINARY;
        if (is_transcode)
        {
            if (modestr[0] == 'r')
                fstream = tb_stream_init_filter_from_charset(stream, encoding, TB_CHARSET_TYPE_UTF8);
            else
                fstream = tb_stream_init_filter_from_charset(stream, TB_CHARSET_TYPE_UTF8, encoding);
            xu_assert_and_check_break(fstream);

            // use fstream as file
            file_ref = fstream;
        }
        else
            file_ref = stream;

        // open file stream
        if (!tb_stream_open(file_ref)) break;

        // skip bom characters if exists
        if (bomoff > 0 && !tb_stream_seek(stream, bomoff)) break;

        // ok
        open_ok = xu_true;

    } while (0);

    // open failed?
    if (!open_ok)
    {
        // exit stream
        if (stream) tb_stream_exit(stream);
        stream = xu_null;

        // exit charset stream filter
        if (fstream) tb_stream_exit(fstream);
        fstream = xu_null;

        // return errors
        xm_io_return_error(lua, "failed to open file!");
    }

    // make file
    xm_io_file_t* file = (xm_io_file_t*)lua_newuserdata(lua, sizeof(xm_io_file_t));
    xu_assert_and_check_return_val(file, 0);

    // init file
    file->u.file_ref = file_ref;
    file->stream     = stream;
    file->fstream    = fstream;
    file->mode       = mode;
    file->type       = XM_IO_FILE_TYPE_FILE;
    file->encoding   = encoding;
    file->utfbom     = utfbom;

    // init the read/write line cache buffer
    tb_buffer_init(&file->rcache);
    tb_buffer_init(&file->wcache);
    return 1;
}
