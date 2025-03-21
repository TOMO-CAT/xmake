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
 * @file        poller.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "poller"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "poller.h"

/* *******************************************************
 * macros
 */

// the singleton type of poller
#define XM_IO_POLLER (XU_SINGLETON_TYPE_USER + 4)

/* *******************************************************
 * private implementation
 */
static xu_handle_t xm_io_poller_instance_init(xu_cpointer_t* ppriv)
{
    // init poller
    xu_poller_ref_t poller = xu_poller_init(xu_null);
    xu_assert_and_check_return_val(poller, xu_null);

    return (xu_handle_t)poller;
}
static xu_void_t xm_io_poller_instance_exit(xu_handle_t poller, xu_cpointer_t priv)
{
    if (poller) xu_poller_exit((xu_poller_ref_t)poller);
}

/* *******************************************************
 * implementation
 */
xu_poller_ref_t xm_io_poller()
{
    return (xu_poller_ref_t)xu_singleton_instance(XM_IO_POLLER, xm_io_poller_instance_init, xm_io_poller_instance_exit,
                                                  xu_null, xu_null);
}
