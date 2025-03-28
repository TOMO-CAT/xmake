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
 * @file        socket_sendto.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "socket_sendto"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * implementation
 */

// io.socket_sendto(sock, data, addr, port)
xu_int_t xm_io_socket_sendto(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // check socket
    if (!xm_lua_ispointer(lua, 1))
    {
        lua_pushinteger(lua, -1);
        lua_pushliteral(lua, "invalid socket!");
        return 2;
    }

    // get socket
    xu_socket_ref_t sock = (xu_socket_ref_t)xm_lua_topointer(lua, 1);
    xu_check_return_val(sock, 0);

    // get data and size
    xu_size_t        size = 0;
    xu_byte_t const* data = xu_null;
    if (xm_lua_isinteger(lua, 2)) data = (xu_byte_t const*)(xu_size_t)(xu_long_t)lua_tointeger(lua, 2);
    if (xm_lua_isinteger(lua, 3)) size = (xu_size_t)lua_tointeger(lua, 3);
    if (!data || !size)
    {
        lua_pushinteger(lua, -1);
        lua_pushfstring(lua, "invalid data(%p) and size(%d)!", data, (xu_int_t)size);
        return 2;
    }
    xu_assert_static(sizeof(lua_Integer) >= sizeof(xu_pointer_t));

    // get address
    xu_char_t const* addr = lua_tostring(lua, 4);
    xu_uint16_t      port = (xu_uint16_t)luaL_checknumber(lua, 5);
    if (!addr || !port)
    {
        lua_pushinteger(lua, -1);
        lua_pushliteral(lua, "invalid address!");
        return 2;
    }

    // get address family
    xu_size_t family = (xu_size_t)luaL_checknumber(lua, 6);

    // init ip address
    xu_ipaddr_t ipaddr;
    xu_ipaddr_set(&ipaddr, addr, port, (xu_uint8_t)family);

    // send data
    xu_long_t real = xu_socket_usend(sock, &ipaddr, data, size);
    lua_pushinteger(lua, (xu_int_t)real);
    return 1;
}
