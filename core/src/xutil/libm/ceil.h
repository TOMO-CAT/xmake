#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */
#define xu_ceil(x) ((x) > 0 ? (xu_int32_t)((x) + 0.9999999999) : (xu_int32_t)(x))
