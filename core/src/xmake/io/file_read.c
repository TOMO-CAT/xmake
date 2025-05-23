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
 * @file        file_read.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "file_read"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * types
 */
typedef enum __xm_pushline_state_e
{
    PL_EOF,
    PL_FIN,
    PL_CONL,
    PL_FAIL,

} xm_pushline_state_e;

/* *******************************************************
 * implementation
 */
static xu_long_t xm_io_file_buffer_readline(xu_stream_ref_t stream, xu_buffer_ref_t line)
{
    // check
    xu_assert_and_check_return_val(stream && line, -1);

    // read line and reserve crlf
    xu_bool_t  eof    = xu_false;
    xu_hize_t  offset = 0;
    xu_byte_t* data   = xu_null;
    xu_hong_t  size   = xu_stream_size(stream);
    while (size < 0 || (offset = xu_stream_offset(stream)) < size)
    {
        xu_long_t real = xu_stream_peek(stream, &data, XU_STREAM_BLOCK_MAXN);
        if (real > 0)
        {
            xu_char_t const* e = xu_strnchr((xu_char_t const*)data, real, '\n');
            if (e)
            {
                xu_size_t n = (xu_byte_t const*)e + 1 - data;
                if (!xu_stream_skip(stream, n)) return -1;
                xu_buffer_memncat(line, data, n);
                break;
            }
            else
            {
                if (!xu_stream_skip(stream, real)) return -1;
                xu_buffer_memncat(line, data, real);
            }
        }
        else if (!real)
        {
            real = xu_stream_wait(stream, XU_STREAM_WAIT_READ, -1);
            if (real <= 0)
            {
                eof = xu_true;
                break;
            }
        }
        else
        {
            eof = xu_true;
            break;
        }
    }

    // ok?
    xu_size_t linesize = xu_buffer_size(line);
    if (linesize)
        return linesize;
    else
        return (eof || xu_stream_beof(stream)) ? -1 : 0;
}
static xu_int_t xm_io_file_buffer_pushline(xu_buffer_ref_t buf, xm_io_file_t* file, xu_char_t const* continuation,
                                           xu_bool_t keep_crlf)
{
    // check
    xu_assert(buf && file && continuation && xm_io_file_is_file(file) && file->u.file_ref);

    // is binary?
    xu_bool_t is_binary = file->encoding == XM_IO_FILE_ENCODING_BINARY;
    if (is_binary)
    {
        continuation = "";
        keep_crlf    = xu_true;
    }

    // clear line buffer
    xu_buffer_clear(&file->rcache);

    // read line data
    xu_long_t size = xm_io_file_buffer_readline(file->u.file_ref, &file->rcache);

    // translate line data
    xu_int_t   result = PL_FAIL;
    xu_char_t* data   = xu_null;
    xu_size_t  conlen = xu_strlen(continuation);
    do
    {
        // eof?
        if (size < 0)
        {
            result = PL_EOF;
            break;
        }

        // patch two '\0'
        xu_buffer_memncat(&file->rcache, (xu_byte_t const*)"\0\0", 2);

        // get line data
        data = (xu_char_t*)xu_buffer_data(&file->rcache);
        xu_assert_and_check_break(data);

        // no lf found
        if (size > 0 && data[size - 1] != '\n')
            result = PL_FIN;
        else if (size > 1)
        {
            // crlf? => lf
            if (!is_binary && data[size - 2] == '\r')
            {
                data[size - 2] = '\n';
                size--;
            }

            // has continuation?
            xu_bool_t has_conline = conlen && size >= conlen + 1 &&
                                    xu_strncmp(continuation, (xu_char_t const*)(data + size - conlen - 1), conlen) == 0;

            // do not keep crlf, strip the last lf
            if (!keep_crlf && !has_conline) size--;

            // strip it if has continuation?
            if (has_conline) size -= conlen + 1;
            data[size] = '\0';

            result = has_conline ? PL_CONL : PL_FIN;
        }
        else
        {
            // a single '\n'
            if (!keep_crlf) size = 0;
            result = PL_FIN;
        }

    } while (0);

    // push line data
    if (data && size > 0 && (result == PL_FIN || result == PL_CONL))
        xu_buffer_memncat(buf, (xu_byte_t const*)data, size);

    // return result
    return result;
}
static xu_int_t xm_io_file_read_all_directly(lua_State* lua, xm_io_file_t* file)
{
    // check
    xu_assert(lua && file && xm_io_file_is_file(file) && file->u.file_ref);

    // init buffer
    xu_buffer_t buf;
    if (!xu_buffer_init(&buf)) xm_io_return_error(lua, "init buffer failed!");

    // read all
    xu_byte_t       data[XU_STREAM_BLOCK_MAXN];
    xu_stream_ref_t stream = file->u.file_ref;
    while (!xu_stream_beof(stream))
    {
        xu_long_t real = xu_stream_read(stream, data, sizeof(data));
        if (real > 0)
            xu_buffer_memncat(&buf, data, real);
        else if (!real)
        {
            real = xu_stream_wait(stream, XU_STREAM_WAIT_READ, -1);
            xu_check_break(real > 0);
        }
        else
            break;
    }

    if (xu_buffer_size(&buf))
        lua_pushlstring(lua, (xu_char_t const*)xu_buffer_data(&buf), xu_buffer_size(&buf));
    else
        lua_pushliteral(lua, "");
    xu_buffer_exit(&buf);
    return 1;
}
static xu_int_t xm_io_file_read_all(lua_State* lua, xm_io_file_t* file, xu_char_t const* continuation)
{
    // check
    xu_assert(lua && file && continuation && xm_io_file_is_file(file) && file->u.file_ref);

    // is binary? read all directly
    xu_bool_t is_binary = file->encoding == XM_IO_FILE_ENCODING_BINARY;
    if (is_binary) return xm_io_file_read_all_directly(lua, file);

    // init buffer
    xu_buffer_t buf;
    if (!xu_buffer_init(&buf)) xm_io_return_error(lua, "init buffer failed!");

    // read all
    xu_bool_t has_content = xu_false;
    while (1)
    {
        switch (xm_io_file_buffer_pushline(&buf, file, continuation, xu_true))
        {
        case PL_EOF:
            if (!has_content)
                lua_pushliteral(lua, "");
            else
                lua_pushlstring(lua, (xu_char_t const*)xu_buffer_data(&buf), xu_buffer_size(&buf));
            xu_buffer_exit(&buf);
            return 1;
        case PL_FIN:
        case PL_CONL: has_content = xu_true; continue;
        case PL_FAIL:
        default:
            xu_buffer_exit(&buf);
            xm_io_return_error(lua, "failed to read all");
            break;
        }
    }
}

