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
 * @file        isfile.c
 *
 */

/* *******************************************************
 * trace
 */
#define TB_TRACE_MODULE_NAME                "isfile"
#define TB_TRACE_MODULE_DEBUG               (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_os_isfile(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the path
    xu_char_t const* path = luaL_checkstring(lua, 1);
    tb_check_return_val(path, 0);

    // is file?
    xu_file_info_t info = {0};
    lua_pushboolean(lua, xu_file_info(path, &info) && (info.type == TB_FILE_TYPE_FILE));
    return 1;
}
