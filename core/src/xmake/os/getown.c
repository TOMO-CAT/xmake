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
 * @author      TitanSnow
 * @file        getown.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "getown"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"
#include <sys/stat.h>
#include <unistd.h>

/* *******************************************************
 * implementation
 */
// get owner by a given path
xu_int_t xm_os_getown(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the pathname
    xu_char_t const* pathname = luaL_checkstring(lua, 1);
    xu_check_return_val(pathname, 0);

    // get stat
    struct stat sts;
    if (stat(pathname, &sts) != 0) return 0;

    // push
    lua_newtable(lua);
    lua_pushstring(lua, "uid");
    lua_pushinteger(lua, sts.st_uid);
    lua_settable(lua, -3);
    lua_pushstring(lua, "gid");
    lua_pushinteger(lua, sts.st_gid);
    lua_settable(lua, -3);

    // ok
    return 1;
}
