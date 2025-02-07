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
 * @file        file_seek.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "file_seek"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * implementation
 */

// io.file_seek(file, [whence [, offset]])
xu_int_t xm_io_file_seek(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // is user data?
    if (!lua_isuserdata(lua, 1)) xm_io_return_error(lua, "seek(invalid file)!");

    // get file
    xm_io_file_t* file = (xm_io_file_t*)lua_touserdata(lua, 1);
    xu_check_return_val(file, 0);

    // get whence and offset
    xu_char_t const* whence = luaL_optstring(lua, 2, "cur");
    xu_hong_t        offset = (xu_hong_t)luaL_optnumber(lua, 3, 0);
    xu_assert_and_check_return_val(whence, 0);

    // seek file
    if (xm_io_file_is_file(file))
    {
        xu_assert(file->u.file_ref);
        switch (*whence)
        {
        case 's': // "set"
            break;
        case 'e': // "end"
        {
            xu_hong_t size = xu_stream_size(file->u.file_ref);
            if (size > 0 && size + offset <= size)
                offset = size + offset;
            else
                xm_io_return_error(lua, "seek failed, invalid offset!");
        }
        break;
        default: // "cur"
            offset = xu_stream_offset(file->u.file_ref) + offset;
            break;
        }

        if (xu_stream_seek(file->u.file_ref, offset))
        {
            lua_pushnumber(lua, (lua_Number)offset);
            return 1;
        }
        else
            xm_io_return_error(lua, "seek failed!");
    }
    else
        xm_io_return_error(lua, "seek is not supported on this file");
}
