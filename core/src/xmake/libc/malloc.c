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
 * @file        malloc.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "malloc"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/libc/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_libc_malloc(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // do malloc
    xu_pointer_t data = xu_null;
    xu_long_t    size = (xu_long_t)luaL_checkinteger(lua, 1);
    if (size > 0) data = xu_malloc(size);
    lua_pushinteger(lua, (lua_Integer)data);
    return 1;
}
