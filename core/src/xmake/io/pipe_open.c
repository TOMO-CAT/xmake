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
 * @file        pipe_open.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "pipe_open"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * implementation
 */

/*
 * io.pipe_open(name, mode, buffsize)
 */
xu_int_t xm_io_pipe_open(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get pipe name and mode
    xu_char_t const* name    = luaL_checkstring(lua, 1);
    xu_char_t const* modestr = luaL_optstring(lua, 2, "r");
    xu_assert_and_check_return_val(name && modestr, 0);

    // get pipe mode value
    xu_size_t mode = XU_PIPE_MODE_RO;
    if (modestr[0] == 'w') mode = XU_PIPE_MODE_WO;

    // set block mode
    if (modestr[1] == 'B') mode |= XU_PIPE_MODE_BLOCK;

    // get buffer size
    xu_size_t buffsize = (xu_size_t)luaL_checknumber(lua, 3);

    // open pipe file
    xu_pipe_file_ref_t pipefile = xu_pipe_file_init(name, mode, buffsize);
    if (pipefile)
        xm_lua_pushpointer(lua, (xu_pointer_t)pipefile);
    else
        lua_pushnil(lua);
    return 1;
}
