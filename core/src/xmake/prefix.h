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
 * @file        prefix.h
 *
 */

#pragma once

// for xmake/scripts/module/xmi.h
#define XM_PREFIX_H

/* *******************************************************
 * includes
 */
#include "luaconf.h"
#include "xmake/prefix/config.h"
#include "xmake/prefix/version.h"

#ifdef USE_LUAJIT
#    include "lauxlib.h"
#    include "luajit.h"
#    include "lualib.h"
#else
#    include "lauxlib.h"
#    include "lua.h"
#    include "lualib.h"
#endif

/* *******************************************************
 * private interfaces
 */

// this issue has been fixed, @see https://github.com/LuaJIT/LuaJIT/commit/e9af1abec542e6f9851ff2368e7f196b6382a44c
#if 0 // XU_CPU_BIT64
/* we use this interface instead of lua_pushlightuserdata() to fix bad light userdata pointer bug
 *
 * @see https://github.com/xmake-io/xmake/issues/914
 * https://github.com/LuaJIT/LuaJIT/pull/230
 *
 * @note we cannot lua_newuserdata() because we need pass this pointer to the external lua code
 * in poller_wait()/event_callback, but lua_pushuserdata does not exists
 */
static __xu_inline__ xu_void_t xm_lua_pushpointer(lua_State* lua, xu_pointer_t ptr)
{
    xu_uint64_t ptrval = (xu_uint64_t)ptr;
    if ((ptrval >> 47) == 0)
        lua_pushlightuserdata(lua, ptr);
    else
    {
        xu_char_t str[64];
        xu_long_t len = xu_snprintf(str, sizeof(str), "%p", ptr);
        lua_pushlstring(lua, str, len);
    }
}
static __xu_inline__ xu_bool_t xm_lua_ispointer(lua_State* lua, xu_int_t idx)
{
    return lua_isuserdata(lua, idx) || lua_isstring(lua, idx);
}
static __xu_inline__ xu_pointer_t xm_lua_topointer2(lua_State* lua, xu_int_t idx, xu_char_t const** pstr)
{
    xu_pointer_t ptr = xu_null;
    if (lua_isuserdata(lua, idx))
    {
        ptr = lua_touserdata(lua, idx);
        if (pstr) *pstr = xu_null;
    }
    else
    {
        size_t len = 0;
        xu_char_t const* str = luaL_checklstring(lua, idx, &len);
        if (str && len > 2 && str[0] == '0' && str[1] == 'x')
            ptr = (xu_pointer_t)xu_s16tou64(str);
        if (pstr) *pstr = str;
    }
    return ptr;
}
static __xu_inline__ xu_pointer_t xm_lua_topointer(lua_State* lua, xu_int_t idx)
{
    return xm_lua_topointer2(lua, idx, xu_null);
}
#else
static __xu_inline__ xu_void_t xm_lua_pushpointer(lua_State* lua, xu_pointer_t ptr) { lua_pushlightuserdata(lua, ptr); }
static __xu_inline__ xu_bool_t xm_lua_ispointer(lua_State* lua, xu_int_t idx) { return lua_isuserdata(lua, idx); }
static __xu_inline__ xu_pointer_t xm_lua_topointer2(lua_State* lua, xu_int_t idx, xu_char_t const** pstr)
{
    if (pstr) *pstr = xu_null;
    return lua_touserdata(lua, idx);
}
static __xu_inline__ xu_pointer_t xm_lua_topointer(lua_State* lua, xu_int_t idx) { return lua_touserdata(lua, idx); }
#endif

static __xu_inline__ xu_void_t xm_lua_register(lua_State* lua, xu_char_t const* libname, luaL_Reg const* l)
{
#if LUA_VERSION_NUM >= 504
    lua_getglobal(lua, libname);
    if (lua_isnil(lua, -1))
    {
        lua_pop(lua, 1);
        lua_newtable(lua);
    }
    luaL_setfuncs(lua, l, 0);
    lua_setglobal(lua, libname);
#else
    luaL_register(lua, libname, l);
#endif
}

static __xu_inline__ xu_int_t xm_lua_isinteger(lua_State* lua, int idx)
{
#ifdef USE_LUAJIT
    return lua_isnumber(lua, idx);
#else
    return lua_isinteger(lua, idx);
#endif
}
