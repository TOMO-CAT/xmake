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
 * @author      OpportunityLiu, ruki
 * @file        file_flush.c
 *
 */

/* *******************************************************
 * trace
 */
#define TB_TRACE_MODULE_NAME "file_flush"
#define TB_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * private implementation
 */
static xu_bool_t xm_io_std_flush_impl(xm_io_file_t* file)
{
    tb_assert_and_check_return_val(xm_io_file_is_std(file), xu_false);
    return (file->u.std_ref != tb_stdfile_input()) ? tb_stdfile_flush(file->u.std_ref) : xu_false;
}

static xu_bool_t xm_io_file_flush_impl(xm_io_file_t* file)
{
    // check
    tb_assert_and_check_return_val(xm_io_file_is_file(file), xu_false);
    return tb_stream_sync(file->u.file_ref, xu_false);
}

/* *******************************************************
 * interfaces
 */

// io.file_flush(file)
xu_int_t xm_io_file_flush(lua_State* lua)
{
    // check
    tb_assert_and_check_return_val(lua, 0);

    // is user data?
    if (!lua_isuserdata(lua, 1)) xm_io_return_error(lua, "flush(invalid file)!");

    // get file
    xm_io_file_t* file = (xm_io_file_t*)lua_touserdata(lua, 1);
    tb_check_return_val(file, 0);

    // flush file
    xu_bool_t ok = xm_io_file_is_file(file) ? xm_io_file_flush_impl(file) : xm_io_std_flush_impl(file);
    if (ok)
    {
        lua_pushboolean(lua, xu_true);
        return 1;
    }
    else
        xm_io_return_error(lua, "failed to flush file");
}
