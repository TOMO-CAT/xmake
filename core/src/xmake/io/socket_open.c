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
 * @file        socket_open.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "socket_open"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * implementation
 */

/*
 * io.socket_open(socktype, family)
 */
xu_int_t xm_io_socket_open(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get socket type
    xu_size_t socktype = (xu_size_t)luaL_checknumber(lua, 1);

    // get address family
    xu_size_t family = (xu_size_t)luaL_checknumber(lua, 2);

    // map socket type
    switch (socktype)
    {
    case 2: socktype = XU_SOCKET_TYPE_UDP; break;
    case 3: socktype = XU_SOCKET_TYPE_ICMP; break;
    default: socktype = XU_SOCKET_TYPE_TCP; break;
    }

    // init socket
    tb_socket_ref_t sock = tb_socket_init(socktype, family);
    if (sock)
        xm_lua_pushpointer(lua, (xu_pointer_t)sock);
    else
        lua_pushnil(lua);
    return 1;
}
