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
 * @file        stdfile.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "stdfile"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/io/prefix.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* *******************************************************
 * macros
 */

// the singleton type of stdfile
#define XM_IO_STDFILE_STDIN (XU_SINGLETON_TYPE_USER + 1)
#define XM_IO_STDFILE_STDOUT (XU_SINGLETON_TYPE_USER + 2)
#define XM_IO_STDFILE_STDERR (XU_SINGLETON_TYPE_USER + 3)

/* *******************************************************
 * private implementation
 */
static xu_size_t xm_io_stdfile_isatty(xu_size_t type)
{
    xu_bool_t answer = xu_false;

    switch (type)
    {
    case XM_IO_FILE_TYPE_STDIN: answer = isatty(fileno(stdin)); break;
    case XM_IO_FILE_TYPE_STDOUT: answer = isatty(fileno(stdout)); break;
    case XM_IO_FILE_TYPE_STDERR: answer = isatty(fileno(stderr)); break;
    }

    if (answer) type |= XM_IO_FILE_FLAG_TTY;
    return type;
}

// @see https://github.com/xmake-io/xmake/issues/2580
static xu_void_t xm_io_stdfile_init_buffer(xu_size_t type)
{
    struct stat stats;
    xu_int_t    size = BUFSIZ;
    if (fstat(fileno(stdout), &stats) != -1) size = stats.st_blksize;
    setvbuf(stdout, xu_null, _IOLBF, size);
}

static xm_io_file_t* xm_io_stdfile_new(lua_State* lua, xu_size_t type)
{
    // init stdfile
    xu_stdfile_ref_t fp = xu_null;
    switch (type)
    {
    case XM_IO_FILE_TYPE_STDIN: fp = xu_stdfile_input(); break;
    case XM_IO_FILE_TYPE_STDOUT: fp = xu_stdfile_output(); break;
    case XM_IO_FILE_TYPE_STDERR: fp = xu_stdfile_error(); break;
    }

    // new file
    xm_io_file_t* file = (xm_io_file_t*)lua_newuserdata(lua, sizeof(xm_io_file_t));
    xu_assert_and_check_return_val(file, xu_null);

    // init file
    file->u.std_ref = fp;
    file->stream    = xu_null;
    file->fstream   = xu_null;
    file->type      = xm_io_stdfile_isatty(type);
    file->encoding  = XU_CHARSET_TYPE_UTF8;

    // init stdio buffer
    xm_io_stdfile_init_buffer(type);

    // init the read/write line cache buffer
    xu_buffer_init(&file->rcache);
    xu_buffer_init(&file->wcache);
    return file;
}

/* *******************************************************
 * interfaces
 */

// io.stdfile(stdin: 1, stdout: 2, stderr: 3)
xu_int_t xm_io_stdfile(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get std type
    xu_long_t type = (xu_long_t)lua_tointeger(lua, 1);

    /* push a new stdfile
     *
     * @note we need to ensure that it is a singleton in the external lua script, and will only be created once, e.g.
     * io.stdin, io.stdout, io.stderr
     */
    xm_io_file_t* file = xm_io_stdfile_new(lua, type);
    if (file)
        return 1;
    else
        xm_io_return_error(lua, "invalid stdfile type!");
}
