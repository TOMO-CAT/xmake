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
 * @file        startswith.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "startswith"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/string/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_string_startswith(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the string and prefix
    size_t           prefix_size = 0;
    xu_char_t const* string      = luaL_checkstring(lua, 1);
    xu_char_t const* prefix      = luaL_checklstring(lua, 2, &prefix_size);
    xu_check_return_val(string && prefix, 0);

    // string:startswith(prefix)?
    lua_pushboolean(lua, !xu_strncmp(string, prefix, (xu_size_t)prefix_size));

    // ok
    return 1;
}
