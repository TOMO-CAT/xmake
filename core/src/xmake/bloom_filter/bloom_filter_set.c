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
 * @file        bloom_filter_set.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "bloom_filter_set"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/bloom_filter/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_bloom_filter_set(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // is pointer?
    if (!xm_lua_ispointer(lua, 1)) return 0;

    // get the bloom filter
    xu_bloom_filter_ref_t filter = (xu_bloom_filter_ref_t)xm_lua_topointer(lua, 1);
    xu_check_return_val(filter, 0);

    // get item
    xu_char_t const* item = luaL_checkstring(lua, 2);
    xu_assert_and_check_return_val(item, 0);

    // set item
    xu_bool_t ok = xu_bloom_filter_set(filter, item);
    lua_pushboolean(lua, ok);
    return 1;
}
