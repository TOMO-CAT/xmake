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
 * @file        rmdir.c
 *
 */

/* *******************************************************
 * trace
 */
#define TB_TRACE_MODULE_NAME "rmdir"
#define TB_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"

/* *******************************************************
 * private implementation
 */
static tb_long_t xm_os_rmdir_empty(xu_char_t const* path, xu_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    xu_bool_t* is_emptydir = (xu_bool_t*)priv;
    xu_assert_and_check_return_val(path && info && is_emptydir, TB_DIRECTORY_WALK_CODE_END);

    // is emptydir?
    if (info->type == TB_FILE_TYPE_DIRECTORY || info->type == TB_FILE_TYPE_FILE)
    {
        // not emptydir
        *is_emptydir = xu_false;
        return TB_DIRECTORY_WALK_CODE_END;
    }
    return TB_DIRECTORY_WALK_CODE_CONTINUE;
}
static tb_long_t xm_os_rmdir_remove(xu_char_t const* path, xu_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(path, TB_DIRECTORY_WALK_CODE_END);

    // is directory?
    if (info->type == TB_FILE_TYPE_DIRECTORY)
    {
        // is emptydir?
        xu_bool_t is_emptydir = xu_true;
        xu_directory_walk(path, xu_false, xu_true, xm_os_rmdir_empty, &is_emptydir);

        // trace
        tb_trace_d("path: %s, emptydir: %u", path, is_emptydir);

        // remove empty directory
        if (is_emptydir) tb_directory_remove(path);
    }
    return TB_DIRECTORY_WALK_CODE_CONTINUE;
}

/* *******************************************************
 * implementation
 */
xu_int_t xm_os_rmdir(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the path
    xu_char_t const* path = luaL_checkstring(lua, 1);
    tb_check_return_val(path, 0);

    // only remove empty directory?
    xu_bool_t rmempty = lua_toboolean(lua, 2);
    if (rmempty)
    {
        // remove all empty directories
        xu_directory_walk(path, xu_true, xu_false, xm_os_rmdir_remove, xu_null);

        // remove empty root directory
        xu_bool_t is_emptydir = xu_true;
        xu_directory_walk(path, xu_false, xu_true, xm_os_rmdir_empty, &is_emptydir);
        if (is_emptydir) tb_directory_remove(path);

        // trace
        tb_trace_d("path: %s, emptydir: %u", path, is_emptydir);

        // ok?
        lua_pushboolean(lua, !xu_file_info(path, xu_null));
    }
    else
    {
        // os.rmdir(path)
        lua_pushboolean(lua, tb_directory_remove(path));
    }
    return 1;
}
