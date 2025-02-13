#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */
#define xu_floor(x) ((x) > 0 ? (xu_int32_t)(x) : (xu_int32_t)((x)-0.9999999999))