static xu_int_t xm_io_file_read_line(lua_State* lua, xm_io_file_t* file, xu_char_t const* continuation,
                                     xu_bool_t keep_crlf)
{
    // check
    xu_assert(lua && file && continuation && xm_io_file_is_file(file) && file->u.file_ref);

    // init buffer
    xu_buffer_t buf;
    if (!xu_buffer_init(&buf)) xm_io_return_error(lua, "init buffer failed!");

    // read line
    xu_bool_t has_content = xu_false;
    while (1)
    {
        switch (xm_io_file_buffer_pushline(&buf, file, continuation, keep_crlf))
        {
        case PL_EOF:
            if (!has_content)
                lua_pushnil(lua);
            else
                lua_pushlstring(lua, (xu_char_t const*)xu_buffer_data(&buf), xu_buffer_size(&buf));
            xu_buffer_exit(&buf);
            return 1;
        case PL_FIN:
            lua_pushlstring(lua, (xu_char_t const*)xu_buffer_data(&buf), xu_buffer_size(&buf));
            xu_buffer_exit(&buf);
            return 1;
        case PL_CONL: has_content = xu_true; continue;
        case PL_FAIL:
        default:
            xu_buffer_exit(&buf);
            xm_io_return_error(lua, "failed to readline");
            break;
        }
    }
}

static xu_int_t xm_io_file_read_n(lua_State* lua, xm_io_file_t* file, xu_char_t const* continuation, xu_long_t n)
{
    // check
    xu_assert(lua && file && continuation && xm_io_file_is_file(file) && file->u.file_ref);

    // check continuation
    if (*continuation != '\0') xm_io_return_error(lua, "continuation is not supported for read number of bytes");

    // check encoding
    if (file->encoding != XM_IO_FILE_ENCODING_BINARY)
        xm_io_return_error(lua, "read number of bytes only allows binary file, reopen with 'rb' and try again");

    xu_bool_t ok = xu_false;
    if (n == 0)
    {
        xu_byte_t* data = xu_null;
        if (xu_stream_need(file->u.file_ref, &data, 1))
        {
            lua_pushliteral(lua, "");
            ok = xu_true;
        }
    }
    else
    {
        xu_byte_t* bufptr = xu_buffer_resize(&file->rcache, n + 1);
        if (bufptr)
        {
            if (xu_stream_bread(file->u.file_ref, bufptr, n))
            {
                lua_pushlstring(lua, (xu_char_t const*)bufptr, n);
                ok = xu_true;
            }
        }
    }
    if (!ok) lua_pushnil(lua);
    return 1;
}

