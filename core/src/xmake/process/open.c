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
 * @file        open.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "process.open"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "../io/prefix.h"
#include "xmake/process/prefix.h"

/* *******************************************************
 * implementation
 */

/* p = process.open(command,
 * {outpath = "", errpath = "", outfile = "",
 *  errfile = "", outpipe = "", errpipe = "",
 *  infile = "", inpipe = "", inpipe = "",
 *  envs = {"PATH=xxx", "XXX=yyy"}})
 */
xu_int_t xm_process_open(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get command
    size_t           command_size = 0;
    xu_char_t const* command      = luaL_checklstring(lua, 1, &command_size);
    xu_check_return_val(command, 0);

    // init attributes
    xu_process_attr_t attr = {0};

    // get option arguments
    xu_size_t          envn       = 0;
    xu_char_t const*   envs[1024] = {0};
    xu_char_t const*   inpath     = xu_null;
    xu_char_t const*   outpath    = xu_null;
    xu_char_t const*   errpath    = xu_null;
    xm_io_file_t*      infile     = xu_null;
    xm_io_file_t*      outfile    = xu_null;
    xm_io_file_t*      errfile    = xu_null;
    xu_pipe_file_ref_t inpipe     = xu_null;
    xu_pipe_file_ref_t outpipe    = xu_null;
    xu_pipe_file_ref_t errpipe    = xu_null;
    if (lua_istable(lua, 2))
    {
        // is detached?
        lua_pushstring(lua, "detach");
        lua_gettable(lua, 2);
        if (lua_toboolean(lua, -1)) attr.flags |= XU_PROCESS_FLAG_DETACH;
        lua_pop(lua, 1);

        // get curdir
        lua_pushstring(lua, "curdir");
        lua_gettable(lua, 3);
        attr.curdir = lua_tostring(lua, -1);
        lua_pop(lua, 1);

        // get inpath
        lua_pushstring(lua, "inpath");
        lua_gettable(lua, 2);
        inpath = lua_tostring(lua, -1);
        lua_pop(lua, 1);

        // get outpath
        lua_pushstring(lua, "outpath");
        lua_gettable(lua, 2);
        outpath = lua_tostring(lua, -1);
        lua_pop(lua, 1);

        // get errpath
        lua_pushstring(lua, "errpath");
        lua_gettable(lua, 2);
        errpath = lua_tostring(lua, -1);
        lua_pop(lua, 1);

        // get infile
        if (!inpath)
        {
            lua_pushstring(lua, "infile");
            lua_gettable(lua, 3);
            infile = (xm_io_file_t*)lua_touserdata(lua, -1);
            lua_pop(lua, 1);
        }

        // get outfile
        if (!outpath)
        {
            lua_pushstring(lua, "outfile");
            lua_gettable(lua, 3);
            outfile = (xm_io_file_t*)lua_touserdata(lua, -1);
            lua_pop(lua, 1);
        }

        // get errfile
        if (!errpath)
        {
            lua_pushstring(lua, "errfile");
            lua_gettable(lua, 3);
            errfile = (xm_io_file_t*)lua_touserdata(lua, -1);
            lua_pop(lua, 1);
        }

        // get inpipe
        if (!inpath && !infile)
        {
            lua_pushstring(lua, "inpipe");
            lua_gettable(lua, 3);
            inpipe = (xu_pipe_file_ref_t)lua_touserdata(lua, -1);
            lua_pop(lua, 1);
        }

        // get outpipe
        if (!outpath && !outfile)
        {
            lua_pushstring(lua, "outpipe");
            lua_gettable(lua, 3);
            outpipe = (xu_pipe_file_ref_t)lua_touserdata(lua, -1);
            lua_pop(lua, 1);
        }

        // get errpipe
        if (!errpath && !errfile)
        {
            lua_pushstring(lua, "errpipe");
            lua_gettable(lua, 3);
            errpipe = (xu_pipe_file_ref_t)lua_touserdata(lua, -1);
            lua_pop(lua, 1);
        }

        // get environments
        lua_pushstring(lua, "envs");
        lua_gettable(lua, 2);
        if (lua_istable(lua, -1))
        {
            // get environment variables count
            xu_size_t count = (xu_size_t)lua_objlen(lua, -1);

            // get all passed environment variables
            xu_size_t i;
            for (i = 0; i < count; i++)
            {
                // get envs[i]
                lua_pushinteger(lua, i + 1);
                lua_gettable(lua, -2);

                // is string?
                if (lua_isstring(lua, -1))
                {
                    // add this environment value
                    if (envn + 1 < xu_arrayn(envs))
                        envs[envn++] = lua_tostring(lua, -1);
                    else
                    {
                        // error
                        lua_pushfstring(lua, "envs is too large(%d > %d) for process.openv", (xu_int_t)envn,
                                        xu_arrayn(envs) - 1);
                        lua_error(lua);
                    }
                }
                else
                {
                    // error
                    lua_pushfstring(lua, "invalid envs[%d] type(%s) for process.openv", (xu_int_t)i,
                                    luaL_typename(lua, -1));
                    lua_error(lua);
                }

                // pop it
                lua_pop(lua, 1);
            }
        }
        lua_pop(lua, 1);
    }

    // redirect stdin?
    if (inpath)
    {
        // redirect stdin to file
        attr.in.path = inpath;
        attr.inmode  = XU_FILE_MODE_RO;
        attr.intype  = XU_PROCESS_REDIRECT_TYPE_FILEPATH;
    }
    else if (infile && xm_io_file_is_file(infile))
    {
        xu_file_ref_t rawfile = xu_null;
        if (xu_stream_ctrl(infile->stream, XU_STREAM_CTRL_FILE_GET_FILE, &rawfile) && rawfile)
        {
            attr.in.file = rawfile;
            attr.intype  = XU_PROCESS_REDIRECT_TYPE_FILE;
        }
    }
    else if (inpipe)
    {
        attr.in.pipe = inpipe;
        attr.intype  = XU_PROCESS_REDIRECT_TYPE_PIPE;
    }

    // redirect stdout?
    if (outpath)
    {
        // redirect stdout to file
        attr.out.path = outpath;
        attr.outmode  = XU_FILE_MODE_RW | XU_FILE_MODE_TRUNC | XU_FILE_MODE_CREAT;
        attr.outtype  = XU_PROCESS_REDIRECT_TYPE_FILEPATH;
    }
    else if (outfile && xm_io_file_is_file(outfile))
    {
        xu_file_ref_t rawfile = xu_null;
        if (xu_stream_ctrl(outfile->stream, XU_STREAM_CTRL_FILE_GET_FILE, &rawfile) && rawfile)
        {
            attr.out.file = rawfile;
            attr.outtype  = XU_PROCESS_REDIRECT_TYPE_FILE;
        }
    }
    else if (outpipe)
    {
        attr.out.pipe = outpipe;
        attr.outtype  = XU_PROCESS_REDIRECT_TYPE_PIPE;
    }

    // redirect stderr?
    if (errpath)
    {
        // redirect stderr to file
        attr.err.path = errpath;
        attr.errmode  = XU_FILE_MODE_RW | XU_FILE_MODE_TRUNC | XU_FILE_MODE_CREAT;
        attr.errtype  = XU_PROCESS_REDIRECT_TYPE_FILEPATH;
    }
    else if (errfile && xm_io_file_is_file(errfile))
    {
        xu_file_ref_t rawfile = xu_null;
        if (xu_stream_ctrl(errfile->stream, XU_STREAM_CTRL_FILE_GET_FILE, &rawfile) && rawfile)
        {
            attr.err.file = rawfile;
            attr.errtype  = XU_PROCESS_REDIRECT_TYPE_FILE;
        }
    }
    else if (errpipe)
    {
        attr.err.pipe = errpipe;
        attr.errtype  = XU_PROCESS_REDIRECT_TYPE_PIPE;
    }

    // set the new environments
    if (envn > 0) attr.envp = envs;

    // init process
    xu_process_ref_t process = (xu_process_ref_t)xu_process_init_cmd(command, &attr);
    if (process)
        xm_lua_pushpointer(lua, (xu_pointer_t)process);
    else
        lua_pushnil(lua);
    return 1;
}
