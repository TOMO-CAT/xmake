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
 * @file        getenv.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "getenv"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"

/* *******************************************************
 * macros
 */

// the separator
#define XM_OS_ENV_SEP ':'

/* *******************************************************
 * implementation
 */
xu_int_t xm_os_getenv(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the name
    xu_char_t const* name = luaL_checkstring(lua, 1);
    xu_check_return_val(name, 0);

    // init values
    xu_string_t values;
    if (!xu_string_init(&values)) return 0;

    // init environment
    xu_environment_ref_t environment = xu_environment_init();
    if (environment)
    {
        // load variable
        if (xu_environment_load(environment, name))
        {
            // make values
            xu_bool_t is_first = xu_true;
            xu_for_all_if(xu_char_t const*, value, environment, value)
            {
                // append separator
                if (!is_first)
                    xu_string_chrcat(&values, XM_OS_ENV_SEP);
                else
                    is_first = xu_false;

                // append value
                xu_string_cstrcat(&values, value);
            }
        }

        // exit environment
        xu_environment_exit(environment);
    }

    // save result
    if (xu_string_size(&values))
        lua_pushstring(lua, xu_string_cstr(&values));
    else
        lua_pushnil(lua);

    // exit values
    xu_string_exit(&values);

    // ok
    return 1;
}
