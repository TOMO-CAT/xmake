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
 * @file        xmake.c
 *
 */

/* *******************************************************
 * includes
 */
#include "xmake/xmake.h"

/* *******************************************************
 * private implementation
 */
static __xu_inline__ xu_bool_t xm_check_mode(xu_size_t mode)
{
#ifdef __xm_debug__
    if (!(mode & XU_MODE_DEBUG))
    {
        xu_trace_e("libxmake.a has __xm_debug__ but xmake/xmake.h not");
        return xu_false;
    }
#else
    if (mode & XU_MODE_DEBUG)
    {
        xu_trace_e("xmake/xmake.h has __xm_debug__ but libxmake.a not");
        return xu_false;
    }
#endif

#ifdef __xm_small__
    if (!(mode & XU_MODE_SMALL))
    {
        xu_trace_e("libxmake.a has __xm_small__ but xmake/xmake.h not");
        return xu_false;
    }
#else
    if (mode & XU_MODE_SMALL)
    {
        xu_trace_e("xmake/xmake.h has __xm_small__ but libxmake.a not");
        return xu_false;
    }
#endif

    // ok
    return xu_true;
}
static __xu_inline__ xu_bool_t xm_version_check(xu_hize_t build)
{
    // the version oly for link the static vtag string
    xu_version_t const* version = xm_version();
    xu_used(version);

    // ok
    if ((build / 100) == (XM_VERSION_BUILD / 100))
    {
        xu_trace_d("version: %s", XM_VERSION_STRING);
        return xu_true;
    }
    else
    {
        xu_trace_w("version: %s != %llu", XM_VERSION_STRING, build);
    }

    // no
    return xu_false;
}

/* *******************************************************
 * implementation
 */
xu_bool_t xm_init_(xu_size_t mode, xu_hize_t build)
{
    // trace
    xu_trace_d("init: ..");

    // check mode
    if (!xm_check_mode(mode)) return xu_false;

    // check version
    xm_version_check(build);

#if 0
    // init tbox, we always use the tbox's default allocator
    if (!xutil_init(xu_null, xu_default_allocator(xu_null, 0))) return xu_false;
#else
    // init tbox, since small compilation mode is enabled, it still uses the native allocator
    if (!xutil_init(xu_null, xu_null))
    {
        return xu_false;
    }

#endif

    // trace
    xu_trace_d("init: ok");

    // ok
    return xu_true;
}
xu_void_t xm_exit()
{
    // exit xutil
    xutil_exit();
}
xu_version_t const* xm_version()
{
    // init version tag for binary search
    static __xu_volatile__ xu_char_t const* s_vtag = "[xmake]: [vtag]: " XM_VERSION_STRING;
    xu_used(s_vtag);

    // init version
    static xu_version_t s_version = {0};
    if (!s_version.major)
    {
        s_version.major = XM_VERSION_MAJOR;
        s_version.minor = XM_VERSION_MINOR;
        s_version.alter = XM_VERSION_ALTER;
        s_version.build = (xu_hize_t)xu_atoll(XM_VERSION_BUILD_STRING);
    }

    return &s_version;
}
