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
 * @file        pipe_openpair.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "pipe_openpair"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * implementation
 */

/*
 * io.pipe_openpair(mode, buffsize)
 */
xu_int_t xm_io_pipe_openpair(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get pipe mode
    xu_char_t const* modestr = luaL_optstring(lua, 1, "AA");
    xu_assert_and_check_return_val(modestr, 0);

    // init mode
    xu_size_t mode[2] = {0};
    if (modestr[0] == 'B') mode[0] |= XU_PIPE_MODE_BLOCK;
    if (modestr[1] == 'B') mode[1] |= XU_PIPE_MODE_BLOCK;

    // get buffer size
    xu_size_t buffsize = (xu_size_t)luaL_checknumber(lua, 2);

    // init pipe
    xu_pipe_file_ref_t pipefile[2];
    if (xu_pipe_file_init_pair(pipefile, mode, buffsize))
    {
        xm_lua_pushpointer(lua, (xu_pointer_t)pipefile[0]);
        xm_lua_pushpointer(lua, (xu_pointer_t)pipefile[1]);
    }
    else
    {
        lua_pushnil(lua);
        lua_pushnil(lua);
    }
    return 2;
}
