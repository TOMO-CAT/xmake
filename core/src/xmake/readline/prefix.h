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
 * @author      TitanSnow
 * @file        prefix.h
 *
 */
#ifndef XM_READLINE_PREFIX_H
#define XM_READLINE_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "xmake/prefix.h"
#ifdef XM_CONFIG_API_HAVE_READLINE
#   include <stdio.h>   // on some OS (like centos) required
#   include <readline/readline.h>
#   include <readline/history.h>
#endif


#endif
