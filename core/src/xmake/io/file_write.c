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
 * @file        file_write.c
 *
 */

/* *******************************************************
 * trace
 */
#define TB_TRACE_MODULE_NAME "file_write"
#define TB_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * private implementation
 */
static xu_void_t xm_io_file_write_file_utfbom(xm_io_file_t* file)
{
    // check
    tb_assert(file && xm_io_file_is_file(file) && file->u.file_ref);

    // write bom
    switch (file->encoding)
    {
    case TB_CHARSET_TYPE_UTF8:
    {
        static tb_byte_t bom[] = {0xef, 0xbb, 0xbf};
        tb_stream_bwrit(file->u.file_ref, bom, sizeof(bom));
    }
    break;
    case TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE:
    {
        static tb_byte_t bom[] = {0xff, 0xfe};
        tb_stream_bwrit(file->u.file_ref, bom, sizeof(bom));
    }
    break;
    case TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_BE:
    {
        static tb_byte_t bom[] = {0xfe, 0xff};
        tb_stream_bwrit(file->u.file_ref, bom, sizeof(bom));
    }
    break;
    default: break;
    }
}
static xu_void_t xm_io_file_write_file_directly(xm_io_file_t* file, tb_byte_t const* data, xu_size_t size)
{
    tb_assert(file && data && xm_io_file_is_file(file) && file->u.file_ref);
    tb_stream_bwrit(file->u.file_ref, data, size);
}
static xu_void_t xm_io_file_write_file_transcrlf(xm_io_file_t* file, tb_byte_t const* data, xu_size_t size)
{
    // check
    tb_assert(file && data && xm_io_file_is_file(file) && file->u.file_ref);
    return xm_io_file_write_file_directly(file, data, size);
}
static xu_void_t xm_io_file_write_std(xm_io_file_t* file, tb_byte_t const* data, xu_size_t size)
{
    // check
    tb_assert(file && data && xm_io_file_is_std(file));

    // check type
    xu_size_t type = (file->type & ~XM_IO_FILE_FLAG_TTY);
    tb_check_return(type != XM_IO_FILE_TYPE_STDIN);

    // write data to stdout/stderr
    tb_stdfile_writ(file->u.std_ref, data, size);
}

/* *******************************************************
 * implementation
 */

// io.file_write(file, ...)
xu_int_t xm_io_file_write(lua_State* lua)
{
    // check
    tb_assert_and_check_return_val(lua, 0);

    // is user data?
    if (!lua_isuserdata(lua, 1)) xm_io_return_error(lua, "write(invalid file)!");

    // get file
    xm_io_file_t* file = (xm_io_file_t*)lua_touserdata(lua, 1);
    tb_check_return_val(file, 0);

    // write file data
    xu_int_t narg = lua_gettop(lua);
    if (narg > 1)
    {
        xu_bool_t is_binary = file->encoding == XM_IO_FILE_ENCODING_BINARY;
        for (xu_int_t i = 2; i <= narg; i++)
        {
            // get data
            size_t           datasize = 0;
            tb_byte_t const* data     = xu_null;
            if (lua_isstring(lua, i))
                data = (tb_byte_t const*)luaL_checklstring(lua, i, &datasize);
            else if (lua_istable(lua, i))
            {
                // get bytes data
                lua_pushstring(lua, "data");
                lua_gettable(lua, i);
                if (xm_lua_isinteger(lua, -1)) data = (tb_byte_t const*)(xu_size_t)(tb_long_t)lua_tointeger(lua, -1);
                lua_pop(lua, 1);
                tb_assert_static(sizeof(lua_Integer) >= sizeof(tb_pointer_t));

                lua_pushstring(lua, "size");
                lua_gettable(lua, i);
                if (xm_lua_isinteger(lua, -1)) datasize = (xu_size_t)lua_tointeger(lua, -1);
                lua_pop(lua, 1);

                // mark as binary data
                is_binary = xu_true;
            }

            tb_check_continue(datasize);
            tb_assert_and_check_break(data);

            // write data to std or file
            if (xm_io_file_is_std(file))
                xm_io_file_write_std(file, data, (xu_size_t)datasize);
            else if (is_binary)
                xm_io_file_write_file_directly(file, data, (xu_size_t)datasize);
            else
            {
                // write utf bom first?
                if (file->utfbom)
                {
                    xm_io_file_write_file_utfbom(file);
                    file->utfbom = xu_false;
                }
                xm_io_file_write_file_transcrlf(file, data, (xu_size_t)datasize);
            }
        }
    }
    lua_settop(lua, 1);
    lua_pushboolean(lua, xu_true);
    return 1;
}
