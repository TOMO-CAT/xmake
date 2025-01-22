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
 * @file        free.c
 *
 */

/* *******************************************************
 * trace
 */
#define TB_TRACE_MODULE_NAME "free"
#define TB_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/libc/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_libc_free(lua_State* lua)
{
    // check
    tb_assert_and_check_return_val(lua, 0);

    // do free
    tb_pointer_t data = (tb_pointer_t)(xu_size_t)luaL_checkinteger(lua, 1);
    if (data) tb_free(data);
    return 0;
}
