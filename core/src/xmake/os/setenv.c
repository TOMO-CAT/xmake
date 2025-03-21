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
 * @file        setenv.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "setenv"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"

/* *******************************************************
 * implementation
 */
// ok = os.setenv(name, value)
xu_int_t xm_os_setenv(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the name and value
    size_t           value_size = 0;
    xu_char_t const* name       = luaL_checkstring(lua, 1);
    xu_char_t const* value      = luaL_checklstring(lua, 2, &value_size);
    xu_check_return_val(name, 0);

    // set it
    lua_pushboolean(lua, value ? xu_environment_set(name, value) : xu_false);
    return 1;
}