static xu_size_t xm_io_file_std_buffer_pushline(xu_buffer_ref_t buf, xm_io_file_t* file, xu_char_t const* continuation,
                                                xu_bool_t keep_crlf)
{
    // check
    xu_assert(buf && file && continuation && xm_io_file_is_std(file));

    // get input buffer
    xu_char_t strbuf[8192];
    xu_size_t buflen = 0;
    xu_size_t result = PL_FAIL;
    if (xu_stdfile_gets(file->u.std_ref, strbuf, xu_arrayn(strbuf) - 1))
        buflen = xu_strlen(strbuf);
    else
        return PL_EOF;

    xu_size_t conlen = xu_strlen(continuation);
    if (buflen > 0 && strbuf[buflen - 1] != '\n')
    {
        // end of file, no lf found
        result = PL_FIN;
    }
    else if (buflen > 1)
    {
        // crlf? => lf
        if (strbuf[buflen - 2] == '\r')
        {
            strbuf[buflen - 2] = '\n';
            buflen--;
        }

        // has continuation?
        xu_bool_t has_conline =
            conlen && buflen >= conlen + 1 && xu_strncmp(continuation, (strbuf + buflen - conlen - 1), conlen) == 0;

        // do not keep crlf, strip the last lf
        if (!keep_crlf && !has_conline) buflen--;

        // strip it if has continuation?
        if (has_conline) buflen -= conlen + 1;
        strbuf[buflen] = '\0';

        result = has_conline ? PL_CONL : PL_FIN;
    }
    else
    {
        // a single '\n'
        if (!keep_crlf) buflen = 0;
        result = PL_FIN;
    }

    if (result == PL_FIN || result == PL_CONL) xu_buffer_memncat(buf, (xu_byte_t const*)strbuf, buflen);
    return result;
}

static xu_int_t xm_io_file_std_read_line(lua_State* lua, xm_io_file_t* file, xu_char_t const* continuation,
                                         xu_bool_t keep_crlf)
{
    // check
    xu_assert(lua && file && continuation && xm_io_file_is_std(file));

    // init buffer
    xu_buffer_t buf;
    if (!xu_buffer_init(&buf)) xm_io_return_error(lua, "init buffer failed!");

    // read line
    xu_bool_t has_content = xu_false;
    while (1)
    {
        switch (xm_io_file_std_buffer_pushline(&buf, file, continuation, keep_crlf))
        {
        case PL_EOF:
            if (!has_content)
                lua_pushnil(lua);
            else
                lua_pushlstring(lua, (xu_char_t const*)xu_buffer_data(&buf), xu_buffer_size(&buf));
            xu_buffer_exit(&buf);
            return 1;
        case PL_FIN:
            lua_pushlstring(lua, (xu_char_t const*)xu_buffer_data(&buf), xu_buffer_size(&buf));
            xu_buffer_exit(&buf);
            return 1;
        case PL_CONL: has_content = xu_true; continue;
        case PL_FAIL:
        default:
            xu_buffer_exit(&buf);
            xm_io_return_error(lua, "failed to readline");
            break;
        }
    }
}

static xu_int_t xm_io_file_std_read_all(lua_State* lua, xm_io_file_t* file, xu_char_t const* continuation)
{
    // check
    xu_assert(lua && file && continuation && xm_io_file_is_std(file));

    // init buffer
    xu_buffer_t buf;
    if (!xu_buffer_init(&buf)) xm_io_return_error(lua, "init buffer failed!");

    // read all
    xu_bool_t has_content = xu_false;
    while (1)
    {
        switch (xm_io_file_std_buffer_pushline(&buf, file, continuation, xu_true))
        {
        case PL_EOF:
            if (!has_content)
                lua_pushliteral(lua, "");
            else
                lua_pushlstring(lua, (xu_char_t const*)xu_buffer_data(&buf), xu_buffer_size(&buf));
            xu_buffer_exit(&buf);
            return 1;
        case PL_FIN:
        case PL_CONL: has_content = xu_true; continue;
        case PL_FAIL:
        default:
            xu_buffer_exit(&buf);
            xm_io_return_error(lua, "failed to readline");
            break;
        }
    }
}

