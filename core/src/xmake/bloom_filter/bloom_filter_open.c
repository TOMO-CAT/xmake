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
 * @file        bloom_filter_open.c
 *
 */

/* *******************************************************
 * trace
 */
#define TB_TRACE_MODULE_NAME                "bloom_filter_open"
#define TB_TRACE_MODULE_DEBUG               (0)

/* *******************************************************
 * includes
 */
#include "xmake/bloom_filter/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_bloom_filter_open(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get arguments
    xu_int_t probability = (xu_int_t)lua_tointeger(lua, 1);
    xu_int_t hash_count = (xu_int_t)lua_tointeger(lua, 2);
    xu_int_t item_maxn = (xu_int_t)lua_tointeger(lua, 3);
    if (hash_count > 16 || item_maxn < 0)
    {
        lua_pushnil(lua);
        lua_pushfstring(lua, "invalid hash count(%p) and item maxn(%d)!", hash_count, item_maxn);
        return 2;
    }

    // init the bloom filter
    tb_bloom_filter_ref_t filter = tb_bloom_filter_init(probability, hash_count, item_maxn, tb_element_str(xu_true));
    if (filter) xm_lua_pushpointer(lua, (tb_pointer_t)filter);
    else lua_pushnil(lua);
    return 1;
}
