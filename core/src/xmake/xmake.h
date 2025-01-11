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
 * @file        xmake.h
 *
 */
#ifndef XM_XMAKE_H
#define XM_XMAKE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "xmake/prefix.h"
#include "xmake/engine.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef __xm_debug__
#   define __xm_mode_debug__    TB_MODE_DEBUG
#else
#   define __xm_mode_debug__    (0)
#endif

#ifdef __xm_small__
#   define __xm_mode_small__    TB_MODE_SMALL
#else
#   define __xm_mode_small__    (0)
#endif

/*! init xmake
 *
 * @return          xm_true or xm_false
 *
 * @code
    #include "xmake/xmake.h"

    int main(int argc, char** argv)
    {
        // init xmake
        if (!xm_init()) return 0;


        // exit xmake
        xm_exit();
        return 0;
    }
 * @endcode
 */
#define xm_init()     xm_init_((xm_size_t)(__xm_mode_debug__ | __xm_mode_small__), XM_VERSION_BUILD)

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the xmake library
 *
 * @param mode      the compile mode for check __tb_small__ and __tb_debug__
 * @param build     the build version
 *
 * @return          xm_true or xm_false
 */
xm_bool_t           xm_init_(xm_size_t mode, tb_hize_t build);

/// exit the xmake library
xm_void_t           xm_exit(xm_noarg_t);

/*! the xmake version
 *
 * @return          the xmake version
 */
tb_version_t const* xm_version(xm_noarg_t);

#ifdef __cplusplus
}
#endif

#endif
