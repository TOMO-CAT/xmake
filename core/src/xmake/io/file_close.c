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
 * @file        file_close.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "file_close"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"

/* *******************************************************
 * interfaces
 */

// io.file_close(file)
xu_int_t xm_io_file_close(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // is user data?
    if (!lua_isuserdata(lua, 1)) xm_io_return_error(lua, "close(invalid file)!");

    // get file
    xm_io_file_t* file = (xm_io_file_t*)lua_touserdata(lua, 1);
    tb_check_return_val(file, 0);

    // close file
    if (xm_io_file_is_file(file))
    {
        // check
        tb_assert(file->u.file_ref);

        // flush filter stream cache, TODO we should fix it in tbox/stream
        if ((file->mode & TB_FILE_MODE_RW) == TB_FILE_MODE_RW && file->fstream)
        {
            if (!tb_stream_sync(file->u.file_ref, xu_false)) return xu_false;
        }

        // close file
        tb_stream_clos(file->u.file_ref);
        file->u.file_ref = xu_null;

        // exit fstream
        if (file->fstream) tb_stream_exit(file->fstream);
        file->fstream = xu_null;

        // exit stream
        if (file->stream) tb_stream_exit(file->stream);
        file->stream = xu_null;

        // exit the line cache buffer
        tb_buffer_exit(&file->rcache);
        tb_buffer_exit(&file->wcache);

        // gc will free it if no any refs for lua_newuserdata()
        // ...

        // ok
        lua_pushboolean(lua, xu_true);
        return 1;
    }
    else // for stdfile (gc/close)
    {
        // exit the line cache buffer
        tb_buffer_exit(&file->rcache);
        tb_buffer_exit(&file->wcache);

        // gc will free it if no any refs for lua_newuserdata()
        // ...

        // ok
        lua_pushboolean(lua, xu_true);
        return 1;
    }
}
