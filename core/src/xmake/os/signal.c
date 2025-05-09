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
 * @file        signal.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "signal"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"
#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)
#    include <signal.h>
#    include <unistd.h>
#elif defined(XU_CONFIG_OS_LINUX) || defined(XU_CONFIG_OS_BSD) || defined(XU_CONFIG_OS_ANDROID) ||                     \
    defined(XU_CONFIG_OS_HAIKU)
#    include <signal.h>
#    include <unistd.h>
#endif
#ifdef XU_CONFIG_OS_BSD
#    include <signal.h>
#    include <sys/sysctl.h>
#    include <sys/types.h>
#endif

/* *******************************************************
 * types
 */
typedef enum __xm_os_signal_e
{
    XM_OS_SIGINT = 2
} xm_os_signal_e;

typedef enum __xm_os_signal_handler_e
{
    XM_OS_SIGFUN = 0,
    XM_OS_SIGDFL = 1,
    XM_OS_SIGIGN = 2,
} xm_os_signal_handler_e;

/* *******************************************************
 * globals
 */
static lua_State* g_lua = xu_null;

/* *******************************************************
 * private implementation
 */
static xu_void_t xm_os_signal_handler_impl(xu_int_t signo)
{
    // do callback(signo)
    lua_State* lua = g_lua;
    if (lua)
    {
        xu_char_t name[64] = {0};
        xu_snprintf(name, sizeof(name), "_SIGNAL_HANDLER_%d", signo);
        lua_getglobal(lua, name);
        lua_pushinteger(lua, signo);
        lua_call(lua, 1, 0);
    }
}

#if defined(SIGINT)
static xu_void_t xm_os_signal_handler(xu_int_t signo_native)
{
    xu_int_t signo = -1;
    switch (signo_native)
    {
    case SIGINT: signo = XM_OS_SIGINT; break;
    default: break;
    }
    if (signo >= 0) xm_os_signal_handler_impl(signo);
}
#endif

/* *******************************************************
 * implementation
 */
xu_int_t xm_os_signal(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);
    g_lua            = lua;
    xu_int_t handler = XM_OS_SIGFUN;

    // check signal handler
    if (lua_isnumber(lua, 2))
        handler = (xu_int_t)luaL_checkinteger(lua, 2);
    else if (!lua_isfunction(lua, 2))
        return 0;

    // save signal handler
    xu_int_t signo = (xu_int_t)luaL_checkinteger(lua, 1);
    if (handler == XM_OS_SIGFUN)
    {
        xu_char_t name[64] = {0};
        xu_snprintf(name, sizeof(name), "_SIGNAL_HANDLER_%d", signo);
        lua_pushvalue(lua, 2);
        lua_setglobal(lua, name);
    }

#if defined(SIGINT)
    switch (signo)
    {
    case XM_OS_SIGINT: signo = SIGINT; break;
    default: return 0;
    }

    switch (handler)
    {
    case XM_OS_SIGFUN: signal(signo, xm_os_signal_handler); break;
    case XM_OS_SIGDFL: signal(signo, SIG_DFL); break;
    case XM_OS_SIGIGN: signal(signo, SIG_IGN); break;
    default: break;
    }
#endif
    return 0;
}
