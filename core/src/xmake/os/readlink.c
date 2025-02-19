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
 * @file        readlink.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "readlink"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"
#include <unistd.h>

/* *******************************************************
 * implementation
 */
xu_int_t xm_os_readlink(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the path
    xu_char_t const* path = luaL_checkstring(lua, 1);
    xu_check_return_val(path, 0);

    // is link?
    xu_char_t srcpath[XU_PATH_MAXN];
    xu_long_t size = readlink(path, srcpath, XU_PATH_MAXN);
    if (size == XU_PATH_MAXN)
    {
        xu_size_t  maxn = XU_PATH_MAXN * 2;
        xu_char_t* data = (xu_char_t*)xu_malloc(maxn);
        if (data)
        {
            xu_long_t size = readlink(path, data, maxn);
            if (size > 0 && size < maxn)
            {
                data[size] = '\0';
                lua_pushstring(lua, data);
            }
            else
                lua_pushnil(lua);
            xu_free(data);
        }
    }
    else if (size >= 0 && size < XU_PATH_MAXN)
    {
        srcpath[size] = '\0';
        lua_pushstring(lua, srcpath);
    }
    else
        lua_pushnil(lua);

    // ok
    return 1;
}
