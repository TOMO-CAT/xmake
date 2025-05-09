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
 * @file        syserror.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "syserror"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_os_syserror(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get syserror state
    xu_int_t  err      = 0;
    xu_size_t syserror = xu_syserror_state();
    switch (syserror)
    {
    case XU_STATE_SYSERROR_NOT_PERM: err = 1; break;
    case XU_STATE_SYSERROR_NOT_FILEDIR: err = 2; break;
#if ((XU_VERSION_MAJOR * 100) + (XU_VERSION_MINOR * 10) + XU_VERSION_ALTER) >= 173
    case XU_STATE_SYSERROR_NOT_ACCESS: err = 3; break;
#endif
    case XU_STATE_SYSERROR_UNKNOWN_ERROR: err = -1; break;
    }
    lua_pushinteger(lua, err);
    return 1;
}