static xu_int_t xm_io_file_std_read_n(lua_State* lua, xm_io_file_t* file, xu_char_t const* continuation, xu_long_t n)
{
    // check
    xu_assert(lua && file && continuation && xm_io_file_is_std(file));

    // check continuation
    if (*continuation != '\0') xm_io_return_error(lua, "continuation is not supported for std streams");

    // io.read(0)
    if (n == 0)
    {
        xu_char_t ch;
        if (!xu_stdfile_peek(file->u.std_ref, &ch))
            lua_pushnil(lua);
        else
            lua_pushliteral(lua, "");
        return 1;
    }

    // get line buffer
    xu_byte_t* buf_ptr = xu_buffer_resize(&file->rcache, (xu_size_t)n);
    xu_assert(buf_ptr);

    // io.read(n)
    if (xu_stdfile_read(file->u.std_ref, buf_ptr, (xu_size_t)n))
        lua_pushlstring(lua, (xu_char_t const*)buf_ptr, (size_t)n);
    else
        lua_pushnil(lua);
    return 1;
}

static xu_int_t xm_io_file_std_read_num(lua_State* lua, xm_io_file_t* file, xu_char_t const* continuation)
{
    // check
    xu_assert(lua && file && continuation && xm_io_file_is_std(file));

    // check continuation
    if (*continuation != '\0') xm_io_return_error(lua, "continuation is not supported for std streams");

    // read number
    xu_char_t strbuf[512];
    if (xu_stdfile_gets(file->u.std_ref, strbuf, xu_arrayn(strbuf)))
        lua_pushnumber(lua, xu_s10tod(strbuf)); // TODO check invalid float number string and push nil
    else
        lua_pushnil(lua);
    return 1;
}

/* io.file_read(file, [mode, [continuation]])
 * io.file_read(file, "all", "\\")
 * io.file_read(file, "L")
 * io.file_read(file, "l")
 * io.file_read(file, "n")
 * io.file_read(file, 10)
 */
xu_int_t xm_io_file_read(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // is user data?
    if (!lua_isuserdata(lua, 1)) xm_io_return_error(lua, "read(invalid file)!");

    // get file
    xm_io_file_t* file = (xm_io_file_t*)lua_touserdata(lua, 1);
    xu_check_return_val(file, 0);

    // get arguments
    xu_char_t const* mode         = luaL_optstring(lua, 2, "l");
    xu_char_t const* continuation = luaL_optstring(lua, 3, "");
    xu_assert_and_check_return_val(mode && continuation, 0);

    xu_long_t count = -1;
    if (lua_isnumber(lua, 2))
    {
        count = (xu_long_t)lua_tointeger(lua, 2);
        if (count < 0) xm_io_return_error(lua, "invalid read size, must be positive nubmber or 0");
    }
    else if (*mode == '*')
        mode++;

    if (xm_io_file_is_file(file))
    {
        if (count >= 0) return xm_io_file_read_n(lua, file, continuation, count);
        switch (*mode)
        {
        case 'a': return xm_io_file_read_all(lua, file, continuation);
        case 'L': return xm_io_file_read_line(lua, file, continuation, xu_true);
        case 'n': xm_io_return_error(lua, "read number is not implemented");
        case 'l': return xm_io_file_read_line(lua, file, continuation, xu_false);
        default: xm_io_return_error(lua, "unknonwn read mode"); return 0;
        }
    }
    else
    {
        if (count >= 0) return xm_io_file_std_read_n(lua, file, continuation, count);
        switch (*mode)
        {
        case 'a': return xm_io_file_std_read_all(lua, file, continuation);
        case 'L': return xm_io_file_std_read_line(lua, file, continuation, xu_true);
        case 'n': return xm_io_file_std_read_num(lua, file, continuation);
        case 'l': return xm_io_file_std_read_line(lua, file, continuation, xu_false);
        default: xm_io_return_error(lua, "unknonwn read mode"); return 0;
        }
    }
}
