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
 * @file        sha.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "sha"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/hash/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_hash_sha(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get mode
    xu_size_t mode = (xu_size_t)lua_tointeger(lua, 1);

    // is bytes? get data and size
    if (xm_lua_isinteger(lua, 2) && xm_lua_isinteger(lua, 3))
    {
        xu_byte_t const* data = (xu_byte_t const*)(xu_size_t)(xu_long_t)lua_tointeger(lua, 2);
        xu_size_t        size = (xu_size_t)lua_tointeger(lua, 3);
        if (!data || !size)
        {
            lua_pushnil(lua);
            lua_pushfstring(lua, "invalid data(%p) and size(%d)!", data, (xu_int_t)size);
            return 2;
        }
        xu_assert_static(sizeof(lua_Integer) >= sizeof(xu_pointer_t));

        // compute sha
        xu_sha_t  sha;
        xu_byte_t buffer[32];
        xu_sha_init(&sha, mode);
        xu_sha_spak(&sha, data, size);
        xu_sha_exit(&sha, buffer, sizeof(buffer));

        // make sha string
        xu_size_t i      = 0;
        xu_size_t n      = sha.digest_len << 2;
        xu_char_t s[256] = {0};
        for (i = 0; i < n; ++i)
            xu_snprintf(s + (i << 1), 3, "%02x", buffer[i]);

        // save result
        lua_pushstring(lua, s);
        return 1;
    }

    // get the filename
    xu_char_t const* filename = luaL_checkstring(lua, 2);
    xu_check_return_val(filename, 0);

    // load data from file
    xu_bool_t       ok     = xu_false;
    tb_stream_ref_t stream = tb_stream_init_from_file(filename, TB_FILE_MODE_RO);
    if (stream)
    {
        // open stream
        if (tb_stream_open(stream))
        {
            // init sha
            xu_sha_t sha;
            xu_sha_init(&sha, mode);

            // read data and update sha
            xu_byte_t data[TB_STREAM_BLOCK_MAXN];
            while (!tb_stream_beof(stream))
            {
                // read data
                xu_long_t real = tb_stream_read(stream, data, sizeof(data));

                // ok?
                if (real > 0) xu_sha_spak(&sha, data, real);
                // no data? continue it
                else if (!real)
                {
                    // wait
                    real = tb_stream_wait(stream, TB_STREAM_WAIT_READ, tb_stream_timeout(stream));
                    xu_check_break(real > 0);

                    // has read?
                    xu_assert_and_check_break(real & TB_STREAM_WAIT_READ);
                }
                // failed or end?
                else
                    break;
            }

            // exit sha
            xu_byte_t buffer[32];
            xu_sha_exit(&sha, buffer, sizeof(buffer));

            // make sha string
            xu_size_t i      = 0;
            xu_size_t n      = sha.digest_len << 2;
            xu_char_t s[256] = {0};
            for (i = 0; i < n; ++i)
                xu_snprintf(s + (i << 1), 3, "%02x", buffer[i]);

            // save result
            lua_pushstring(lua, s);

            // ok
            ok = xu_true;
        }

        // exit stream
        tb_stream_exit(stream);
    }
    if (!ok) lua_pushnil(lua);
    return 1;
}
