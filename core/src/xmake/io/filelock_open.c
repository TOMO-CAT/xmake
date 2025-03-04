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
 * @file        filelock_open.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "filelock_open"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * implementation
 */

/*
 * io.filelock_open(path)
 */
xu_int_t xm_io_filelock_open(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get file path
    xu_char_t const* path = luaL_checkstring(lua, 1);
    xu_assert_and_check_return_val(path, 0);

    // init file lock
    xu_long_t         tryn = 2;
    xu_filelock_ref_t lock = xu_null;
    while (!lock && tryn-- > 0)
        lock = xu_filelock_init_from_path(path, xu_file_info(path, xu_null) ? XU_FILE_MODE_RW
                                                                            : XU_FILE_MODE_RW | XU_FILE_MODE_CREAT);
    if (lock)
        xm_lua_pushpointer(lua, (xu_pointer_t)lock);
    else
        lua_pushnil(lua);
    return 1;
}
