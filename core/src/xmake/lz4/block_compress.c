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
 * @file        block_compress.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "block_compress"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/lz4/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_lz4_block_compress(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get data and size
    xu_int_t         size = 0;
    xu_byte_t const* data = xu_null;
    if (xm_lua_isinteger(lua, 1)) data = (xu_byte_t const*)(xu_size_t)(xu_long_t)lua_tointeger(lua, 1);
    if (xm_lua_isinteger(lua, 2)) size = (xu_int_t)lua_tointeger(lua, 2);
    if (!data || !size || size > LZ4_MAX_INPUT_SIZE)
    {
        lua_pushnil(lua);
        lua_pushfstring(lua, "invalid data(%p) and size(%d)!", data, (xu_int_t)size);
        return 2;
    }
    xu_assert_static(sizeof(lua_Integer) >= sizeof(xu_pointer_t));

    // do compress
    xu_bool_t  ok          = xu_false;
    xu_byte_t* output_data = xu_null;
    xu_byte_t  buffer[8192];
    do
    {
        xu_int_t output_size = LZ4_compressBound(size);
        xu_assert_and_check_break(output_size);

        output_data = output_size <= sizeof(buffer) ? buffer : (xu_byte_t*)xu_malloc(output_size);
        xu_assert_and_check_break(output_data);

        xu_int_t real = LZ4_compress_default((xu_char_t const*)data, (xu_char_t*)output_data, size, output_size);
        xu_assert_and_check_break(real > 0);

        lua_pushlstring(lua, (xu_char_t const*)output_data, real);
        ok = xu_true;
    } while (0);

    if (output_data && output_data != buffer)
    {
        xu_free(output_data);
        output_data = xu_null;
    }

    if (!ok) lua_pushnil(lua);
    return 1;
}
