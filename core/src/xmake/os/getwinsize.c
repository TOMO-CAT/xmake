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
 * @author      TitanSnow
 * @file        getwinsize.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "getwinsize"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"
#include <errno.h> // for errno
#include <sys/ioctl.h>
#include <unistd.h> // for STDOUT_FILENO

/* *******************************************************
 * implementation
 */

// get console window size
xu_int_t xm_os_getwinsize(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // init default window size (we will not consider winsize limit if cannot get it)
    xu_int_t w = XU_MAXS16, h = XU_MAXS16;

    // get winsize
    struct winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0)
    {
        w = (xu_int_t)size.ws_col;
        h = (xu_int_t)size.ws_row;
    }

    /* local winsize = os.getwinsize()
     *
     * return
     * {
     *      width = -1 or ..
     * ,    height = -1 or ..
     * }
     */
    lua_newtable(lua);
    lua_pushstring(lua, "width");
    lua_pushinteger(lua, w);
    lua_settable(lua, -3);
    lua_pushstring(lua, "height");
    lua_pushinteger(lua, h);
    lua_settable(lua, -3);

    // ok
    return 1;
}
