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
 * @file        file_rawfd.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "file_rawfd"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * macros
 */

// file to fd
#define xm_io_file2fd(file) (lua_Number) xu_file2fd(file)

/* *******************************************************
 * implementation
 */

/* io.file_rawfd(file)
 *
 * @note this interface is very dangerous and is only used in some special/hacking cases.
 *
 * e.g. set VS_UNICODE_OUTPUT=fd to enable vs unicode output, @see https://github.com/xmake-io/xmake/issues/528
 */
xu_int_t xm_io_file_rawfd(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // is user data?
    if (!lua_isuserdata(lua, 1)) xm_io_return_error(lua, "get rawfd for invalid file!");

    // get file
    xm_io_file_t* file = (xm_io_file_t*)lua_touserdata(lua, 1);
    xu_check_return_val(file, 0);

    // get file raw fd
    if (xm_io_file_is_file(file))
    {
        xu_file_ref_t rawfile = xu_null;
        if (xu_stream_ctrl(file->stream, XU_STREAM_CTRL_FILE_GET_FILE, &rawfile))
        {
            lua_pushnumber(lua, xm_io_file2fd(rawfile));
            return 1;
        }
    }

    // get rawfd failed
    xm_io_return_error(lua, "get rawfd for invalid file!");
}
