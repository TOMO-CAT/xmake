#pragma once

/* *******************************************************
 * includes
 */
#include "semver.h"
#include "xmake/prefix.h"

/* *******************************************************
 * interfaces
 */

/* push struct semver
 *
 * @param lua       the lua context
 * @param semver    the semver struct
 *
 */
xu_void_t lua_pushsemver(lua_State* lua, semver_t const* semver);
