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
 * @file        config.h
 *
 */
#ifndef XM_PREFIX_CONFIG_H
#define XM_PREFIX_CONFIG_H

/* *******************************************************
 * includes
 */
#include "tbox/tbox.h"
#include "xmake/config.h"
#include "xutil/xutil.h"

/* *******************************************************
 * macros
 */

/*! @def __xm_small__
 *
 * small mode
 */
#if XM_CONFIG_SMALL
#    define __xm_small__
#endif

/*! @def __xm_debug__
 *
 * debug mode
 */
#ifdef __xu_debug__
#    define __xm_debug__
#endif

#endif
