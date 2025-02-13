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
 * @author      ruki
 * @file        pipe_read.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "pipe_read"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * implementation
 */

// real, data_or_errors = io.pipe_read(pipefile, size)
xu_int_t xm_io_pipe_read(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // check pipe file
    if (!xm_lua_ispointer(lua, 1))
    {
        lua_pushinteger(lua, -1);
        lua_pushliteral(lua, "invalid pipe file!");
        return 2;
    }

    // get pipe file
    xu_pipe_file_ref_t pipefile = (xu_pipe_file_ref_t)xm_lua_topointer(lua, 1);
    xu_check_return_val(pipefile, 0);

    // get data
    xu_byte_t* data = xu_null;
    if (xm_lua_isinteger(lua, 2)) data = (xu_byte_t*)(xu_size_t)(xu_long_t)lua_tointeger(lua, 2);
    if (!data)
    {
        lua_pushinteger(lua, -1);
        lua_pushfstring(lua, "invalid data(%p)!", data);
        return 2;
    }
    xu_assert_static(sizeof(lua_Integer) >= sizeof(xu_pointer_t));

    // get size
    xu_long_t size = 0;
    if (xm_lua_isinteger(lua, 3)) size = (xu_long_t)lua_tointeger(lua, 3);
    if (size <= 0)
    {
        lua_pushinteger(lua, -1);
        lua_pushfstring(lua, "invalid size(%d)!", (xu_int_t)size);
        return 2;
    }

    // read data
    xu_long_t real = xu_pipe_file_read(pipefile, data, size);
    lua_pushinteger(lua, (xu_int_t)real);
    return 1;
}
