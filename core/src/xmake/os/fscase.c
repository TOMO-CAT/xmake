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
 * @file        fscase.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "fscase"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_os_fscase(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

#if ((XU_VERSION_MAJOR * 100) + (XU_VERSION_MINOR * 10) + XU_VERSION_ALTER) >= 174
    xu_char_t const* path = luaL_checkstring(lua, 1);
    xu_check_return_val(path, 0);

    lua_pushinteger(lua, (xu_int_t)tb_file_fscase(path));
#else
    lua_pushinteger(lua, -1);
#endif
    return 1;
}
