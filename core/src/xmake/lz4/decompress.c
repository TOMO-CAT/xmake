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
 * @file        decompress.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "decompress"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/lz4/prefix.h"

/* *******************************************************
 * implementation
 */
xu_int_t xm_lz4_decompress(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get data and size
    xu_size_t        size = 0;
    xu_byte_t const* data = xu_null;
    if (xm_lua_isinteger(lua, 1)) data = (xu_byte_t const*)(xu_size_t)(xu_long_t)lua_tointeger(lua, 1);
    if (xm_lua_isinteger(lua, 2)) size = (xu_size_t)lua_tointeger(lua, 2);
    if (!data || !size)
    {
        lua_pushnil(lua);
        lua_pushfstring(lua, "invalid data(%p) and size(%d)!", data, (xu_int_t)size);
        return 2;
    }
    xu_assert_static(sizeof(lua_Integer) >= sizeof(xu_pointer_t));

    // do decompress
    xu_bool_t                   ok = xu_false;
    LZ4F_errorCode_t            code;
    LZ4F_decompressionContext_t ctx = xu_null;
    xu_buffer_t                 result;
    do
    {
        xu_buffer_init(&result);

        code = LZ4F_createDecompressionContext(&ctx, LZ4F_VERSION);
        if (LZ4F_isError(code)) break;

        xu_byte_t buffer[8192];
        xu_bool_t failed = xu_false;
        while (1)
        {
            size_t advance     = (size_t)size;
            size_t buffer_size = sizeof(buffer);
            code               = LZ4F_decompress(ctx, buffer, &buffer_size, data, &advance, xu_null);
            if (LZ4F_isError(code))
            {
                failed = xu_true;
                break;
            }

            if (buffer_size == 0) break;
            data += advance;
            size -= advance;

            xu_buffer_memncat(&result, buffer, buffer_size);
        }
        xu_assert_and_check_break(!failed && xu_buffer_size(&result));

        lua_pushlstring(lua, (xu_char_t const*)xu_buffer_data(&result), xu_buffer_size(&result));
        ok = xu_true;
    } while (0);

    if (ctx)
    {
        LZ4F_freeDecompressionContext(ctx);
        ctx = xu_null;
    }
    xu_buffer_exit(&result);

    if (!ok)
    {
        xu_char_t const* error = LZ4F_getErrorName(code);
        lua_pushnil(lua);
        lua_pushstring(lua, error ? error : "unknown");
        return 2;
    }
    return 1;
}
