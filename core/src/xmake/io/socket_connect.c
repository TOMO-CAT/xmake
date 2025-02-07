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
 * @file        socket_connect.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "socket_connect"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * interfaces
 */

// io.socket_connect(sock, addr, port, family)
xu_int_t xm_io_socket_connect(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // check socket
    if (!xm_lua_ispointer(lua, 1))
    {
        lua_pushnumber(lua, -1);
        lua_pushliteral(lua, "invalid socket!");
        return 2;
    }

    // get socket
    xu_socket_ref_t sock = (xu_socket_ref_t)xm_lua_topointer(lua, 1);
    xu_check_return_val(sock, 0);

    // get address
    xu_char_t const* address = lua_tostring(lua, 2);
    xu_assert_and_check_return_val(address, 0);

    // get family
    xu_uint8_t family = (xu_uint8_t)luaL_checknumber(lua, 4);

    // init address
    xu_ipaddr_t addr;
    if (family == XU_IPADDR_FAMILY_UNIX)
    {
        xu_bool_t is_abstract = (xu_bool_t)lua_toboolean(lua, 3);
        xu_ipaddr_unix_set_cstr(&addr, address, is_abstract);
    }
    else
    {
        xu_uint16_t port = (xu_uint16_t)luaL_checknumber(lua, 3);
        xu_ipaddr_set(&addr, address, port, family);
    }

    // connect socket
    lua_pushnumber(lua, (xu_int_t)xu_socket_connect(sock, &addr));
    return 1;
}
