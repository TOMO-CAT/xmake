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
 * @file        remove.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "fwatcher.remove"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/fwatcher/prefix.h"

/* *******************************************************
 * implementation
 */

// fwatcher.remove(watchdir)
xu_int_t xm_fwatcher_remove(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // is pointer?
    if (!xm_lua_ispointer(lua, 1)) return 0;

    // get the fwatcher
    xu_fwatcher_ref_t fwatcher = (xu_fwatcher_ref_t)xm_lua_topointer(lua, 1);
    xu_check_return_val(fwatcher, 0);

    // get watchdir
    xu_char_t const* watchdir = luaL_checkstring(lua, 2);
    xu_check_return_val(watchdir, 0);

    // remove watchdir
    xu_bool_t ok = xu_fwatcher_remove(fwatcher, watchdir);

    // save result
    lua_pushboolean(lua, ok);
    return 1;
}
