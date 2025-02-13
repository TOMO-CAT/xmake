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
 * @file        xxhash.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "xxhash"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/hash/prefix.h"
#define XXH_NAMESPACE XM_
#define XXH_STATIC_LINKING_ONLY
#define XXH_IMPLEMENTATION
#include "xxhash/xxhash.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_hash_xxhash(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get mode
    xu_size_t mode = (xu_size_t)lua_tointeger(lua, 1);
    if (mode != 64 && mode != 128)
    {
        lua_pushnil(lua);
        lua_pushfstring(lua, "invalid mode(%d))!", (xu_int_t)mode);
        return 2;
    }

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

        // compute hash
        xu_byte_t const* buffer;
        XXH64_hash_t     value64;
        XXH128_hash_t    value128;
        if (mode == 64)
        {
            value64 = XM_XXH3_64bits(data, size);
            buffer  = (xu_byte_t const*)&value64;
        }
        else if (mode == 128)
        {
            value128 = XM_XXH3_128bits(data, size);
            buffer   = (xu_byte_t const*)&value128;
        }

        // make xxhash string
        xu_size_t i      = 0;
        xu_size_t n      = mode >> 3;
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
    xu_stream_ref_t stream = xu_stream_init_from_file(filename, XU_FILE_MODE_RO);
    if (stream)
    {
        // open stream
        XXH3_state_t* state = XM_XXH3_createState();
        if (xu_stream_open(stream) && state)
        {
            // reset xxhash
            if (mode == 64)
                XM_XXH3_64bits_reset(state);
            else
                XM_XXH3_128bits_reset(state);

            // read data and update xxhash
            xu_byte_t data[XU_STREAM_BLOCK_MAXN];
            while (!xu_stream_beof(stream))
            {
                // read data
                xu_long_t real = xu_stream_read(stream, data, sizeof(data));

                // ok?
                if (real > 0)
                {
                    if (mode == 64)
                        XM_XXH3_64bits_update(state, data, real);
                    else
                        XM_XXH3_128bits_update(state, data, real);
                }
                // no data? continue it
                else if (!real)
                {
                    // wait
                    real = xu_stream_wait(stream, XU_STREAM_WAIT_READ, xu_stream_timeout(stream));
                    xu_check_break(real > 0);

                    // has read?
                    xu_assert_and_check_break(real & XU_STREAM_WAIT_READ);
                }
                // failed or end?
                else
                    break;
            }

            // compute hash
            xu_byte_t const* buffer;
            XXH64_hash_t     value64;
            XXH128_hash_t    value128;
            if (mode == 64)
            {
                value64 = XM_XXH3_64bits_digest(state);
                buffer  = (xu_byte_t const*)&value64;
            }
            else
            {
                value128 = XM_XXH3_128bits_digest(state);
                buffer   = (xu_byte_t const*)&value128;
            }

            // make xxhash string
            xu_size_t i      = 0;
            xu_size_t n      = mode >> 3;
            xu_char_t s[256] = {0};
            for (i = 0; i < n; ++i)
                xu_snprintf(s + (i << 1), 3, "%02x", buffer[i]);

            // save result
            lua_pushstring(lua, s);
            ok = xu_true;
        }

        // exit stream
        xu_stream_exit(stream);

        // exit xxhash
        if (state) XM_XXH3_freeState(state);
    }
    if (!ok) lua_pushnil(lua);
    return 1;
}
