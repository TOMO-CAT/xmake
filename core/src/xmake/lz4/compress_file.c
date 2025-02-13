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
 * @file        compress_file.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "compress_file"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/lz4/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_lz4_compress_file(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the file paths
    xu_char_t const* srcpath = luaL_checkstring(lua, 1);
    xu_char_t const* dstpath = luaL_checkstring(lua, 2);
    xu_check_return_val(srcpath && dstpath, 0);

    // init lz4 stream
    xm_lz4_cstream_t* stream_lz4 = xm_lz4_cstream_init();
    xu_check_return_val(stream_lz4, 0);

    // do compress
    xu_bool_t       ok      = xu_false;
    xu_stream_ref_t istream = xu_stream_init_from_file(srcpath, XU_FILE_MODE_RO);
    xu_stream_ref_t ostream =
        xu_stream_init_from_file(dstpath, XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);
    if (istream && ostream && xu_stream_open(istream) && xu_stream_open(ostream))
    {
        xu_bool_t write_ok = xu_false;
        xu_byte_t idata[XU_STREAM_BLOCK_MAXN];
        xu_byte_t odata[XU_STREAM_BLOCK_MAXN];
        while (!xu_stream_beof(istream))
        {
            write_ok        = xu_false;
            xu_long_t ireal = (xu_long_t)xu_stream_read(istream, idata, sizeof(idata));
            if (ireal > 0)
            {
                xu_long_t r = xm_lz4_cstream_write(stream_lz4, idata, ireal, xu_stream_beof(istream));
                xu_assert_and_check_break(r >= 0);
                xu_check_continue(r > 0);

                xu_long_t oreal;
                while ((oreal = xm_lz4_cstream_read(stream_lz4, odata, sizeof(odata))) > 0)
                {
                    if (!xu_stream_bwrit(ostream, odata, oreal))
                    {
                        oreal = -1;
                        break;
                    }
                }
                xu_assert_and_check_break(oreal >= 0);
            }
            else
                break;
            write_ok = xu_true;
        }

        if (xu_stream_beof(istream) && write_ok) ok = xu_true;
    }

    // exit stream
    if (istream)
    {
        xu_stream_exit(istream);
        istream = xu_null;
    }
    if (ostream)
    {
        xu_stream_exit(ostream);
        ostream = xu_null;
    }
    xm_lz4_cstream_exit(stream_lz4);

    // ok?
    lua_pushboolean(lua, ok);
    return 1;
}
