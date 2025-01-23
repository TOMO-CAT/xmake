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
 * @file        pipe_write.c
 *
 */

/* *******************************************************
 * trace
 */
#define TB_TRACE_MODULE_NAME "pipe_write"
#define TB_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * implementation
 */

// io.pipe_write(pipefile, data, start, last)
xu_int_t xm_io_pipe_write(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // check pipe
    if (!xm_lua_ispointer(lua, 1))
    {
        lua_pushinteger(lua, -1);
        lua_pushliteral(lua, "invalid pipe file!");
        return 2;
    }

    // get pipe file
    tb_pipe_file_ref_t pipefile = (tb_pipe_file_ref_t)xm_lua_topointer(lua, 1);
    tb_check_return_val(pipefile, 0);

    // get data and size
    xu_size_t        size = 0;
    tb_byte_t const* data = xu_null;
    if (xm_lua_isinteger(lua, 2)) data = (tb_byte_t const*)(xu_size_t)(xu_long_t)lua_tointeger(lua, 2);
    if (xm_lua_isinteger(lua, 3)) size = (xu_size_t)lua_tointeger(lua, 3);
    if (!data || !size)
    {
        lua_pushinteger(lua, -1);
        lua_pushfstring(lua, "invalid data(%p) and size(%d)!", data, (xu_int_t)size);
        return 2;
    }
    tb_assert_static(sizeof(lua_Integer) >= sizeof(tb_pointer_t));

    // write data
    xu_long_t real = tb_pipe_file_write(pipefile, data, size);
    lua_pushinteger(lua, (xu_int_t)real);
    return 1;
}
