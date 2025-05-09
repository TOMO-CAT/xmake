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
 * @file        close.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "process.close"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/process/prefix.h"

/* *******************************************************
 * implementation
 */

// process.close(p)
xu_int_t xm_process_close(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // is pointer?
    if (!xm_lua_ispointer(lua, 1)) return 0;

    // get the process
    xu_process_ref_t process = (xu_process_ref_t)xm_lua_topointer(lua, 1);
    xu_check_return_val(process, 0);

    // exit process
    xu_process_exit(process);

    // save result: ok
    lua_pushboolean(lua, xu_true);
    return 1;
}
