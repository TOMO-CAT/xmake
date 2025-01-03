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
 * @file        getenvs.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "getenvs"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "xmake/os/prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the separator
#define XM_OS_ENV_SEP                    ':'


/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the user environment
extern tb_char_t** environ;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_int_t xm_os_getenvs(lua_State* lua)
{
    // check
    tb_assert_and_check_return_val(lua, 0);

    // init table
    lua_newtable(lua);

    tb_char_t const** p = (tb_char_t const**)environ;
    if (p)
    {
        tb_int_t  i = 1;
        tb_size_t n = 0;
        while (*p)
        {
            n = tb_strlen(*p);
            if (n)
            {
                lua_pushstring(lua, *p);
                lua_rawseti(lua, -2, i++);
            }
            p++;
        }
    }

    // ok
    return 1;
}
