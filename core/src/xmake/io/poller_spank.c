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
 * @file        poller_spank.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "poller_spank"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "poller.h"
#include "xmake/io/prefix.h"

/* *******************************************************
 * interfaces
 */

// io.poller_spank()
xu_int_t xm_io_poller_spank(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // spank the poller, break the xu_poller_wait() and return all events
    xu_poller_spak(xm_io_poller());
    return 0;
}
