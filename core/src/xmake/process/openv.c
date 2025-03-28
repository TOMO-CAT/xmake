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
 * @file        openv.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "process.openv"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "../io/prefix.h"
#include "xmake/process/prefix.h"
#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_LINUX) || defined(XU_CONFIG_OS_BSD) ||                        \
    defined(XU_CONFIG_OS_HAIKU)
#    include <signal.h>
#endif

/* *******************************************************
 * implementation
 */

/* p = process.openv(shellname, argv,
 * {outpath = "", errpath = "", outfile = "",
 *  errfile = "", outpipe = "", errpipe = "",
 *  infile = "", inpipe = "", inpipe = "",
 *  envs = {"PATH=xxx", "XXX=yyy"}})
 */
xu_int_t xm_process_openv(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // check argv
    if (!lua_istable(lua, 2))
    {
        // error
        lua_pushfstring(lua, "invalid argv type(%s) for process.openv", luaL_typename(lua, 2));
        lua_error(lua);
        return 0;
    }

    // get shellname
    xu_char_t const* shellname = lua_tostring(lua, 1);
    xu_check_return_val(shellname, 0);

    // get the arguments count
    xu_long_t argn = (xu_long_t)lua_objlen(lua, 2);
    xu_check_return_val(argn >= 0, 0);

    // get arguments
    xu_size_t         argi = 0;
    xu_char_t const** argv = xu_nalloc0_type(1 + argn + 1, xu_char_t const*);
    xu_check_return_val(argv, 0);

    // fill arguments
    argv[0] = shellname;
    for (argi = 0; argi < argn; argi++)
    {
        // get argv[i]
        lua_pushinteger(lua, argi + 1);
        lua_gettable(lua, 2);

        // is string?
        if (lua_isstring(lua, -1))
        {
            // pass this argument
            argv[1 + argi] = lua_tostring(lua, -1);
        }
        // is path instance?
        else if (lua_istable(lua, -1))
        {
            lua_pushstring(lua, "_STR");
            lua_gettable(lua, -2);
            argv[1 + argi] = lua_tostring(lua, -1);
            lua_pop(lua, 1);
        }
        else
        {
            // error
            lua_pushfstring(lua, "invalid argv[%d] type(%s) for process.openv", (xu_int_t)argi, luaL_typename(lua, -1));
            lua_error(lua);
        }

        // pop it
        lua_pop(lua, 1);
    }

    // init attributes
    xu_process_attr_t attr = {0};

    // get option arguments
    xu_bool_t          exclusive  = xu_false;
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
    if (lua_istable(lua, 3))
    {
        // is detached?
        lua_pushstring(lua, "detach");
        lua_gettable(lua, 3);
        if (lua_toboolean(lua, -1)) attr.flags |= XU_PROCESS_FLAG_DETACH;
        lua_pop(lua, 1);

        // is exclusive?
        lua_pushstring(lua, "exclusive");
        lua_gettable(lua, 3);
        if (lua_toboolean(lua, -1)) exclusive = xu_true;
        lua_pop(lua, 1);

        // get curdir
        lua_pushstring(lua, "curdir");
        lua_gettable(lua, 3);
        attr.curdir = lua_tostring(lua, -1);
        lua_pop(lua, 1);

        // get inpath
        lua_pushstring(lua, "inpath");
        lua_gettable(lua, 3);
        inpath = lua_tostring(lua, -1);
        lua_pop(lua, 1);

        // get outpath
        lua_pushstring(lua, "outpath");
        lua_gettable(lua, 3);
        outpath = lua_tostring(lua, -1);
        lua_pop(lua, 1);

        // get errpath
        lua_pushstring(lua, "errpath");
        lua_gettable(lua, 3);
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
        lua_gettable(lua, 3);
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

        /* we need to ignore SIGINT and SIGQUIT if we enter exclusive mode
         * @see https://github.com/xmake-io/xmake/discussions/2893
         */
#if defined(SIGINT)
    if (exclusive) signal(SIGINT, SIG_IGN);
#endif
#if defined(SIGQUIT)
    if (exclusive) signal(SIGQUIT, SIG_IGN);
#endif

    // init process
    xu_process_ref_t process = (xu_process_ref_t)xu_process_init(shellname, argv, &attr);
    if (process)
        xm_lua_pushpointer(lua, (xu_pointer_t)process);
    else
        lua_pushnil(lua);

    // exit argv
    if (argv) xu_free(argv);
    argv = xu_null;

    // ok
    return 1;
}
