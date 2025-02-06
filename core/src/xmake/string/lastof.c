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
 * @file        lastof.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "string_lastof"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/string/prefix.h"

/* *******************************************************
 * private implementation
 */
static xu_void_t xm_string_lastof_str(lua_State* lua, xu_char_t const* cstr, xu_size_t nstr, xu_char_t const* csubstr,
                                      xu_size_t nsubstr)
{
    // find it
    xu_char_t const* curr = xu_null;
    xu_char_t const* next = cstr;
    do
    {
        next = xu_strstr(next, csubstr); // faster than tb_strnstr()
        if (next)
        {
            curr = next;
            next += nsubstr;
        }

    } while (!next);

    // found?
    if (curr)
        lua_pushinteger(lua, curr - cstr + 1);
    else
        lua_pushnil(lua);
}
static xu_void_t xm_string_lastof_chr(lua_State* lua, xu_char_t const* cstr, xu_size_t nstr, xu_char_t ch)
{
    xu_char_t const* pos = tb_strrchr(cstr, ch); // faster than tb_strnrchr()
    if (pos)
        lua_pushinteger(lua, pos - cstr + 1);
    else
        lua_pushnil(lua);
}

/* *******************************************************
 * implementation
 */

/* lastof string (only support plain text)
 *
 * @param str             the string
 * @param substr          the substring
 */
xu_int_t xm_string_lastof(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get string
    size_t           nstr = 0;
    xu_char_t const* cstr = luaL_checklstring(lua, 1, &nstr);

    // get substring
    size_t           nsubstr = 0;
    xu_char_t const* csubstr = luaL_checklstring(lua, 2, &nsubstr);

    // lastof it
    lua_newtable(lua);
    if (nsubstr == 1)
        xm_string_lastof_chr(lua, cstr, (xu_size_t)nstr, csubstr[0]);
    else
        xm_string_lastof_str(lua, cstr, (xu_size_t)nstr, csubstr, nsubstr);
    return 1;
}
